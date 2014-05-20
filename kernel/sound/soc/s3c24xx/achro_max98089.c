/*
 *  sound/soc/s3c24xx/achro_max98089.c
 *
 *  Copyright (c) 2009 Samsung Electronics Co. Ltd
 *
 *  This program is free software; you can redistribute  it and/or  modify it
 *  under  the terms of  the GNU General  Public License as published by the
 *  Free Software Foundation;  either version 2 of the  License, or (at your
 *  option) any later version.
 */

#include <linux/platform_device.h>
#include <linux/clk.h>
#include <linux/io.h>

#include <sound/soc.h>
#include <sound/soc-dapm.h>
#include <sound/pcm.h>
#include <sound/pcm_params.h>

#include <mach/regs-clock.h>
#include <mach/map.h>
#include <mach/regs-audss.h>

#include "../codecs/max98088.h"
#include "s3c-dma.h"
#include "s3c64xx-i2s.h"
#include "s3c-dma-wrapper.h"


static struct platform_device *achro_snd_device;

static int set_epll_rate(unsigned long rate)
{
	struct clk *fout_epll;

	fout_epll = clk_get(NULL, "fout_epll");
	if (IS_ERR(fout_epll)) {
		printk(KERN_ERR "%s: failed to get fout_epll\n", __func__);
		return -ENOENT;
	}

	if (rate == clk_get_rate(fout_epll))
		goto out;

	clk_set_rate(fout_epll, rate);
out:
	clk_put(fout_epll);

	return 0;
}

static int achro_max98089_hw_params(struct snd_pcm_substream *substream,
			      struct snd_pcm_hw_params *params)
{
	struct snd_soc_pcm_runtime *rtd = substream->private_data;
	struct snd_soc_dai *codec_dai = rtd->dai->codec_dai;
	struct snd_soc_dai *cpu_dai = rtd->dai->cpu_dai;
	unsigned int rclk, psr=1;
	int bfs, rfs, ret;
	unsigned long epll_out_rate;
	unsigned int  value = 0;
	unsigned int bit_per_sample, sample_rate;

	switch (params_format(params)) {
	case SNDRV_PCM_FORMAT_U24:
	case SNDRV_PCM_FORMAT_S24:
		bfs = 48;
		bit_per_sample = 24;
		break;
	case SNDRV_PCM_FORMAT_U16_LE:
	case SNDRV_PCM_FORMAT_S16_LE:
		bfs = 32;
		bit_per_sample = 16;
		break;
	default:
		return -EINVAL;
	}

	/* For resample */
	value = params_rate(params);
//	if(value != 44100) {
//		printk("%s: sample rate %d --> 44100\n", __func__, value);
//		value = 44100;
//	}
	sample_rate = value;

	/* The Fvco for WM8580 PLLs must fall within [90,100]MHz.
	 * This criterion can't be met if we request PLL output
	 * as {8000x256, 64000x256, 11025x256}Hz.
	 * As a wayout, we rather change rfs to a minimum value that
	 * results in (params_rate(params) * rfs), and itself, acceptable
	 * to both - the CODEC and the CPU.
	 */
	switch (value) {
	case 16000:
	case 22050:
	case 24000:
	case 32000:
	case 44100:
	case 48000:
	case 88200:
	case 96000:
		if (bfs == 48)
			rfs = 384;
		else
			rfs = 256;
		break;
	case 64000:
		rfs = 384;
		break;
	case 8000:
	case 11025:
	case 12000:
		if (bfs == 48)
			rfs = 768;
		else
			rfs = 512;
		break;
	default:
		return -EINVAL;
	}

	rclk = value * rfs;

	switch (rclk) {
	case 4096000:
	case 5644800:
	case 6144000:
	case 8467200:
	case 9216000:
		psr = 8;
		break;
	case 8192000:
	case 11289600:
	case 12288000:
	case 16934400:
	case 18432000:
		psr = 4;
		break;
	case 22579200:
	case 24576000:
	case 33868800:
	case 36864000:
		psr = 2;
		break;
	case 67737600:
	case 73728000:
		psr = 1;
		break;
	default:
		printk("Not yet supported!\n");
		return -EINVAL;
	}

	printk("IIS Audio: %uBits Stereo %uHz\n", bit_per_sample, sample_rate);

#ifdef CONFIG_SND_S5P_RP
	/* Find fastest clock for MP3 decoding & post-effect processing.
	   Maximux clock is 192MHz for AudioSubsystem.
	 */
	for (epll_out_rate = 0; ; psr++) {
		epll_out_rate = rclk * psr;
		if (epll_out_rate > 192000000) {
			psr--;
			epll_out_rate = rclk * psr;
			break;
		}
	}
#else
	epll_out_rate = rclk * psr;
#endif

	/* Set EPLL clock rate */
	ret = set_epll_rate(epll_out_rate);
	if (ret < 0)
		return ret;

	/* Set the AP DAI configuration */
	ret = snd_soc_dai_set_fmt(cpu_dai, SND_SOC_DAIFMT_I2S|SND_SOC_DAIFMT_NB_NF|SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0)
		return ret;

	/* Set the Codec DAI configuration */
	ret = snd_soc_dai_set_fmt(codec_dai, SND_SOC_DAIFMT_I2S|SND_SOC_DAIFMT_NB_NF|SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_sysclk(codec_dai, 0, rclk, SND_SOC_CLOCK_IN);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_sysclk(cpu_dai, S3C64XX_CLKSRC_CDCLK,
					0, SND_SOC_CLOCK_OUT);
	if (ret < 0)
		return ret;

	/* We use MUX for basic ops in SoC-Master mode */
	ret = snd_soc_dai_set_sysclk(cpu_dai, S3C64XX_CLKSRC_MUX,
					0, SND_SOC_CLOCK_IN);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_clkdiv(cpu_dai, S3C_I2SV2_DIV_PRESCALER, psr-1);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_clkdiv(cpu_dai, S3C_I2SV2_DIV_BCLK, bfs);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_clkdiv(cpu_dai, S3C_I2SV2_DIV_RCLK, rfs);
	if (ret < 0)
		return ret;

	return 0;
}

//---------------------------------------------------------------------------------
//---------------------------------------------------------------------------------
int	max98089_routing(u8 path)
{
	struct snd_soc_device *socdev = platform_get_drvdata(achro_snd_device);
	struct snd_soc_codec *codec = socdev->card->codec;
	struct max98088_priv *max98088 = snd_soc_codec_get_drvdata(codec);
	
	switch(path)
	{
		case 0 :
			max98089_disable_playback_path(codec, HP);
			max98089_set_playback_speaker(codec);
			max98088->cur_path = SPK;
			break;
		case 1 :
		case 2 :
			max98089_disable_playback_path(codec, SPK);
			max98089_set_playback_headset(codec);
			max98088->cur_path = HP;
			break;
		case 3 :
			max98089_disable_playback_path(codec, SPK);
//			max98088->cur_path = TV_OUT;
		default :
			break;
	}
	
	return 0;
}
EXPORT_SYMBOL(max98089_routing);

static struct snd_soc_ops achro_max98089_ops = {
	.hw_params = achro_max98089_hw_params,
};

static struct snd_soc_dai_link achro_dai[] = {
{
	.name = "MAX98089 Playback",
	.stream_name = "Playback",
	.cpu_dai = &s3c64xx_i2s_v4_dai,
	.codec_dai = &max98088_dai,
	.ops = &achro_max98089_ops,
},
{
	.name = "MAX98089 Capture",
	.stream_name = "Capture",
	.cpu_dai = &s3c64xx_i2s_v4_dai,
	.codec_dai = &max98088_dai,
	.ops = &achro_max98089_ops,
},
};

static struct snd_soc_card achro = {
	.name = "achro",
	.platform = &s3c_dma_wrapper,
//	.platform = &s3c24xx_soc_platform,
	.dai_link = achro_dai,
	.num_links = ARRAY_SIZE(achro_dai),
};

static struct snd_soc_device achro_snd_devdata = {
	.card = &achro,
	.codec_dev = &soc_codec_dev_max98088,
};

static int __init achro_audio_init(void)
{
	int ret;
	u32 val;

	/* We use I2SCLK for rate generation, so set EPLLout as
	 * the parent of I2SCLK.
	 */
	val = readl(S5P_CLKSRC_AUDSS);
	val &= ~(0x3<<2);
	val |= (1<<0);
	writel(val, S5P_CLKSRC_AUDSS);

	val = readl(S5P_CLKGATE_AUDSS);
	val |= (0x7f<<0);
	writel(val, S5P_CLKGATE_AUDSS);

	achro_snd_device = platform_device_alloc("soc-audio", 0);
	if (!achro_snd_device)
		return -ENOMEM;

	platform_set_drvdata(achro_snd_device, &achro_snd_devdata);
	achro_snd_devdata.dev = &achro_snd_device->dev;

	ret = platform_device_add(achro_snd_device);
	if (ret)
		platform_device_put(achro_snd_device);

	set_epll_rate(11289600*4);

	/* Set the Codec DAI configuration */
	ret = snd_soc_dai_set_fmt(&max98088_dai, SND_SOC_DAIFMT_I2S
					 | SND_SOC_DAIFMT_NB_NF
					 | SND_SOC_DAIFMT_CBS_CFS);
	if (ret < 0)
		return ret;
	/* Set the AP DAI configuration */
	ret = snd_soc_dai_set_fmt(&s3c64xx_i2s_v4_dai, SND_SOC_DAIFMT_I2S
					 | SND_SOC_DAIFMT_NB_NF
					 | SND_SOC_DAIFMT_CBS_CFS);


	ret = snd_soc_dai_set_sysclk(&max98088_dai, 0, 11289600, SND_SOC_CLOCK_IN);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_sysclk(&s3c64xx_i2s_v4_dai, S3C64XX_CLKSRC_CDCLK,
					0, SND_SOC_CLOCK_OUT);
	if (ret < 0)
		return ret;

	/* We use MUX for basic ops in SoC-Master mode */
	ret = snd_soc_dai_set_sysclk(&s3c64xx_i2s_v4_dai, S3C64XX_CLKSRC_MUX,
					0, SND_SOC_CLOCK_IN);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_clkdiv(&s3c64xx_i2s_v4_dai, S3C_I2SV2_DIV_PRESCALER, 4-1);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_clkdiv(&s3c64xx_i2s_v4_dai, S3C_I2SV2_DIV_BCLK, 32);
	if (ret < 0)
		return ret;

	ret = snd_soc_dai_set_clkdiv(&s3c64xx_i2s_v4_dai, S3C_I2SV2_DIV_RCLK, 256);
	if (ret < 0)
		return ret;


	return ret;
}

static void __exit achro_audio_exit(void)
{
	platform_device_unregister(achro_snd_device);
}

module_init(achro_audio_init);
module_exit(achro_audio_exit);

MODULE_AUTHOR("Hardkernel, <@samsung.com>");
MODULE_DESCRIPTION("ALSA SoC ACHRO MAX98089(Codec Slave) Audio Driver");
MODULE_LICENSE("GPL");
