/* linux/drivers/media/video/s5k4eagx.h
 *
 * Copyright (c) 2010 Hardkernel Co., Ltd.
 * 		http://www.hardkernel.com/
 *
 * Driver for S5K4EAGX (SXGA camera) from Samsung Electronics
 * 1/4" 5Mp CMOS Image Sensor SoC with an Embedded Image Processor
 * supporting PVI
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/videodev2_samsung.h>
#define S5K4EAGX_COMPLETE
//#undef S5K4EAGX_COMPLETE
#ifndef __S5K4EAGX_H__
#define __S5K4EAGX_H__

struct s5k4eagx_reg {
	unsigned short addr;
	unsigned short val;
};

struct s5k4eagx_regset_type {
	unsigned char *regset;
	int len;
};

/*
 * Macro
 */
#define REGSET_LENGTH(x)	(sizeof(x)/sizeof(s5k4eagx_reg))

/*
 * Host S/W Register interface (0x70000000-0x70002000)
 */
/* Initialization section */
#define S5K4EAGX_Speed_368Mbps		0
#define S5K4EAGX_Speed_464Mbps		1
#define S5K4EAGX_Speed_552Mbps		2
#define S5K4EAGX_Speed_648Mbps		3
#define S5K4EAGX_Speed_736Mbps		4
#define S5K4EAGX_Speed_832Mbps		5
#define S5K4EAGX_Speed_920Mbps		6

#define S5K4EAGX4Khz_0Mhz		0x0000
#define S5K4EAGX4Khz_46Mhz		0x2CEC
#define S5K4EAGX4Khz_48Mhz		0x2EE0
#define S5K4EAGX4Khz_58Mhz		0x38A4
#define S5K4EAGX4Khz_69Mhz		0x4362
#define S5K4EAGX4Khz_70Mhz		0x445C
#define S5K4EAGX4Khz_81Mhz		0x4F1A
#define S5K4EAGX4Khz_91Mhz		0x58DE
#define S5K4EAGX4Khz_92Mhz		0x59D8
#define S5K4EAGX4Khz_93Mhz		0x5AD2
#define S5K4EAGX4Khz_104Mhz		0x6590
#define S5K4EAGX4Khz_115Mhz		0x704E

#define S5K4EAGXFrTime_30fps		0x014D	/*  33.3ms -> 30 fps */
#define S5K4EAGXFrTime_15fps		0x029A	/*  66.6ms -> 15 fps */
#define S5K4EAGXFrTime_7P5fps		0x0535	/*  133.3ms -> 7.5 fps */
#define S5K4EAGXFrTime_1P5fps		0x1964	/*  650.0ms -> 1.5 fps */
/*=====================================*/
/*========Register map for S5K4EAGX EVT1(Don't modify)===========*/
#define S5K4EAGX_REG_TC_IPRM_InClockLSBs				0x0238
#define S5K4EAGX_REG_TC_IPRM_InClockMSBs				0x023A
#define S5K4EAGX_REG_TC_IPRM_UseNPviClocks			0x0252
#define S5K4EAGX_REG_TC_IPRM_UseNMipiClocks			0x0254
#define S5K4EAGX_REG_TC_IPRM_NumberOfMipiLanes			0x0256
#define S5K4EAGX_REG_TC_IPRM_OpClk4KHz_0				0x025A
#define S5K4EAGX_REG_TC_IPRM_MinOutRate4KHz_0			0x025C
#define S5K4EAGX_REG_TC_IPRM_MaxOutRate4KHz_0			0x025E
#define S5K4EAGX_REG_TC_IPRM_OpClk4KHz_1				0x0260
#define S5K4EAGX_REG_TC_IPRM_MinOutRate4KHz_1			0x0262
#define S5K4EAGX_REG_TC_IPRM_MaxOutRate4KHz_1			0x0264
#define S5K4EAGX_REG_TC_IPRM_InitParamsUpdated			0x026E
#define S5K4EAGX_REG_TC_GP_EnablePreview				0x0280
#define S5K4EAGX_REG_TC_GP_EnablePreviewChanged			0x0282
#define S5K4EAGX_REG_TC_GP_NewConfigSync				0x0290
#define S5K4EAGX_REG_TC_GP_ActivePrevConfig			0x02A4
#define S5K4EAGX_REG_TC_GP_PrevConfigChanged			0x02A6
#define S5K4EAGX_REG_TC_GP_PrevOpenAfterChange			0x02A8
#define S5K4EAGX_REG_0TC_PCFG_usWidth				0x02E2
#define S5K4EAGX_REG_0TC_PCFG_usHeight				0x02E4
#define S5K4EAGX_REG_0TC_PCFG_Format				0x02E6
#define S5K4EAGX_REG_0TC_PCFG_usMaxOut4KHzRate			0x02E8
#define S5K4EAGX_REG_0TC_PCFG_usMinOut4KHzRate			0x02EA
#define S5K4EAGX_REG_0TC_PCFG_PVIMask				0x02F0
#define S5K4EAGX_REG_0TC_PCFG_uClockInd				0x02F8
#define S5K4EAGX_REG_0TC_PCFG_FrRateQualityType			0x02FC
#define S5K4EAGX_REG_0TC_PCFG_usFrTimeType			0x02FA
#define S5K4EAGX_REG_0TC_PCFG_usMaxFrTimeMsecMult10		0x02FE
#define S5K4EAGX_REG_0TC_PCFG_usMinFrTimeMsecMult10		0x0300

#define S5K4EAGX_PCLK_MIN	S5K4EAGX4Khz_48Mhz
#define S5K4EAGX_PCLK_MAX	S5K4EAGX4Khz_48Mhz

#define S5K4EAGX_FrTime_MAX	S5K4EAGXFrTime_30fps
/*
 * User defined commands
 */
 
/*
 * V 4 L 2   F I M C   E X T E N S I O N S
 *
*/
#define V4L2_CID_ROTATION                   (V4L2_CID_PRIVATE_BASE + 0)
#define V4L2_CID_PADDR_Y                    (V4L2_CID_PRIVATE_BASE + 1)
#define V4L2_CID_PADDR_CB                   (V4L2_CID_PRIVATE_BASE + 2)
#define V4L2_CID_PADDR_CR                   (V4L2_CID_PRIVATE_BASE + 3)
#define V4L2_CID_PADDR_CBCR                 (V4L2_CID_PRIVATE_BASE + 4)
#define V4L2_CID_STREAM_PAUSE               (V4L2_CID_PRIVATE_BASE + 53)

#define V4L2_CID_CAM_JPEG_MAIN_SIZE         (V4L2_CID_PRIVATE_BASE + 32)
#define V4L2_CID_CAM_JPEG_MAIN_OFFSET       (V4L2_CID_PRIVATE_BASE + 33)
#define V4L2_CID_CAM_JPEG_THUMB_SIZE        (V4L2_CID_PRIVATE_BASE + 34)
#define V4L2_CID_CAM_JPEG_THUMB_OFFSET      (V4L2_CID_PRIVATE_BASE + 35)
#define V4L2_CID_CAM_JPEG_POSTVIEW_OFFSET   (V4L2_CID_PRIVATE_BASE + 36)
#define V4L2_CID_CAM_JPEG_QUALITY           (V4L2_CID_PRIVATE_BASE + 37)

#define V4L2_CID_CAM_OUT_PRESET           (V4L2_CID_PRIVATE_BASE + 100)   //ydongyol: 2010.11.18
#define V4L2_CID_CAM_ZOOM_CONTROL           (V4L2_CID_PRIVATE_BASE + 101) //ydongyol: added 2010.11.23



/*  Camera class control IDs */
#define V4L2_CID_CAMERA_CLASS_BASE 	(V4L2_CTRL_CLASS_CAMERA | 0x900)
#define V4L2_CID_CAMERA_CLASS 		(V4L2_CTRL_CLASS_CAMERA | 1)

#define V4L2_CID_EXPOSURE_AUTO			(V4L2_CID_CAMERA_CLASS_BASE+1)

#define V4L2_CID_EXPOSURE_ABSOLUTE		(V4L2_CID_CAMERA_CLASS_BASE+2)
#define V4L2_CID_EXPOSURE_AUTO_PRIORITY		(V4L2_CID_CAMERA_CLASS_BASE+3)

#define V4L2_CID_PAN_RELATIVE			(V4L2_CID_CAMERA_CLASS_BASE+4)
#define V4L2_CID_TILT_RELATIVE			(V4L2_CID_CAMERA_CLASS_BASE+5)
#define V4L2_CID_PAN_RESET			(V4L2_CID_CAMERA_CLASS_BASE+6)
#define V4L2_CID_TILT_RESET			(V4L2_CID_CAMERA_CLASS_BASE+7)

#define V4L2_CID_PAN_ABSOLUTE			(V4L2_CID_CAMERA_CLASS_BASE+8)
#define V4L2_CID_TILT_ABSOLUTE			(V4L2_CID_CAMERA_CLASS_BASE+9)

#define V4L2_CID_FOCUS_ABSOLUTE			(V4L2_CID_CAMERA_CLASS_BASE+10)
#define V4L2_CID_FOCUS_RELATIVE			(V4L2_CID_CAMERA_CLASS_BASE+11)
#define V4L2_CID_FOCUS_AUTO			(V4L2_CID_CAMERA_CLASS_BASE+12)


///

#define TPATTERN_COLORBAR		(1)
#define TPATTERN_HORIZONTAL		(2)
#define TPATTERN_VERTICAL		(3)

#define V4L2_PIX_FMT_YVYU       v4l2_fourcc('Y', 'V', 'Y', 'U')

/* FOURCC for FIMC specific */
#define V4L2_PIX_FMT_VYUY       v4l2_fourcc('V', 'Y', 'U', 'Y')
#define V4L2_PIX_FMT_NV16       v4l2_fourcc('N', 'V', '1', '6')
#define V4L2_PIX_FMT_NV61       v4l2_fourcc('N', 'V', '6', '1')
#define V4L2_PIX_FMT_NV12T      v4l2_fourcc('T', 'V', '1', '2')


/* S/W defined features for tune */
#define REG_DELAY	0xFF	/* in ms */
#define REG_CMD		0xFFFF	/* Followed by command */

enum AUTO_FOCUS_MODE
{
	AUTO_FOCUS_BASE,
	AUTO_FOCUS_AUTO,
	AUTO_FOCUS_FIXED,
	AUTO_FOCUS_INFINITY,
	AUTO_FOCUS_MACRO,
//	AUTO_FOCUS_MAX,
};

//ydongyol: 2010.11.24  zoom control
typedef enum zoom_state{
		ZOOM_IDLE=0,
		ZOOM_STEPPING_UP,
		ZOOM_STEPPING_DOWN,
		ZOOM_ONE_STEP_UP,
		ZOOM_ONE_STEP_DOWN,
		ZOOM_GO_TARGET,
		ZOOM_FORCE_OUT_CROP
};

/* Following order should not be changed */
enum image_size_s5k4eagx {
	/* This SoC supports upto SXGA (1280*1024) */
#if 0
	QQVGA, /* 160*120*/
	QCIF, /* 176*144 */
	QVGA, /* 320*240 */
	CIF, /* 352*288 */
#endif
	VGA, /* 640*480 */
#if 0
	SVGA, /* 800*600 */
	HD720P, /* 1280*720 */
	SXGA, /* 1280*1024 */
#endif
};

/*
 * Following values describe controls of camera
 * in user aspect and must be match with index of s5k4eagx_regset[]
 * These values indicates each controls and should be used
 * to control each control
 */
enum s5k4eagx_control {
	S5K4EAGX_INIT,
	S5K4EAGX_EV,
	S5K4EAGX_AWB,
	S5K4EAGX_MWB,
	S5K4EAGX_EFFECT,
	S5K4EAGX_CONTRAST,
	S5K4EAGX_SATURATION,
	S5K4EAGX_SHARPNESS,
};

#define S5K4EAGX_REGSET(x)	{	\
	.regset = x,			\
	.len = sizeof(x)/sizeof(s5k4eagx_reg),}

unsigned char s5k4eagx_init_reg41[][4] = {
/*
0010 0001 //// Reset
1030 0000
0014 0001  
p100
*/
	{0x00, 0x2c, 0xD0, 0x00}, //offset setting
	{0x00, 0x2A, 0x00,0x12},	{0x0F, 0x12, 0x00,0x01},	// Reset
	{0x00, 0x2A, 0x10,0x30},	{0x0F, 0x12, 0x00,0x00},
	{0x00, 0x2A, 0x00,0x14},	{0x0F, 0x12, 0x00,0x01},
	{REG_DELAY, 100, 0, 0},	// p100
};

unsigned short s5k4eagx_init_reg42[][2] = {

{0x0028, 0x7000}, //// SET T&P (AF, StandBy leakge)
{0x002A, 0x31BC}, 
{0x0F12, 0xB538}, // 700031BC 
{0x0F12, 0x2218}, // 700031BE 
{0x0F12, 0xA00D}, // 700031C0 
{0x0F12, 0x6800}, // 700031C2 
{0x0F12, 0x9000}, // 700031C4 
{0x0F12, 0x466B}, // 700031C6 
{0x0F12, 0x8819}, // 700031C8 
{0x0F12, 0x480C}, // 700031CA 
{0x0F12, 0x8441}, // 700031CC 
{0x0F12, 0x8859}, // 700031CE 
{0x0F12, 0x8481}, // 700031D0 
{0x0F12, 0x2400}, // 700031D2 
{0x0F12, 0x84C4}, // 700031D4 
{0x0F12, 0x2103}, // 700031D6 
{0x0F12, 0x8501}, // 700031D8 
{0x0F12, 0x4909}, // 700031DA 
{0x0F12, 0x4809}, // 700031DC 
{0x0F12, 0xF000}, // 700031DE 
{0x0F12, 0xF8A1}, // 700031E0 
{0x0F12, 0x4909}, // 700031E2 
{0x0F12, 0x4809}, // 700031E4 
{0x0F12, 0x0022}, // 700031E6 
{0x0F12, 0xF000}, // 700031E8 
{0x0F12, 0xF89C}, // 700031EA 
{0x0F12, 0x4908}, // 700031EC 
{0x0F12, 0x4809}, // 700031EE 
{0x0F12, 0x6201}, // 700031F0 
{0x0F12, 0xBC38}, // 700031F2 
{0x0F12, 0xBC08}, // 700031F4 
{0x0F12, 0x4718}, // 700031F6 
{0x0F12, 0x4EA3}, // 700031F8 
{0x0F12, 0x00D6}, // 700031FA 
{0x0F12, 0x1C18}, // 700031FC 
{0x0F12, 0x7000}, // 700031FE 
{0x0F12, 0x3219}, // 70003200 
{0x0F12, 0x7000}, // 70003202 
{0x0F12, 0x0D2B}, // 70003204 
{0x0F12, 0x0001}, // 70003206 
{0x0F12, 0x3261}, // 70003208 
{0x0F12, 0x7000}, // 7000320A 
{0x0F12, 0xB375}, // 7000320C 
{0x0F12, 0x0000}, // 7000320E 
{0x0F12, 0x32E9}, // 70003210 
{0x0F12, 0x7000}, // 70003212 
{0x0F12, 0x00C0}, // 70003214 
{0x0F12, 0x7000}, // 70003216 
{0x0F12, 0xB5F8}, // 70003218 
{0x0F12, 0x0004}, // 7000321A 
{0x0F12, 0x2601}, // 7000321C 
{0x0F12, 0x0035}, // 7000321E 
{0x0F12, 0x0020}, // 70003220 
{0x0F12, 0x3810}, // 70003222 
{0x0F12, 0x4086}, // 70003224 
{0x0F12, 0x40A5}, // 70003226 
{0x0F12, 0x4F39}, // 70003228 
{0x0F12, 0x2C10}, // 7000322A 
{0x0F12, 0xDA03}, // 7000322C 
{0x0F12, 0x8838}, // 7000322E 
{0x0F12, 0x43A8}, // 70003230 
{0x0F12, 0x8038}, // 70003232 
{0x0F12, 0xE002}, // 70003234 
{0x0F12, 0x8878}, // 70003236 
{0x0F12, 0x43B0}, // 70003238 
{0x0F12, 0x8078}, // 7000323A 
{0x0F12, 0xF000}, // 7000323C 
{0x0F12, 0xF87A}, // 7000323E 
{0x0F12, 0x2C10}, // 70003240 
{0x0F12, 0xDA03}, // 70003242 
{0x0F12, 0x8838}, // 70003244 
{0x0F12, 0x4328}, // 70003246 
{0x0F12, 0x8038}, // 70003248 
{0x0F12, 0xE002}, // 7000324A 
{0x0F12, 0x8878}, // 7000324C 
{0x0F12, 0x4330}, // 7000324E 
{0x0F12, 0x8078}, // 70003250 
{0x0F12, 0x4930}, // 70003252 
{0x0F12, 0x2000}, // 70003254 
{0x0F12, 0x8188}, // 70003256 
{0x0F12, 0x80C8}, // 70003258 
{0x0F12, 0xBCF8}, // 7000325A 
{0x0F12, 0xBC08}, // 7000325C 
{0x0F12, 0x4718}, // 7000325E 
{0x0F12, 0xB5F8}, // 70003260 
{0x0F12, 0x2407}, // 70003262 
{0x0F12, 0x2C06}, // 70003264 
{0x0F12, 0xD036}, // 70003266 
{0x0F12, 0x2C07}, // 70003268 
{0x0F12, 0xD034}, // 7000326A 
{0x0F12, 0x4A2A}, // 7000326C 
{0x0F12, 0x8BD1}, // 7000326E 
{0x0F12, 0x2900}, // 70003270 
{0x0F12, 0xD02B}, // 70003272 
{0x0F12, 0x00A0}, // 70003274 
{0x0F12, 0x1885}, // 70003276 
{0x0F12, 0x4828}, // 70003278 
{0x0F12, 0x6DEE}, // 7000327A 
{0x0F12, 0x8A80}, // 7000327C 
{0x0F12, 0x4286}, // 7000327E 
{0x0F12, 0xD924}, // 70003280 
{0x0F12, 0x002F}, // 70003282 
{0x0F12, 0x3780}, // 70003284 
{0x0F12, 0x69F8}, // 70003286 
{0x0F12, 0xF000}, // 70003288 
{0x0F12, 0xF85C}, // 7000328A 
{0x0F12, 0x1C71}, // 7000328C 
{0x0F12, 0x0280}, // 7000328E 
{0x0F12, 0xF000}, // 70003290 
{0x0F12, 0xF858}, // 70003292 
{0x0F12, 0x0006}, // 70003294 
{0x0F12, 0x4A20}, // 70003296 
{0x0F12, 0x0060}, // 70003298 
{0x0F12, 0x1880}, // 7000329A 
{0x0F12, 0x8D80}, // 7000329C 
{0x0F12, 0x0A01}, // 7000329E 
{0x0F12, 0x0600}, // 700032A0 
{0x0F12, 0x0E00}, // 700032A2 
{0x0F12, 0x1A08}, // 700032A4 
{0x0F12, 0x0400}, // 700032A6 
{0x0F12, 0x1400}, // 700032A8 
{0x0F12, 0xF000}, // 700032AA 
{0x0F12, 0xF851}, // 700032AC 
{0x0F12, 0x0001}, // 700032AE 
{0x0F12, 0x6DEA}, // 700032B0 
{0x0F12, 0x6FE8}, // 700032B2 
{0x0F12, 0x4351}, // 700032B4 
{0x0F12, 0x1A10}, // 700032B6 
{0x0F12, 0x0300}, // 700032B8 
{0x0F12, 0x1C49}, // 700032BA 
{0x0F12, 0xF000}, // 700032BC 
{0x0F12, 0xF842}, // 700032BE 
{0x0F12, 0x0401}, // 700032C0 
{0x0F12, 0x0430}, // 700032C2 
{0x0F12, 0x0C00}, // 700032C4 
{0x0F12, 0x4308}, // 700032C6 
{0x0F12, 0x61F8}, // 700032C8 
{0x0F12, 0xE004}, // 700032CA 
{0x0F12, 0x00A1}, // 700032CC 
{0x0F12, 0x4814}, // 700032CE 
{0x0F12, 0x1889}, // 700032D0 
{0x0F12, 0x3180}, // 700032D2 
{0x0F12, 0x61C8}, // 700032D4 
{0x0F12, 0x1E64}, // 700032D6 
{0x0F12, 0xD2C4}, // 700032D8 
{0x0F12, 0x2006}, // 700032DA 
{0x0F12, 0xF000}, // 700032DC 
{0x0F12, 0xF840}, // 700032DE 
{0x0F12, 0x2007}, // 700032E0 
{0x0F12, 0xF000}, // 700032E2 
{0x0F12, 0xF83D}, // 700032E4 
{0x0F12, 0xE7B8}, // 700032E6 
{0x0F12, 0xB510}, // 700032E8 
{0x0F12, 0xF000}, // 700032EA 
{0x0F12, 0xF841}, // 700032EC 
{0x0F12, 0x2800}, // 700032EE 
{0x0F12, 0xD00C}, // 700032F0 
{0x0F12, 0x4809}, // 700032F2 
{0x0F12, 0x8B81}, // 700032F4 
{0x0F12, 0x0089}, // 700032F6 
{0x0F12, 0x1808}, // 700032F8 
{0x0F12, 0x6DC1}, // 700032FA 
{0x0F12, 0x4807}, // 700032FC 
{0x0F12, 0x8A80}, // 700032FE 
{0x0F12, 0x4281}, // 70003300 
{0x0F12, 0xD903}, // 70003302 
{0x0F12, 0x2001}, // 70003304 
{0x0F12, 0xBC10}, // 70003306 
{0x0F12, 0xBC08}, // 70003308 
{0x0F12, 0x4718}, // 7000330A 
{0x0F12, 0x2000}, // 7000330C 
{0x0F12, 0xE7FA}, // 7000330E 
{0x0F12, 0x1100}, // 70003310 
{0x0F12, 0xD000}, // 70003312 
{0x0F12, 0x0060}, // 70003314 
{0x0F12, 0xD000}, // 70003316 
{0x0F12, 0x2B14}, // 70003318 
{0x0F12, 0x7000}, // 7000331A 
{0x0F12, 0x158C}, // 7000331C 
{0x0F12, 0x7000}, // 7000331E 
{0x0F12, 0xFFFF}, // 70003320 
{0x0F12, 0x0000}, // 70003322 
{0x0F12, 0x4778}, // 70003324 
{0x0F12, 0x46C0}, // 70003326 
{0x0F12, 0xC000}, // 70003328 
{0x0F12, 0xE59F}, // 7000332A 
{0x0F12, 0xFF1C}, // 7000332C 
{0x0F12, 0xE12F}, // 7000332E 
{0x0F12, 0x1283}, // 70003330 
{0x0F12, 0x0001}, // 70003332 
{0x0F12, 0x4778}, // 70003334 
{0x0F12, 0x46C0}, // 70003336 
{0x0F12, 0xC000}, // 70003338 
{0x0F12, 0xE59F}, // 7000333A 
{0x0F12, 0xFF1C}, // 7000333C 
{0x0F12, 0xE12F}, // 7000333E 
{0x0F12, 0x0D2B}, // 70003340 
{0x0F12, 0x0001}, // 70003342 
{0x0F12, 0x4778}, // 70003344 
{0x0F12, 0x46C0}, // 70003346 
{0x0F12, 0xF004}, // 70003348 
{0x0F12, 0xE51F}, // 7000334A 
{0x0F12, 0x24B8}, // 7000334C 
{0x0F12, 0x0001}, // 7000334E 
{0x0F12, 0x4778}, // 70003350 
{0x0F12, 0x46C0}, // 70003352 
{0x0F12, 0xC000}, // 70003354 
{0x0F12, 0xE59F}, // 70003356 
{0x0F12, 0xFF1C}, // 70003358 
{0x0F12, 0xE12F}, // 7000335A 
{0x0F12, 0x2E39}, // 7000335C 
{0x0F12, 0x0000}, // 7000335E 
{0x0F12, 0x4778}, // 70003360 
{0x0F12, 0x46C0}, // 70003362 
{0x0F12, 0xC000}, // 70003364 
{0x0F12, 0xE59F}, // 70003366 
{0x0F12, 0xFF1C}, // 70003368 
{0x0F12, 0xE12F}, // 7000336A 
{0x0F12, 0xB34D}, // 7000336C 
{0x0F12, 0x0000}, // 7000336E 
{0x0F12, 0x4778}, // 70003370 
{0x0F12, 0x46C0}, // 70003372 
{0x0F12, 0xC000}, // 70003374 
{0x0F12, 0xE59F}, // 70003376 
{0x0F12, 0xFF1C}, // 70003378 
{0x0F12, 0xE12F}, // 7000337A 
{0x0F12, 0xB459}, // 7000337C // End of Trap and Patch (Last : 7000337Eh)// TNP_HW_STBY_PLL_WR
{0x0F12, 0x0000}, // 7000337E // Total Size 452 (0x01C4)// TNP_AF_MINSTAT_FIX

//{0x1000, 0x0001}, 

};


unsigned short s5k4eagx_init_reg43[][2] = {

{0x0028, 0xD000}, //// SET CIS/APS/ANALOG
{0x002A, 0xF404},   
{0x0F12, 0x002A}, // ADC SAT(450mV): revised by Ana 090130  
{0x002A, 0xF46E}, 
{0x0F12, 0x0002}, // CDS TEST [0]SR/SS EN: revised by Ana 090126// [1]S1 H, [2]LDB H, [3]clp H// [4]S34 P X, [5]S24 N X
{0x002A, 0xF45A},   
{0x0F12, 0x0002}, // LD LDB EN : revised by Ana 090126  
{0x002A, 0xF40E},   
{0x0F12, 0x0004}, // RMP REG 1.8V: revised by Ana 090126
{0x002A, 0xF40C},   
{0x0F12, 0x0020}, // rmp_option(RMP_INIT_DAC): revised by Ana 090126
{0x002A, 0xF420},   
{0x0F12, 0x0076}, // COMP(CDS) bias [7:4] comp2, [3:0] comp1: revised by Ana 090126
{0x0F12, 0x0005}, // pix current bias 
{0x002A, 0xF426}, 
{0x0F12, 0x00D4}, // CLP level
{0x002A, 0xF460}, 
{0x0F12, 0x0001}, // CLP on: revised by Ana 090126
{0x002A, 0xE304},   
{0x0F12, 0x0081}, // ADC OFFSET 128: revised by Ana 090126 
{0x002A, 0xE308},   
{0x0F12, 0x0081}, // ADC DEFAULT 128: revised by Ana 090209
{0x002A, 0xE506}, // SET ADLC   
{0x0F12, 0x0093}, // ADLC [7]FL,[6:5]FM,: revised by Ana 090126 // [4]F,[3:2]LM,[1]L,[0]CH  
{0x002A, 0xE402}, 
{0x0F12, 0x040C}, // ADLC BPR EN[10], th 12: revised by Ana 090126
{0x002A, 0xF42A}, 
{0x0F12, 0x0080}, // ALL TX mode enable(ref_option[7]): revised by Ana 090130
{0x002A, 0xF408}, 
{0x0F12, 0x000E}, // aig_sig_mx: revised by Ana 090209
{0x0F12, 0x0007}, // aig_rst_mx: revised by Ana 090209  
{0x002A, 0xF400},   
{0x0F12, 0x0007}, // aig_off_rst1: revised by Ana 090209
{0x002A, 0xF43C}, // SET DOUBLER  
{0x0F12, 0x0001}, // aig_pd_inrush_ctrl: revised by Ana 090126
{0x002A, 0xF440},   
{0x0F12, 0x0042}, // aig_rosc_tune_ncp[7:4], aig_rosc_tune_cp[3:0]: revised by Ana 090126 
{0x002A, 0xF444},   
{0x0F12, 0x0008}, // aig_reg_tune_pix 
{0x002A, 0xF448},   
{0x0F12, 0x0008}, // aig_reg_tune_ntg 
{0x002A, 0xF45C},   
{0x0F12, 0x0001}, // aig_dshut_en  
{0x002A, 0xF406}, // MULTIPLE SAMPLING   
{0x0F12, 0x0001}, // MS[2:0], 2 times pseudo-multiple sampling: revised by Ana 090130 
{0x002A, 0xF410},   
{0x0F12, 0x0000}, // MSOFF_EN=0 : no Multiple sampling if gain < 2 : revised by Ana 090609  
{0x002A, 0xF472}, // APS TIMING 
{0x0F12, 0x005C}, // aig_dstx_width(1us@46MHz)  : revised by Ana 090216  
{0x002A, 0xF470},   
{0x0F12, 0x0004},   
{0x002A, 0x1082},   
{0x0F12, 0x0155}, // D0_D4_cs10Set IO driving current 
{0x0F12, 0x0155}, // D9_D5_cs10Set IO driving current 
{0x0F12, 0x0555}, // GPIO_cd10 Set IO driving current 
{0x0F12, 0x0555}, // CLKs_cd10 Set IO driving current  
{0x0028, 0x7000},   
{0x002A, 0x182C},   
{0x0F12, 0x0007},   
{0x002A, 0x1834},   
{0x0F12, 0x0001},   
{0x002A, 0x183C},   
{0x0F12, 0x005C},   
{0x002A, 0x17B8}, // CDS TIMING 
{0x0F12, 0x34B0},   
{0x0F12, 0x7000}, // senHal_ContPtrs_senModesDataArr 
{0x002A, 0x34B0}, // High speed mode
{0x0F12, 0x0003}, 
{0x0F12, 0x05BA}, 
{0x0F12, 0x0001}, 
{0x0F12, 0x05BC}, 
{0x0F12, 0x0001}, 
{0x0F12, 0x05BC}, 
{0x0F12, 0x0014}, 
{0x0F12, 0x05BC}, 
{0x0F12, 0x0014}, 
{0x0F12, 0x05BC}, 
{0x0F12, 0x01AE}, 
{0x0F12, 0x0238}, 
{0x0F12, 0x01AE}, 
{0x0F12, 0x0238}, 
{0x0F12, 0x0238}, 
{0x0F12, 0x05BA}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0001}, 
{0x0F12, 0x01AC}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x01AC}, 
{0x0F12, 0x0240}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0001}, 
{0x0F12, 0x008C}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0001}, 
{0x0F12, 0x009E}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0001}, 
{0x0F12, 0x01AC}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x00A6}, 
{0x0F12, 0x05BA}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0292}, 
{0x0F12, 0x05BA}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x00D8}, 
{0x0F12, 0x01A8}, 
{0x0F12, 0x02C4}, 
{0x0F12, 0x05B4}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x01AA}, 
{0x0F12, 0x01D6}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x01BB}, 
{0x0F12, 0x01EF}, 
{0x0F12, 0x01F9}, 
{0x0F12, 0x0203}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x01CC}, 
{0x0F12, 0x01EF}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0001}, 
{0x0F12, 0x000D}, 
{0x0F12, 0x0001}, 
{0x0F12, 0x000D}, 
{0x0F12, 0x01AE}, 
{0x0F12, 0x01B6}, 
{0x0F12, 0x05BA}, 
{0x0F12, 0x05C6}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x00D4}, 
{0x0F12, 0x01AC}, 
{0x0F12, 0x02C0}, 
{0x0F12, 0x05B8}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x01AE}, 
{0x0F12, 0x05C9}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x002A, 0x17BC}, 
{0x0F12, 0x358A}, 
{0x0F12, 0x7000}, 
{0x002A, 0x358A}, 
{0x0F12, 0x0003}, 
{0x0F12, 0x081E}, 
{0x0F12, 0x0001}, 
{0x0F12, 0x0414}, 
{0x0F12, 0x040F}, 
{0x0F12, 0x0820}, 
{0x0F12, 0x0014}, 
{0x0F12, 0x040F}, 
{0x0F12, 0x0422}, 
{0x0F12, 0x0820}, 
{0x0F12, 0x0146}, 
{0x0F12, 0x01D0}, 
{0x0F12, 0x0554}, 
{0x0F12, 0x05DE}, 
{0x0F12, 0x01D0}, 
{0x0F12, 0x0410}, 
{0x0F12, 0x05DE}, 
{0x0F12, 0x081E}, 
{0x0F12, 0x0001}, 
{0x0F12, 0x0144}, 
{0x0F12, 0x0414}, 
{0x0F12, 0x0552}, 
{0x0F12, 0x0144}, 
{0x0F12, 0x01D8}, 
{0x0F12, 0x0552}, 
{0x0F12, 0x05E6}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0001}, 
{0x0F12, 0x008C}, 
{0x0F12, 0x040F}, 
{0x0F12, 0x049A}, 
{0x0F12, 0x0001}, 
{0x0F12, 0x009E}, 
{0x0F12, 0x040F}, 
{0x0F12, 0x04AC}, 
{0x0F12, 0x0001}, 
{0x0F12, 0x0144}, 
{0x0F12, 0x040F}, 
{0x0F12, 0x0552}, 
{0x0F12, 0x00A6}, 
{0x0F12, 0x0410}, 
{0x0F12, 0x04B4}, 
{0x0F12, 0x081E}, 
{0x0F12, 0x022A}, 
{0x0F12, 0x0410}, 
{0x0F12, 0x0638}, 
{0x0F12, 0x081E}, 
{0x0F12, 0x00D8}, 
{0x0F12, 0x0140}, 
{0x0F12, 0x0292}, 
{0x0F12, 0x040A}, 
{0x0F12, 0x04E6}, 
{0x0F12, 0x054E}, 
{0x0F12, 0x06A0}, 
{0x0F12, 0x0818}, 
{0x0F12, 0x0142}, 
{0x0F12, 0x016E}, 
{0x0F12, 0x040C}, 
{0x0F12, 0x0438}, 
{0x0F12, 0x0550}, 
{0x0F12, 0x057C}, 
{0x0F12, 0x014D}, 
{0x0F12, 0x0182}, 
{0x0F12, 0x018C}, 
{0x0F12, 0x0196}, 
{0x0F12, 0x0417}, 
{0x0F12, 0x044C}, 
{0x0F12, 0x0456}, 
{0x0F12, 0x0460}, 
{0x0F12, 0x055B}, 
{0x0F12, 0x0590}, 
{0x0F12, 0x059A}, 
{0x0F12, 0x05A4}, 
{0x0F12, 0x0158}, 
{0x0F12, 0x0182}, 
{0x0F12, 0x0422}, 
{0x0F12, 0x044C}, 
{0x0F12, 0x0566}, 
{0x0F12, 0x0590}, 
{0x0F12, 0x0001}, 
{0x0F12, 0x000D}, 
{0x0F12, 0x0001}, 
{0x0F12, 0x000D}, 
{0x0F12, 0x0146}, 
{0x0F12, 0x014E}, 
{0x0F12, 0x0410}, 
{0x0F12, 0x0418}, 
{0x0F12, 0x0554}, 
{0x0F12, 0x055C}, 
{0x0F12, 0x081E}, 
{0x0F12, 0x082A}, 
{0x0F12, 0x00D4}, 
{0x0F12, 0x0144}, 
{0x0F12, 0x028E}, 
{0x0F12, 0x040E}, 
{0x0F12, 0x04E2}, 
{0x0F12, 0x0552}, 
{0x0F12, 0x069C}, 
{0x0F12, 0x081C}, 
{0x0F12, 0x0146}, 
{0x0F12, 0x0410}, 
{0x0F12, 0x0554}, 
{0x0F12, 0x0830}, 
{0x0F12, 0x0000}, 
{0x0F12, 0x0000}, 

};


unsigned short s5k4eagx_init_reg44[][2] = {
{0x0028, 0x7000},
{0x002A, 0x023C}, //// SET AF
{0x0F12, 0x0000}, // #REG_TC_IPRM_LedGpio// No Led Gpio 
{0x0F12, 0x0003}, // #REG_TC_IPRM_CM_Init_AfModeType  // AFModeType - 0:NONE, 2:VCM_PWM, 3:VCM_I2C  
{0x0F12, 0x0000}, // #REG_TC_IPRM_CM_Init_PwmConfig1  // No PWM
{0x0F12, 0x0000}, // #REG_TC_IPRM_CM_Init_PwmConfig2  
{0x0F12, 0x0031}, // #REG_TC_IPRM_CM_Init_GpioConfig1 // No GPIO Port,31 // Use GPIO3 for Enable Port 
{0x0F12, 0x0000}, // #REG_TC_IPRM_CM_Init_GpioConfig2  
{0x002A, 0x024C}, 
{0x0F12, 0x200C}, // #REG_TC_IPRM_CM_Init_Mi2cBits // Use GPIO1 for SCL, GPIO2 for SDA  
{0x0F12, 0x0320}, // #REG_TC_IPRM_CM_Init_Mi2cRateKhz // MI2C Speed : 400KHz
{0x002A, 0x02C4},   
{0x0F12, 0x0100}, // #REG_TC_AF_FstWinStartX 
{0x0F12, 0x00E3}, // #REG_TC_AF_FstWinStartY  
{0x0F12, 0x0200}, // #REG_TC_AF_FstWinSizeX  
{0x0F12, 0x0238}, // #REG_TC_AF_FstWinSizeY  
{0x0F12, 0x018C}, // #REG_TC_AF_ScndWinStartX 
{0x0F12, 0x0166}, // #REG_TC_AF_ScndWinStartY
{0x0F12, 0x00E6}, // #REG_TC_AF_ScndWinSizeX  
{0x0F12, 0x0132}, // #REG_TC_AF_ScndWinSizeY 
{0x0F12, 0x0001}, // #REG_TC_AF_WinSizesUpdated 
{0x002A, 0x0732},   
{0x0F12, 0x00FF}, // #skl_af_StatOvlpExpFactor
{0x002A, 0x15CA}, 
{0x0F12, 0x0003}, // #af_scene_usSaturatedScene 
{0x002A, 0x1540},   
{0x0F12, 0x1000}, // FineSearch Disable // af_search_usSingleAfFlags 
{0x002A, 0x154A},   
{0x0F12, 0x0004}, // #af_search_usFinePeakCount 
{0x002A, 0x14D8},   
{0x0F12, 0x0302}, // #af_pos_usFineStepNumSize  
{0x002A, 0x1536}, 
{0x0F12, 0x00C0}, // #af_search_usPeakThr  
{0x002A, 0x15C2}, // #AF High contrast Scene  
{0x0F12, 0x0200}, // #af_scene_usHighContrastThr
{0x0F12, 0x0000}, // #af_scene_usHighContrastWin (0 : Use Inner Window, 1 : Use Outer Window) 
{0x002A, 0x15C0}, 
{0x0F12, 0x0060}, // #af_scene_usSceneLowNormBrThr  
{0x002A, 0x1560},   
{0x0F12, 0x0280}, // #LowEdgeBoth GRAD af_search_usConfThr_4_  
{0x002A, 0x156C},   
{0x0F12, 0x0390}, // #LowLight HPF af_search_usConfThr_10_  
{0x0F12, 0x0320},   
{0x002A, 0x15A0},   
{0x0F12, 0x0230}, // #af_stat_usMinStatVal  
{0x002A, 0x14CC},   
{0x0F12, 0x0000}, // #af_pos_usHomePos 
{0x0F12, 0x9400}, // #af_pos_usLowConfPos  
{0x002A, 0x1640}, 
{0x0F12, 0x00A0}, // #afd_usParam[5]SlowMotionDelay 
{0x0F12, 0x0010}, // #afd_usParam[6]SlowMotionThreshold
{0x002A, 0x14E0}, 
{0x0F12, 0x0010}, // #af_pos_usTableLastInd // Steps  
{0x0F12, 0x0000}, // #af_pos_usTable_0_   // af_pos_usTable 
{0x0F12, 0x003C}, // #af_pos_usTable_1_
{0x0F12, 0x0041}, // #af_pos_usTable_2_ 
{0x0F12, 0x0046}, // #af_pos_usTable_3_ 
{0x0F12, 0x004B}, // #af_pos_usTable_4_  
{0x0F12, 0x0050}, // #af_pos_usTable_5_ 
{0x0F12, 0x0055}, // #af_pos_usTable_6_ 
{0x0F12, 0x005A}, // #af_pos_usTable_7_  
{0x0F12, 0x005F}, // #af_pos_usTable_8_  
{0x0F12, 0x0064}, // #af_pos_usTable_9_ 
{0x0F12, 0x0069}, // #af_pos_usTable_10_
{0x0F12, 0x006E}, // #af_pos_usTable_11_ 
{0x0F12, 0x0073}, // #af_pos_usTable_12_
{0x0F12, 0x0078}, // #af_pos_usTable_13_  
{0x0F12, 0x007D}, // #af_pos_usTable_14_
{0x0F12, 0x0086}, // #af_pos_usTable_15_  
{0x0F12, 0x0090}, // #af_pos_usTable_16_ AF Fail
{0x002A, 0x14D2}, 
{0x0F12, 0x0B00}, // #af_pos_usMacroStartEnd  
{0x002A, 0x02BC}, 
{0x0F12, 0x0003}, // #REG_TC_AF_AfCmd  
{0x002A, 0x2A2C}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0303}, 
{0x0F12, 0x0303}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0303}, 
{0x0F12, 0x0303}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0303}, 
{0x0F12, 0x0303}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0303}, 
{0x0F12, 0x0303}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0101}, 
{0x0F12, 0x0101}, 
{0x002A, 0x13C8}, 
{0x0F12, 0x003A}, // #TVAR_ae_BrAve  
{0x002A, 0x13CE}, 
{0x0F12, 0x000F}, // #ae_StatMode 
{0x002A, 0x05B8}, //// SET EXPOSURE&GAIN 
{0x0F12, 0x59D8}, // 6D60 // #lt_uMaxExp1
{0x0F12, 0x0000}, // (x4 8000 / 70ms)  
{0x0F12, 0xAFC9}, // C350 // #lt_uMaxExp2
{0x0F12, 0x0000}, // (x4 25000 / 250ms)
{0x0F12, 0x59D8}, // 6D60 // #lt_uCapMaxExp1
{0x0F12, 0x0000}, // (x4 8000 / 70ms)  
{0x0F12, 0xAFC9}, // C350 // #lt_uCapMaxExp2
{0x0F12, 0x0000}, // (x4 25000 / 250ms)
{0x002A, 0x05C8},   
{0x0F12, 0x0300}, // #lt_uMaxAnGain1
{0x0F12, 0x0500}, // #lt_uMaxAnGain2
{0x0F12, 0x0100}, // #lt_uMaxDGain  
{0x0F12, 0x1000}, // #lt_uMaxTotGain (Limit Gain)  
{0x002A, 0x059C}, 
{0x0F12, 0x0111}, // #lt_uLimitHigh 
{0x0F12, 0x00EF}, // #lt_uLimitLow
{0x002A, 0x14A6}, // #ae_GainsOut (for fast AE)
{0x0F12, 0x0001}, // #ae_GainOut_0_ 
{0x0F12, 0x0008}, // #ae_GainOut_1_ 
{0x0F12, 0x0015}, // #ae_GainOut_2_ 
{0x002A, 0x0532}, 
{0x0F12, 0x077F}, // Auto algorithm on
{0x002A, 0x0EE2},   
{0x0F12, 0x0001}, // #AFC_Default60Hz start auto
{0x002A, 0x0630},   
{0x0F12, 0x0000}, // #lt_ExpGain_uSubsamplingmode // Preview0: table off, 1: table[1] on, 2: table[2] on
{0x0F12, 0x0000}, // #lt_ExpGain_uNonSubsampling // capture
{0x002A, 0x05FC},  
{0x0F12, 0x0800}, // #lt_MBR_uMaxAnGain // set as 2 times lagerer than Normal Total gain  
{0x0F12, 0x0200}, // #lt_MBR_uMaxDigGain  
{0x0F12, 0x0001}, // #lt_MBR_ulExpIn_0_
{0x0F12, 0x0000}, 
{0x0F12, 0x0A3C}, // #lt_MBR_ulExpIn_1_ 
{0x0F12, 0x0000}, 
{0x0F12, 0x3408}, // #lt_MBR_ulExpIn_2_
{0x0F12, 0x0000}, 
{0x0F12, 0x764E}, // #lt_MBR_ulExpIn_3_ 
{0x0F12, 0x0000}, 
{0x0F12, 0xCB26}, // #lt_MBR_ulExpIn_4_ 
{0x0F12, 0x0000}, 
{0x002A, 0x0614}, 
{0x0F12, 0x0001}, // #lt_MBR_ulExpOut_0_  
{0x0F12, 0x0000},   
{0x0F12, 0x0516}, // #lt_MBR_ulExpOut_1_  
{0x0F12, 0x0000}, 
{0x0F12, 0x1A04}, // #lt_MBR_ulExpOut_2_
{0x0F12, 0x0000}, 
{0x0F12, 0x3408}, // #lt_MBR_ulExpOut_3_  
{0x0F12, 0x0000},   
{0x0F12, 0x6810}, // #lt_MBR_ulExpOut_4_  
{0x0F12, 0x0000}, 
{0x002A, 0x162C}, 
{0x0F12, 0x0001}, // #DIS_usHorThres
{0x0F12, 0x0001}, // #DIS_usVerThres
{0x002A, 0x162A}, 
{0x0F12, 0x0000}, // 1:enable DIS_DIS_enable
{0x002A, 0x08D4},   
{0x0F12, 0x0000}, // #wbt_bUseOutdoorASH  
{0x002A, 0x08F4}, 
{0x0F12, 0x0CE6}, // #TVAR_ash_pGAS_high
{0x0F12, 0x7000}, 
{0x002A, 0x08F8}, 
{0x0F12, 0x0D76}, // #TVAR_ash_pGAS_low 
{0x0F12, 0x7000},   
{0x002A, 0x0CE6},   
{0x0F12, 0x0f00},   
{0x0F12, 0x0000},   
{0x0F12, 0x000f},   
{0x0F12, 0x0f0f},   
{0x0F12, 0x000f},   
{0x0F12, 0x0f00},   
{0x0F12, 0x0f0f},   
{0x0F12, 0x0f00},   
{0x0F12, 0x000f},   
{0x0F12, 0x0000},   
{0x0F12, 0x000f},   
{0x0F12, 0x0f00},   
{0x0F12, 0x000f},   
{0x0F12, 0x000f},   
{0x0F12, 0x0f0f},   
{0x0F12, 0x0f00},   
{0x0F12, 0x0f00},   
{0x0F12, 0x000f},   
{0x0F12, 0x0f00},   
{0x0F12, 0x0000},   
{0x0F12, 0x000f},   
{0x0F12, 0x0f0f},   
{0x0F12, 0x000f},   
{0x0F12, 0x0f00},   
{0x0F12, 0x0f0f},   
{0x0F12, 0x0f00},   
{0x0F12, 0x000f},   
{0x0F12, 0x0000},   
{0x0F12, 0x000f},   
{0x0F12, 0x0f00},   
{0x0F12, 0x000f},   
{0x0F12, 0x000f},   
{0x0F12, 0x0f0f},   
{0x0F12, 0x0f00},   
{0x0F12, 0x0f00},   
{0x0F12, 0x000f},   
{0x0F12, 0x0f00},   
{0x0F12, 0x0000},   
{0x0F12, 0x000f},   
{0x0F12, 0x000f},   
{0x0F12, 0x000f},   
{0x0F12, 0x0f00},   
{0x0F12, 0x0f00},   
{0x0F12, 0x0f00},   
{0x0F12, 0x000f},   
{0x0F12, 0x0000},   
{0x0F12, 0x000f},   
{0x0F12, 0x0f00},   
{0x0F12, 0x000f},   
{0x0F12, 0x0f00},   
{0x0F12, 0x000f},   
{0x0F12, 0x0f00},   
{0x0F12, 0x0f00},   
{0x0F12, 0x000f},   
{0x0F12, 0x0f00},   
{0x0F12, 0x0000},   
{0x0F12, 0x000f},   
{0x0F12, 0x000f},   
{0x0F12, 0x000f},   
{0x0F12, 0x0f00},   
{0x0F12, 0x0f0f},   
{0x0F12, 0x0f00},   
{0x0F12, 0x000f},   
{0x0F12, 0x0000},   
{0x0F12, 0x000f},   
{0x0F12, 0x0f00},   
{0x0F12, 0x000f},   
{0x0F12, 0x000f},   
{0x0F12, 0x000f},   
{0x0F12, 0x0f00},   
{0x0F12, 0x0f00},   
{0x0F12, 0x000f},   
{0x002A, 0x0D76},   
{0x0F12, 0x9cdd},   
{0x0F12, 0xe1b8},   
{0x0F12, 0x08e7},   
{0x0F12, 0x0fb3},   
{0x0F12, 0xea38},   
{0x0F12, 0x0e9e},   
{0x0F12, 0xdda1},   
{0x0F12, 0xfd5b},   
{0x0F12, 0xf5c6},   
{0x0F12, 0x09b4},   
{0x0F12, 0x11c3},   
{0x0F12, 0xe431},   
{0x0F12, 0xf693},   
{0x0F12, 0xf60c},   
{0x0F12, 0x27c2},   
{0x0F12, 0xdcb0},   
{0x0F12, 0xe770},   
{0x0F12, 0x34b8},   
{0x0F12, 0x391a},   
{0x0F12, 0x0913},   
{0x0F12, 0xd5c2},   
{0x0F12, 0x22d3},   
{0x0F12, 0x1af3},   
{0x0F12, 0xcf1e},   
{0x0F12, 0xc4b5},   
{0x0F12, 0x1128},   
{0x0F12, 0xfe6c},   
{0x0F12, 0x0086},   
{0x0F12, 0xfb0b},   
{0x0F12, 0xf4c7},   
{0x0F12, 0x1dc9},   
{0x0F12, 0xe5a4},   
{0x0F12, 0x1dd9},   
{0x0F12, 0xeafb},   
{0x0F12, 0xed7f},   
{0x0F12, 0x330b},   
{0x0F12, 0xb1da},   
{0x0F12, 0xda70},   
{0x0F12, 0x0d86},   
{0x0F12, 0x0b57},   
{0x0F12, 0xf54f},   
{0x0F12, 0x0486},   
{0x0F12, 0xd7f3},   
{0x0F12, 0xf8ea},   
{0x0F12, 0xf731},   
{0x0F12, 0x0e8c},   
{0x0F12, 0x056f},   
{0x0F12, 0xf11a},   
{0x0F12, 0xf781},   
{0x0F12, 0xf87a},   
{0x0F12, 0x256b},   
{0x0F12, 0xdd39},   
{0x0F12, 0xec49},   
{0x0F12, 0x2ade},   
{0x0F12, 0x3287},   
{0x0F12, 0x0d2c},   
{0x0F12, 0xd779},   
{0x0F12, 0x1a85},   
{0x0F12, 0x1f9f},   
{0x0F12, 0xd54e},   
{0x0F12, 0xe015},   
{0x0F12, 0x055b},   
{0x0F12, 0xfd06},   
{0x0F12, 0x0d58},   
{0x0F12, 0xf992},   
{0x0F12, 0xe6c8},   
{0x0F12, 0x0261},   
{0x0F12, 0xf1a9},   
{0x0F12, 0x1d9f},   
{0x0F12, 0xe32c},   
{0x0F12, 0xe8ab},   
{0x0F12, 0x425a},   
{0x0F12, 0x917b},   
{0x0F12, 0xdf7a},   
{0x0F12, 0x0f4f},   
{0x0F12, 0x067d},   
{0x0F12, 0xf36f},   
{0x0F12, 0x0a16},   
{0x0F12, 0xdec4},   
{0x0F12, 0x0285},   
{0x0F12, 0xee55},   
{0x0F12, 0x1297},   
{0x0F12, 0x0c86},   
{0x0F12, 0xe269},   
{0x0F12, 0x011e},   
{0x0F12, 0xf7ec},   
{0x0F12, 0x256e},   
{0x0F12, 0xdafa},   
{0x0F12, 0xe6db},   
{0x0F12, 0x3d01},   
{0x0F12, 0x2373},   
{0x0F12, 0x02a5},   
{0x0F12, 0xdb67},   
{0x0F12, 0x2791},   
{0x0F12, 0x14cc},   
{0x0F12, 0xc7dc},   
{0x0F12, 0xdd70},   
{0x0F12, 0x10f3},   
{0x0F12, 0x00f3},   
{0x0F12, 0xf5bd},   
{0x0F12, 0xff36},   
{0x0F12, 0x02f0},   
{0x0F12, 0x0f2e},   
{0x0F12, 0xebba},   
{0x0F12, 0x1614},   
{0x0F12, 0xf1ba},   
{0x0F12, 0xf573},   
{0x0F12, 0x1f72},   
{0x0F12, 0x9af9},   
{0x0F12, 0xde48},   
{0x0F12, 0x12a1},   
{0x0F12, 0x0046},   
{0x0F12, 0xfcb9},   
{0x0F12, 0x02ff},   
{0x0F12, 0xdc48},   
{0x0F12, 0x0214},   
{0x0F12, 0xefde},   
{0x0F12, 0x1406},   
{0x0F12, 0x04c1},   
{0x0F12, 0xeab8},   
{0x0F12, 0xfa74},   
{0x0F12, 0xf092},   
{0x0F12, 0x2843},   
{0x0F12, 0xd7b7},   
{0x0F12, 0xef72},   
{0x0F12, 0x35a0},   
{0x0F12, 0x3366},   
{0x0F12, 0x0f4c},   
{0x0F12, 0xd958},   
{0x0F12, 0x2351},   
{0x0F12, 0x186c},   
{0x0F12, 0xc47a},   
{0x0F12, 0xce8a},   
{0x0F12, 0x0a48},   
{0x0F12, 0xf859},   
{0x0F12, 0x06d8},   
{0x0F12, 0xf14c},   
{0x0F12, 0x0b15},   
{0x0F12, 0x1508},   
{0x0F12, 0xea85},   
{0x0F12, 0x213c},   
{0x0F12, 0xe468},   
{0x0F12, 0xfa5f},   
{0x0F12, 0x2039},   

{0x002A, 0x0906}, //// SET GAS ALPHA 
{0x0F12, 0x0001}, // #ash_nGasAlphaStep
{0x0F12, 0x0001}, // #ash_bUseGasAlpha 
{0x0F12, 0x1400}, // #TVAR_ash_GASAlpha_0__0_
{0x0F12, 0x1000}, // #TVAR_ash_GASAlpha_0__1_
{0x0F12, 0x1000}, // #TVAR_ash_GASAlpha_0__2_
{0x0F12, 0x0D80}, // #TVAR_ash_GASAlpha_0__3_ 
{0x0F12, 0x1380}, // #TVAR_ash_GASAlpha_1__0_
{0x0F12, 0x1000}, // #TVAR_ash_GASAlpha_1__1_
{0x0F12, 0x1000}, // #TVAR_ash_GASAlpha_1__2_
{0x0F12, 0x0EA0}, // #TVAR_ash_GASAlpha_1__3_ 
{0x0F12, 0x1300}, // #TVAR_ash_GASAlpha_2__0_
{0x0F12, 0x1000}, // #TVAR_ash_GASAlpha_2__1_
{0x0F12, 0x1000}, // #TVAR_ash_GASAlpha_2__2_
{0x0F12, 0x0F00}, // #TVAR_ash_GASAlpha_2__3_ 
{0x0F12, 0x1000}, // #TVAR_ash_GASAlpha_3__0_
{0x0F12, 0x1000}, // #TVAR_ash_GASAlpha_3__1_
{0x0F12, 0x1000}, // #TVAR_ash_GASAlpha_3__2_
{0x0F12, 0x0F00}, // #TVAR_ash_GASAlpha_3__3_ 
{0x0F12, 0x0F00}, // #TVAR_ash_GASAlpha_4__0_ 
{0x0F12, 0x1000}, // #TVAR_ash_GASAlpha_4__1_
{0x0F12, 0x1000}, // #TVAR_ash_GASAlpha_4__2_
{0x0F12, 0x0F00}, // #TVAR_ash_GASAlpha_4__3_ 
{0x0F12, 0x0F00}, // #TVAR_ash_GASAlpha_5__0_//DNP 
{0x0F12, 0x1000}, // #TVAR_ash_GASAlpha_5__1_
{0x0F12, 0x1000}, // #TVAR_ash_GASAlpha_5__2_
{0x0F12, 0x1000}, // #TVAR_ash_GASAlpha_5__3_
{0x0F12, 0x0F00}, // #TVAR_ash_GASAlpha_6__0_//D65 
{0x0F12, 0x1000}, // #TVAR_ash_GASAlpha_6__1_
{0x0F12, 0x1000}, // #TVAR_ash_GASAlpha_6__2_
{0x0F12, 0x1000}, // #TVAR_ash_GASAlpha_6__3_
{0x0F12, 0x1000}, // #TVAR_ash_GASOutdoorAlpha  
{0x0F12, 0x1000}, // #TVAR_ash_GASOutdoorAlpha_0_  
{0x0F12, 0x1000}, // #TVAR_ash_GASOutdoorAlpha_1_  
{0x0F12, 0x1000}, // #TVAR_ash_GASOutdoorAlpha_2_  
{0x0F12, 0x1000}, // #TVAR_ash_GASOutdoorAlpha_3_  
{0x002A, 0x08DC},   
{0x0F12, 0x00C0}, // #TVAR_ash_AwbAshCord_0_  
{0x0F12, 0x00DA}, // #TVAR_ash_AwbAshCord_1_  
{0x0F12, 0x00FA}, // #TVAR_ash_AwbAshCord_2_  
{0x0F12, 0x0105}, // #TVAR_ash_AwbAshCord_3_ 
{0x0F12, 0x0120}, // #TVAR_ash_AwbAshCord_4_ 
{0x0F12, 0x0140}, // #TVAR_ash_AwbAshCord_5_ 
{0x0F12, 0x0180}, // #TVAR_ash_AwbAshCord_6_ 
{0x002A, 0x116C}, 
{0x0F12, 0x012D}, // #awbb_IntcR  
{0x0F12, 0x012E}, // #awbb_IntcB  
{0x002A, 0x1186}, 
{0x0F12, 0x0420}, // #awbb_MvEq_RBthresh  
{0x002A, 0x12DE},   
{0x0F12, 0x2710}, // #awbb_GainsMaxMove
{0x002A, 0x139E}, 
{0x0F12, 0x0034}, // 0034 : Bypass Gain convergence 0134 : No Check AE Stable  // awbb_Use_Filters
{0x002A, 0x13A8}, // AWB Convergence Speed
{0x0F12, 0x0004}, // #awbb_WpFilterMinThr Stable  
{0x0F12, 0x0190}, // #awbb_WpFilterMaxThr boost
{0x0F12, 0x00A0}, // #awbb_WpFilterCoef  
{0x002A, 0x13A2}, 
{0x0F12, 0x0500}, // #awbb_GainsInit_0_
{0x0F12, 0x0400}, // #awbb_GainsInit_1_
{0x0F12, 0x0650}, // #awbb_GainsInit_2_
{0x002A, 0x118C},   
{0x0F12, 0x0680}, // #awbb_GamutWidthThr1 
{0x0F12, 0x0338}, // #awbb_GamutHeightThr1
{0x0F12, 0x0023}, // #awbb_GamutWidthThr2 
{0x0F12, 0x0019}, // #awbb_GamutHeightThr2
{0x002A, 0x11EE},   
{0x0F12, 0xFEF7}, // #awbb_SCDetectionMap_SEC_StartR_B 
{0x0F12, 0x0021}, // #awbb_SCDetectionMap_SEC_StepR_B 
{0x0F12, 0x1388}, // #awbb_SCDetectionMap_SEC_SunnyNB 
{0x0F12, 0x0122}, // #awbb_SCDetectionMap_SEC_StepNB  
{0x0F12, 0x018F}, // #awbb_SCDetectionMap_SEC_LowTempR_B  
{0x0F12, 0x0096}, // #awbb_SCDetectionMap_SEC_SunnyNBZone
{0x0F12, 0x000E}, // #awbb_SCDetectionMap_SEC_LowTempR_BZone 
{0x002A, 0x11A0}, 
{0x0F12, 0x0032}, // #awbb_LowBr 
{0x0F12, 0x001E}, // #awbb_LowBr_NBzone 
{0x002A, 0x0FD4}, //// SET AWB INDOOR BOUNDARY 
{0x0F12, 0x0376}, // #awbb_IndoorGrZones_m_BGrid_0__m_left  
{0x0F12, 0x0391}, // #awbb_IndoorGrZones_m_BGrid_0__m_right 
{0x0F12, 0x032E}, // #awbb_IndoorGrZones_m_BGrid_1__m_left
{0x0F12, 0x0386}, // #awbb_IndoorGrZones_m_BGrid_1__m_right 
{0x0F12, 0x030A}, // #awbb_IndoorGrZones_m_BGrid_2__m_left
{0x0F12, 0x036F}, // #awbb_IndoorGrZones_m_BGrid_2__m_right  
{0x0F12, 0x02DB}, // #awbb_IndoorGrZones_m_BGrid_3__m_left
{0x0F12, 0x0349}, // #awbb_IndoorGrZones_m_BGrid_3__m_right 
{0x0F12, 0x02B6}, // #awbb_IndoorGrZones_m_BGrid_4__m_left
{0x0F12, 0x0325}, // #awbb_IndoorGrZones_m_BGrid_4__m_right 
{0x0F12, 0x028F}, // #awbb_IndoorGrZones_m_BGrid_5__m_left
{0x0F12, 0x0308}, // #awbb_IndoorGrZones_m_BGrid_5__m_right 
{0x0F12, 0x026C}, // #awbb_IndoorGrZones_m_BGrid_6__m_left
{0x0F12, 0x02E8}, // #awbb_IndoorGrZones_m_BGrid_6__m_right  
{0x0F12, 0x0246}, // #awbb_IndoorGrZones_m_BGrid_7__m_left  
{0x0F12, 0x02C0}, // #awbb_IndoorGrZones_m_BGrid_7__m_right  
{0x0F12, 0x0226}, // #awbb_IndoorGrZones_m_BGrid_8__m_left  
{0x0F12, 0x02A6}, // #awbb_IndoorGrZones_m_BGrid_8__m_right  
{0x0F12, 0x0205}, // #awbb_IndoorGrZones_m_BGrid_9__m_left  
{0x0F12, 0x028C}, // #awbb_IndoorGrZones_m_BGrid_9__m_right  
{0x0F12, 0x01F6}, // #awbb_IndoorGrZones_m_BGrid_10__m_left  
{0x0F12, 0x0273}, // #awbb_IndoorGrZones_m_BGrid_10__m_right
{0x0F12, 0x01EB}, // #awbb_IndoorGrZones_m_BGrid_11__m_left  
{0x0F12, 0x0261}, // #awbb_IndoorGrZones_m_BGrid_11__m_right
{0x0F12, 0x01ED}, // #awbb_IndoorGrZones_m_BGrid_12__m_left  
{0x0F12, 0x0247}, // #awbb_IndoorGrZones_m_BGrid_12__m_right
{0x0F12, 0x0203}, // #awbb_IndoorGrZones_m_BGrid_13__m_left 
{0x0F12, 0x021F}, // #awbb_IndoorGrZones_m_BGrid_13__m_right 
{0x0F12, 0x0000}, // #awbb_IndoorGrZones_m_BGrid_14__m_left 
{0x0F12, 0x0000}, // #awbb_IndoorGrZones_m_BGrid_14__m_right
{0x0F12, 0x0000}, // #awbb_IndoorGrZones_m_BGrid_15__m_left 
{0x0F12, 0x0000}, // #awbb_IndoorGrZones_m_BGrid_15__m_right
{0x0F12, 0x0000}, // #awbb_IndoorGrZones_m_BGrid_16__m_left 
{0x0F12, 0x0000}, // #awbb_IndoorGrZones_m_BGrid_16__m_right
{0x0F12, 0x0000}, // #awbb_IndoorGrZones_m_BGrid_17__m_left 
{0x0F12, 0x0000}, // #awbb_IndoorGrZones_m_BGrid_17__m_right
{0x0F12, 0x0000}, // #awbb_IndoorGrZones_m_BGrid_18__m_left 
{0x0F12, 0x0000}, // #awbb_IndoorGrZones_m_BGrid_18__m_right
{0x0F12, 0x0000}, // #awbb_IndoorGrZones_m_BGrid_19__m_left 
{0x0F12, 0x0000}, // #awbb_IndoorGrZones_m_BGrid_19__m_right
{0x0F12, 0x0005}, // #awbb_IndoorGrZones_m_GridStep
{0x002A, 0x1028}, 
{0x0F12, 0x000E}, // #awbb_IndoorGrZones_ZInfo_m_GridSz
{0x002A, 0x102C}, 
{0x0F12, 0x013B}, // #awbb_IndoorGrZones_m_Boffs 
{0x002A, 0x1030}, //// SET AWB OUTDOOR BOUNDARY 
{0x0F12, 0x0277}, // #awbb_OutdoorGrZones_m_BGrid_0__m_left 
{0x0F12, 0x0294}, // #awbb_OutdoorGrZones_m_BGrid_0__m_right
{0x0F12, 0x0247}, // #awbb_OutdoorGrZones_m_BGrid_1__m_left 
{0x0F12, 0x029B}, // #awbb_OutdoorGrZones_m_BGrid_1__m_right 
{0x0F12, 0x0236}, // #awbb_OutdoorGrZones_m_BGrid_2__m_left 
{0x0F12, 0x0294}, // #awbb_OutdoorGrZones_m_BGrid_2__m_right
{0x0F12, 0x0222}, // #awbb_OutdoorGrZones_m_BGrid_3__m_left 
{0x0F12, 0x0286}, // #awbb_OutdoorGrZones_m_BGrid_3__m_right
{0x0F12, 0x020E}, // #awbb_OutdoorGrZones_m_BGrid_4__m_left  
{0x0F12, 0x0276}, // #awbb_OutdoorGrZones_m_BGrid_4__m_right
{0x0F12, 0x020C}, // #awbb_OutdoorGrZones_m_BGrid_5__m_left  
{0x0F12, 0x0264}, // #awbb_OutdoorGrZones_m_BGrid_5__m_right
{0x0F12, 0x0210}, // #awbb_OutdoorGrZones_m_BGrid_6__m_left 
{0x0F12, 0x0259}, // #awbb_OutdoorGrZones_m_BGrid_6__m_right
{0x0F12, 0x0210}, // #awbb_OutdoorGrZones_m_BGrid_7__m_left 
{0x0F12, 0x024E}, // #awbb_OutdoorGrZones_m_BGrid_7__m_right 
{0x0F12, 0x021F}, // #awbb_OutdoorGrZones_m_BGrid_8__m_left  
{0x0F12, 0x0235}, // #awbb_OutdoorGrZones_m_BGrid_8__m_right
{0x0F12, 0x0000}, // #awbb_OutdoorGrZones_m_BGrid_9__m_left 
{0x0F12, 0x0000}, // #awbb_OutdoorGrZones_m_BGrid_9__m_right
{0x0F12, 0x0000}, // #awbb_OutdoorGrZones_m_BGrid_10__m_left
{0x0F12, 0x0000}, // #awbb_OutdoorGrZones_m_BGrid_10__m_right  
{0x0F12, 0x0000}, // #awbb_OutdoorGrZones_m_BGrid_11__m_left
{0x0F12, 0x0000}, // #awbb_OutdoorGrZones_m_BGrid_11__m_right  
{0x0F12, 0x0004}, // #awbb_OutdoorGrZones_m_GridStep  
{0x002A, 0x1064}, 
{0x0F12, 0x0006}, // #awbb_OutdoorGrZones_ZInfo_m_GridSz 
{0x002A, 0x1068},   
{0x0F12, 0x0221}, // #awbb_OutdoorGrZones_m_Boffs
{0x002A, 0x106C}, //// SET AWB LOWBR BOUNDARY 
{0x0F12, 0x038E}, // #awbb_LowBrGrZones_m_BGrid_0__m_left 
{0x0F12, 0x03DE}, // #awbb_LowBrGrZones_m_BGrid_0__m_right
{0x0F12, 0x0310}, // #awbb_LowBrGrZones_m_BGrid_1__m_left
{0x0F12, 0x03D0}, // #awbb_LowBrGrZones_m_BGrid_1__m_right
{0x0F12, 0x02B7}, // #awbb_LowBrGrZones_m_BGrid_2__m_left 
{0x0F12, 0x0393}, // #awbb_LowBrGrZones_m_BGrid_2__m_right  
{0x0F12, 0x0265}, // #awbb_LowBrGrZones_m_BGrid_3__m_left
{0x0F12, 0x035F}, // #awbb_LowBrGrZones_m_BGrid_3__m_right
{0x0F12, 0x0226}, // #awbb_LowBrGrZones_m_BGrid_4__m_left
{0x0F12, 0x031B}, // #awbb_LowBrGrZones_m_BGrid_4__m_right
{0x0F12, 0x01EB}, // #awbb_LowBrGrZones_m_BGrid_5__m_left 
{0x0F12, 0x02D2}, // #awbb_LowBrGrZones_m_BGrid_5__m_right
{0x0F12, 0x01C7}, // #awbb_LowBrGrZones_m_BGrid_6__m_left 
{0x0F12, 0x02A7}, // #awbb_LowBrGrZones_m_BGrid_6__m_right
{0x0F12, 0x01CC}, // #awbb_LowBrGrZones_m_BGrid_7__m_left 
{0x0F12, 0x0273}, // #awbb_LowBrGrZones_m_BGrid_7__m_right  
{0x0F12, 0x01ED}, // #awbb_LowBrGrZones_m_BGrid_8__m_left 
{0x0F12, 0x023D}, // #awbb_LowBrGrZones_m_BGrid_8__m_right
{0x0F12, 0x0000}, // #awbb_LowBrGrZones_m_BGrid_9__m_left
{0x0F12, 0x0000}, // #awbb_LowBrGrZones_m_BGrid_9__m_right  
{0x0F12, 0x0000}, // #awbb_LowBrGrZones_m_BGrid_10__m_left  
{0x0F12, 0x0000}, // #awbb_LowBrGrZones_m_BGrid_10__m_right 
{0x0F12, 0x0000}, // #awbb_LowBrGrZones_m_BGrid_11__m_left  
{0x0F12, 0x0000}, // #awbb_LowBrGrZones_m_BGrid_11__m_right 
{0x0F12, 0x0006}, // #awbb_LowBrGrZones_m_GridStep 
{0x002A, 0x10A0}, 
{0x0F12, 0x000C}, // #awbb_LowBrGrZones_ZInfo_m_GridSz 
{0x002A, 0x10A4},   
{0x0F12, 0x0116}, // #awbb_LowBrGrZones_m_Boffs 
{0x002A, 0x13B0},   
{0x0F12, 0x0000}, // #awbb_GridEnable  
{0x002A, 0x083C},   
{0x0F12, 0x0000}, // #seti_bUseOutdoorGamma  
{0x002A, 0x074C}, //// SET GAMMA (INDOOR, OUTDOOR)
{0x0F12, 0x0000}, // #SARR_usGammaLutRGBIndoor_0__0_  
{0x0F12, 0x0002}, // #SARR_usGammaLutRGBIndoor_0__1_  
{0x0F12, 0x0008}, // #SARR_usGammaLutRGBIndoor_0__2_  
{0x0F12, 0x0020}, // #SARR_usGammaLutRGBIndoor_0__3_  
{0x0F12, 0x0059}, // #SARR_usGammaLutRGBIndoor_0__4_  
{0x0F12, 0x00CF}, // #SARR_usGammaLutRGBIndoor_0__5_
{0x0F12, 0x0132}, // #SARR_usGammaLutRGBIndoor_0__6_  
{0x0F12, 0x015C}, // #SARR_usGammaLutRGBIndoor_0__7_
{0x0F12, 0x0180}, // #SARR_usGammaLutRGBIndoor_0__8_  
{0x0F12, 0x01BC}, // #SARR_usGammaLutRGBIndoor_0__9_
{0x0F12, 0x01E8}, // #SARR_usGammaLutRGBIndoor_0__10_  
{0x0F12, 0x020F}, // #SARR_usGammaLutRGBIndoor_0__11_  
{0x0F12, 0x0232}, // #SARR_usGammaLutRGBIndoor_0__12_ 
{0x0F12, 0x0273}, // #SARR_usGammaLutRGBIndoor_0__13_ 
{0x0F12, 0x02AF}, // #SARR_usGammaLutRGBIndoor_0__14_  
{0x0F12, 0x0309}, // #SARR_usGammaLutRGBIndoor_0__15_ 
{0x0F12, 0x0355}, // #SARR_usGammaLutRGBIndoor_0__16_ 
{0x0F12, 0x0394}, // #SARR_usGammaLutRGBIndoor_0__17_ 
{0x0F12, 0x03CE}, // #SARR_usGammaLutRGBIndoor_0__18_  
{0x0F12, 0x03FF}, // #SARR_usGammaLutRGBIndoor_0__19_  
{0x0F12, 0x0000}, // #SARR_usGammaLutRGBIndoor_1__0_  
{0x0F12, 0x0002}, // #SARR_usGammaLutRGBIndoor_1__1_  
{0x0F12, 0x0008}, // #SARR_usGammaLutRGBIndoor_1__2_  
{0x0F12, 0x0020}, // #SARR_usGammaLutRGBIndoor_1__3_  
{0x0F12, 0x0059}, // #SARR_usGammaLutRGBIndoor_1__4_  
{0x0F12, 0x00CF}, // #SARR_usGammaLutRGBIndoor_1__5_
{0x0F12, 0x0132}, // #SARR_usGammaLutRGBIndoor_1__6_  
{0x0F12, 0x015C}, // #SARR_usGammaLutRGBIndoor_1__7_
{0x0F12, 0x0180}, // #SARR_usGammaLutRGBIndoor_1__8_  
{0x0F12, 0x01BC}, // #SARR_usGammaLutRGBIndoor_1__9_
{0x0F12, 0x01E8}, // #SARR_usGammaLutRGBIndoor_1__10_  
{0x0F12, 0x020F}, // #SARR_usGammaLutRGBIndoor_1__11_  
{0x0F12, 0x0232}, // #SARR_usGammaLutRGBIndoor_1__12_ 
{0x0F12, 0x0273}, // #SARR_usGammaLutRGBIndoor_1__13_ 
{0x0F12, 0x02AF}, // #SARR_usGammaLutRGBIndoor_1__14_  
{0x0F12, 0x0309}, // #SARR_usGammaLutRGBIndoor_1__15_ 
{0x0F12, 0x0355}, // #SARR_usGammaLutRGBIndoor_1__16_ 
{0x0F12, 0x0394}, // #SARR_usGammaLutRGBIndoor_1__17_ 
{0x0F12, 0x03CE}, // #SARR_usGammaLutRGBIndoor_1__18_  
{0x0F12, 0x03FF}, // #SARR_usGammaLutRGBIndoor_1__19_  
{0x0F12, 0x0000}, // #SARR_usGammaLutRGBIndoor_2__0_  
{0x0F12, 0x0002}, // #SARR_usGammaLutRGBIndoor_2__1_  
{0x0F12, 0x0008}, // #SARR_usGammaLutRGBIndoor_2__2_  
{0x0F12, 0x0020}, // #SARR_usGammaLutRGBIndoor_2__3_  
{0x0F12, 0x0059}, // #SARR_usGammaLutRGBIndoor_2__4_  
{0x0F12, 0x00CF}, // #SARR_usGammaLutRGBIndoor_2__5_
{0x0F12, 0x0132}, // #SARR_usGammaLutRGBIndoor_2__6_  
{0x0F12, 0x015C}, // #SARR_usGammaLutRGBIndoor_2__7_
{0x0F12, 0x0180}, // #SARR_usGammaLutRGBIndoor_2__8_  
{0x0F12, 0x01BC}, // #SARR_usGammaLutRGBIndoor_2__9_
{0x0F12, 0x01E8}, // #SARR_usGammaLutRGBIndoor_2__10_  
{0x0F12, 0x020F}, // #SARR_usGammaLutRGBIndoor_2__11_  
{0x0F12, 0x0232}, // #SARR_usGammaLutRGBIndoor_2__12_ 
{0x0F12, 0x0273}, // #SARR_usGammaLutRGBIndoor_2__13_ 
{0x0F12, 0x02AF}, // #SARR_usGammaLutRGBIndoor_2__14_  
{0x0F12, 0x0309}, // #SARR_usGammaLutRGBIndoor_2__15_ 
{0x0F12, 0x0355}, // #SARR_usGammaLutRGBIndoor_2__16_ 
{0x0F12, 0x0394}, // #SARR_usGammaLutRGBIndoor_2__17_ 
{0x0F12, 0x03CE}, // #SARR_usGammaLutRGBIndoor_2__18_  
{0x0F12, 0x03FF}, // #SARR_usGammaLutRGBIndoor_2__19_  
{0x0F12, 0x0000}, // #SARR_usGammaLutRGBOutdoor_0__0_ 
{0x0F12, 0x0002}, // #SARR_usGammaLutRGBOutdoor_0__1_ 
{0x0F12, 0x0008}, // #SARR_usGammaLutRGBOutdoor_0__2_ 
{0x0F12, 0x0020}, // #SARR_usGammaLutRGBOutdoor_0__3_ 
{0x0F12, 0x0059}, // #SARR_usGammaLutRGBOutdoor_0__4_ 
{0x0F12, 0x00CF}, // #SARR_usGammaLutRGBOutdoor_0__5_  
{0x0F12, 0x0132}, // #SARR_usGammaLutRGBOutdoor_0__6_ 
{0x0F12, 0x015C}, // #SARR_usGammaLutRGBOutdoor_0__7_  
{0x0F12, 0x0180}, // #SARR_usGammaLutRGBOutdoor_0__8_ 
{0x0F12, 0x01BC}, // #SARR_usGammaLutRGBOutdoor_0__9_  
{0x0F12, 0x01E8}, // #SARR_usGammaLutRGBOutdoor_0__10_ 
{0x0F12, 0x020F}, // #SARR_usGammaLutRGBOutdoor_0__11_ 
{0x0F12, 0x0232}, // #SARR_usGammaLutRGBOutdoor_0__12_
{0x0F12, 0x0273}, // #SARR_usGammaLutRGBOutdoor_0__13_
{0x0F12, 0x02AF}, // #SARR_usGammaLutRGBOutdoor_0__14_ 
{0x0F12, 0x0309}, // #SARR_usGammaLutRGBOutdoor_0__15_
{0x0F12, 0x0355}, // #SARR_usGammaLutRGBOutdoor_0__16_
{0x0F12, 0x0394}, // #SARR_usGammaLutRGBOutdoor_0__17_
{0x0F12, 0x03CE}, // #SARR_usGammaLutRGBOutdoor_0__18_ 
{0x0F12, 0x03FF}, // #SARR_usGammaLutRGBOutdoor_0__19_ 
{0x0F12, 0x0000}, // #SARR_usGammaLutRGBOutdoor_1__0_ 
{0x0F12, 0x0002}, // #SARR_usGammaLutRGBOutdoor_1__1_ 
{0x0F12, 0x0008}, // #SARR_usGammaLutRGBOutdoor_1__2_ 
{0x0F12, 0x0020}, // #SARR_usGammaLutRGBOutdoor_1__3_ 
{0x0F12, 0x0059}, // #SARR_usGammaLutRGBOutdoor_1__4_ 
{0x0F12, 0x00CF}, // #SARR_usGammaLutRGBOutdoor_1__5_  
{0x0F12, 0x0132}, // #SARR_usGammaLutRGBOutdoor_1__6_ 
{0x0F12, 0x015C}, // #SARR_usGammaLutRGBOutdoor_1__7_  
{0x0F12, 0x0180}, // #SARR_usGammaLutRGBOutdoor_1__8_ 
{0x0F12, 0x01BC}, // #SARR_usGammaLutRGBOutdoor_1__9_  
{0x0F12, 0x01E8}, // #SARR_usGammaLutRGBOutdoor_1__10_ 
{0x0F12, 0x020F}, // #SARR_usGammaLutRGBOutdoor_1__11_ 
{0x0F12, 0x0232}, // #SARR_usGammaLutRGBOutdoor_1__12_
{0x0F12, 0x0273}, // #SARR_usGammaLutRGBOutdoor_1__13_
{0x0F12, 0x02AF}, // #SARR_usGammaLutRGBOutdoor_1__14_ 
{0x0F12, 0x0309}, // #SARR_usGammaLutRGBOutdoor_1__15_
{0x0F12, 0x0355}, // #SARR_usGammaLutRGBOutdoor_1__16_
{0x0F12, 0x0394}, // #SARR_usGammaLutRGBOutdoor_1__17_
{0x0F12, 0x03CE}, // #SARR_usGammaLutRGBOutdoor_1__18_ 
{0x0F12, 0x03FF}, // #SARR_usGammaLutRGBOutdoor_1__19_ 
{0x0F12, 0x0000}, // #SARR_usGammaLutRGBOutdoor_2__0_ 
{0x0F12, 0x0002}, // #SARR_usGammaLutRGBOutdoor_2__1_ 
{0x0F12, 0x0008}, // #SARR_usGammaLutRGBOutdoor_2__2_ 
{0x0F12, 0x0020}, // #SARR_usGammaLutRGBOutdoor_2__3_ 
{0x0F12, 0x0059}, // #SARR_usGammaLutRGBOutdoor_2__4_ 
{0x0F12, 0x00CF}, // #SARR_usGammaLutRGBOutdoor_2__5_  
{0x0F12, 0x0132}, // #SARR_usGammaLutRGBOutdoor_2__6_ 
{0x0F12, 0x015C}, // #SARR_usGammaLutRGBOutdoor_2__7_  
{0x0F12, 0x0180}, // #SARR_usGammaLutRGBOutdoor_2__8_ 
{0x0F12, 0x01BC}, // #SARR_usGammaLutRGBOutdoor_2__9_  
{0x0F12, 0x01E8}, // #SARR_usGammaLutRGBOutdoor_2__10_ 
{0x0F12, 0x020F}, // #SARR_usGammaLutRGBOutdoor_2__11_ 
{0x0F12, 0x0232}, // #SARR_usGammaLutRGBOutdoor_2__12_
{0x0F12, 0x0273}, // #SARR_usGammaLutRGBOutdoor_2__13_
{0x0F12, 0x02AF}, // #SARR_usGammaLutRGBOutdoor_2__14_ 
{0x0F12, 0x0309}, // #SARR_usGammaLutRGBOutdoor_2__15_
{0x0F12, 0x0355}, // #SARR_usGammaLutRGBOutdoor_2__16_
{0x0F12, 0x0394}, // #SARR_usGammaLutRGBOutdoor_2__17_
{0x0F12, 0x03CE}, // #SARR_usGammaLutRGBOutdoor_2__18_ 
{0x0F12, 0x03FF}, // #SARR_usGammaLutRGBOutdoor_2__19_
 

};


unsigned short s5k4eagx_init_reg45[][2] = {
{0x0028, 0x7000},
{0x002A, 0x08B8}, //// SET CCM
{0x0F12, 0x3700}, // #TVAR_wbt_pBaseCcms
{0x0F12, 0x7000}, 
{0x002A, 0x3700},  
{0x0F12, 0x01CB}, // #TVAR_wbt_pBaseCcms[0]// R  
{0x0F12, 0xFF8E}, // #TVAR_wbt_pBaseCcms[1]
{0x0F12, 0xFFD2}, // #TVAR_wbt_pBaseCcms[2]
{0x0F12, 0xFF64}, // #TVAR_wbt_pBaseCcms[3]// G  
{0x0F12, 0x01B2}, // #TVAR_wbt_pBaseCcms[4]
{0x0F12, 0xFF35}, // #TVAR_wbt_pBaseCcms[5]
{0x0F12, 0xFFDF}, // #TVAR_wbt_pBaseCcms[6]// B
{0x0F12, 0xFFE9}, // #TVAR_wbt_pBaseCcms[7]
{0x0F12, 0x01BD}, // #TVAR_wbt_pBaseCcms[8]
{0x0F12, 0x011C}, // #TVAR_wbt_pBaseCcms[9]// Y  
{0x0F12, 0x011B}, // #TVAR_wbt_pBaseCcms[10]  
{0x0F12, 0xFF43}, // #TVAR_wbt_pBaseCcms[11]  
{0x0F12, 0x019D}, // #TVAR_wbt_pBaseCcms[12]  // M  
{0x0F12, 0xFF4C}, // #TVAR_wbt_pBaseCcms[13]  
{0x0F12, 0x01CC}, // #TVAR_wbt_pBaseCcms[14]  
{0x0F12, 0xFF33}, // #TVAR_wbt_pBaseCcms[15]  // C  
{0x0F12, 0x0173}, // #TVAR_wbt_pBaseCcms[16] 
{0x0F12, 0x012F}, // #TVAR_wbt_pBaseCcms[17]  
{0x0F12, 0x01CB}, // #TVAR_wbt_pBaseCcms[18]  // R  
{0x0F12, 0xFF8E}, // #TVAR_wbt_pBaseCcms[19]  
{0x0F12, 0xFFD2}, // #TVAR_wbt_pBaseCcms[20]  
{0x0F12, 0xFF64}, // #TVAR_wbt_pBaseCcms[21]  // G  
{0x0F12, 0x01B2}, // #TVAR_wbt_pBaseCcms[22]  
{0x0F12, 0xFF35}, // #TVAR_wbt_pBaseCcms[23]  
{0x0F12, 0xFFDF}, // #TVAR_wbt_pBaseCcms[24]  // B
{0x0F12, 0xFFE9}, // #TVAR_wbt_pBaseCcms[25]  
{0x0F12, 0x01BD}, // #TVAR_wbt_pBaseCcms[26]  
{0x0F12, 0x011C}, // #TVAR_wbt_pBaseCcms[27]  // Y  
{0x0F12, 0x011B}, // #TVAR_wbt_pBaseCcms[28]  
{0x0F12, 0xFF43}, // #TVAR_wbt_pBaseCcms[29]  
{0x0F12, 0x019D}, // #TVAR_wbt_pBaseCcms[30]  // M  
{0x0F12, 0xFF4C}, // #TVAR_wbt_pBaseCcms[31]  
{0x0F12, 0x01CC}, // #TVAR_wbt_pBaseCcms[32]  
{0x0F12, 0xFF33}, // #TVAR_wbt_pBaseCcms[33]  // C  
{0x0F12, 0x0173}, // #TVAR_wbt_pBaseCcms[34] 
{0x0F12, 0x012F}, // #TVAR_wbt_pBaseCcms[35]  
{0x0F12, 0x01CB}, // #TVAR_wbt_pBaseCcms[36] // R 
{0x0F12, 0xFF8E}, // #TVAR_wbt_pBaseCcms[37]  
{0x0F12, 0xFFD2}, // #TVAR_wbt_pBaseCcms[38]  
{0x0F12, 0xFF64}, // #TVAR_wbt_pBaseCcms[39]  // G  
{0x0F12, 0x01B2}, // #TVAR_wbt_pBaseCcms[40]  
{0x0F12, 0xFF35}, // #TVAR_wbt_pBaseCcms[41]  
{0x0F12, 0xFFDF}, // #TVAR_wbt_pBaseCcms[42]  // B
{0x0F12, 0xFFE9}, // #TVAR_wbt_pBaseCcms[43]  
{0x0F12, 0x01BD}, // #TVAR_wbt_pBaseCcms[44]  
{0x0F12, 0x011C}, // #TVAR_wbt_pBaseCcms[45]  // Y  
{0x0F12, 0x011B}, // #TVAR_wbt_pBaseCcms[46]  
{0x0F12, 0xFF43}, // #TVAR_wbt_pBaseCcms[47]  
{0x0F12, 0x019D}, // #TVAR_wbt_pBaseCcms[48]  // M  
{0x0F12, 0xFF4C}, // #TVAR_wbt_pBaseCcms[49]  
{0x0F12, 0x01CC}, // #TVAR_wbt_pBaseCcms[50]  
{0x0F12, 0xFF33}, // #TVAR_wbt_pBaseCcms[51]  // C  
{0x0F12, 0x0173}, // #TVAR_wbt_pBaseCcms[52] 
{0x0F12, 0x012F}, // #TVAR_wbt_pBaseCcms[53]  
{0x0F12, 0x01CB}, // #TVAR_wbt_pBaseCcms[54] // R 
{0x0F12, 0xFF8E}, // #TVAR_wbt_pBaseCcms[55]  
{0x0F12, 0xFFD2}, // #TVAR_wbt_pBaseCcms[56]  
{0x0F12, 0xFF64}, // #TVAR_wbt_pBaseCcms[57]  // G  
{0x0F12, 0x01B2}, // #TVAR_wbt_pBaseCcms[58]  
{0x0F12, 0xFF35}, // #TVAR_wbt_pBaseCcms[59]  
{0x0F12, 0xFFDF}, // #TVAR_wbt_pBaseCcms[60]  // B
{0x0F12, 0xFFE9}, // #TVAR_wbt_pBaseCcms[61]  
{0x0F12, 0x01BD}, // #TVAR_wbt_pBaseCcms[62]  
{0x0F12, 0x011C}, // #TVAR_wbt_pBaseCcms[63]  // Y  
{0x0F12, 0x011B}, // #TVAR_wbt_pBaseCcms[64]  
{0x0F12, 0xFF43}, // #TVAR_wbt_pBaseCcms[65]  
{0x0F12, 0x019D}, // #TVAR_wbt_pBaseCcms[66]  // M  
{0x0F12, 0xFF4C}, // #TVAR_wbt_pBaseCcms[67]  
{0x0F12, 0x01CC}, // #TVAR_wbt_pBaseCcms[68]  
{0x0F12, 0xFF33}, // #TVAR_wbt_pBaseCcms[69]  // C  
{0x0F12, 0x0173}, // #TVAR_wbt_pBaseCcms[70] 
{0x0F12, 0x012F}, // #TVAR_wbt_pBaseCcms[71]  
{0x0F12, 0x01CB}, // #TVAR_wbt_pBaseCcms[72]  // R  
{0x0F12, 0xFF8E}, // #TVAR_wbt_pBaseCcms[73]  
{0x0F12, 0xFFD2}, // #TVAR_wbt_pBaseCcms[74]  
{0x0F12, 0xFF64}, // #TVAR_wbt_pBaseCcms[75]  // G  
{0x0F12, 0x01B2}, // #TVAR_wbt_pBaseCcms[76]  
{0x0F12, 0xFF35}, // #TVAR_wbt_pBaseCcms[77]  
{0x0F12, 0xFFDF}, // #TVAR_wbt_pBaseCcms[78]  // B
{0x0F12, 0xFFE9}, // #TVAR_wbt_pBaseCcms[79]  
{0x0F12, 0x01BD}, // #TVAR_wbt_pBaseCcms[80]  
{0x0F12, 0x011C}, // #TVAR_wbt_pBaseCcms[81]  // Y  
{0x0F12, 0x011B}, // #TVAR_wbt_pBaseCcms[82]  
{0x0F12, 0xFF43}, // #TVAR_wbt_pBaseCcms[83]  
{0x0F12, 0x019D}, // #TVAR_wbt_pBaseCcms[84]  // M  
{0x0F12, 0xFF4C}, // #TVAR_wbt_pBaseCcms[85]  
{0x0F12, 0x01CC}, // #TVAR_wbt_pBaseCcms[86]  
{0x0F12, 0xFF33}, // #TVAR_wbt_pBaseCcms[87]  // C  
{0x0F12, 0x0173}, // #TVAR_wbt_pBaseCcms[88] 
{0x0F12, 0x012F}, // #TVAR_wbt_pBaseCcms[89]  
{0x0F12, 0x01CB}, // #TVAR_wbt_pBaseCcms[90] // R 
{0x0F12, 0xFF8E}, // #TVAR_wbt_pBaseCcms[91]  
{0x0F12, 0xFFD2}, // #TVAR_wbt_pBaseCcms[92]  
{0x0F12, 0xFF64}, // #TVAR_wbt_pBaseCcms[93]  // G  
{0x0F12, 0x01B2}, // #TVAR_wbt_pBaseCcms[94]  
{0x0F12, 0xFF35}, // #TVAR_wbt_pBaseCcms[95]  
{0x0F12, 0xFFDF}, // #TVAR_wbt_pBaseCcms[96]  // B
{0x0F12, 0xFFE9}, // #TVAR_wbt_pBaseCcms[97]  
{0x0F12, 0x01BD}, // #TVAR_wbt_pBaseCcms[98]  
{0x0F12, 0x011C}, // #TVAR_wbt_pBaseCcms[99]  // Y  
{0x0F12, 0x011B}, // #TVAR_wbt_pBaseCcms[100] 
{0x0F12, 0xFF43}, // #TVAR_wbt_pBaseCcms[101] 
{0x0F12, 0x019D}, // #TVAR_wbt_pBaseCcms[102] // M  
{0x0F12, 0xFF4C}, // #TVAR_wbt_pBaseCcms[103] 
{0x0F12, 0x01CC}, // #TVAR_wbt_pBaseCcms[104] 
{0x0F12, 0xFF33}, // #TVAR_wbt_pBaseCcms[105] // C  
{0x0F12, 0x0173}, // #TVAR_wbt_pBaseCcms[106]
{0x0F12, 0x012F}, // #TVAR_wbt_pBaseCcms[107] 
{0x002A, 0x08C0},  
{0x0F12, 0x37D8}, // #TVAR_wbt_pOutdoorCcm 
{0x0F12, 0x7000},  
{0x002A, 0x37D8},  
{0x0F12, 0x01CB}, // #TVAR_wbt_pOutdoorCcm[0] // R 
{0x0F12, 0xFF8E}, // #TVAR_wbt_pOutdoorCcm[1] 
{0x0F12, 0xFFD2}, // #TVAR_wbt_pOutdoorCcm[2] 
{0x0F12, 0xFF64}, // #TVAR_wbt_pOutdoorCcm[3] // G  
{0x0F12, 0x01B2}, // #TVAR_wbt_pOutdoorCcm[4] 
{0x0F12, 0xFF35}, // #TVAR_wbt_pOutdoorCcm[5] 
{0x0F12, 0xFFDF}, // #TVAR_wbt_pOutdoorCcm[6] // B
{0x0F12, 0xFFE9}, // #TVAR_wbt_pOutdoorCcm[7] 
{0x0F12, 0x01BD}, // #TVAR_wbt_pOutdoorCcm[8] 
{0x0F12, 0x011C}, // #TVAR_wbt_pOutdoorCcm[9] // Y  
{0x0F12, 0x011B}, // #TVAR_wbt_pOutdoorCcm[10]
{0x0F12, 0xFF43}, // #TVAR_wbt_pOutdoorCcm[11]
{0x0F12, 0x019D}, // #TVAR_wbt_pOutdoorCcm[12]// M  
{0x0F12, 0xFF4C}, // #TVAR_wbt_pOutdoorCcm[13]
{0x0F12, 0x01CC}, // #TVAR_wbt_pOutdoorCcm[14]
{0x0F12, 0xFF33}, // #TVAR_wbt_pOutdoorCcm[15]// C  
{0x0F12, 0x0173}, // #TVAR_wbt_pOutdoorCcm[16]  
{0x0F12, 0x012F}, // #TVAR_wbt_pOutdoorCcm[17]
{0x002A, 0x08C6},  
{0x0F12, 0x00D0}, // #SARR_AwbCcmCord_0_
{0x0F12, 0x00E0}, // #SARR_AwbCcmCord_1_
{0x0F12, 0x0100}, // #SARR_AwbCcmCord_2_  
{0x0F12, 0x0110}, // #SARR_AwbCcmCord_3_  
{0x0F12, 0x0130}, // #SARR_AwbCcmCord_4_  
{0x0F12, 0x0180}, // #SARR_AwbCcmCord_5_  
{0x002A, 0x099A},  
{0x0F12, 0x0000}, // #afit_bAfitExOffMBR  
{0x002A, 0x0958},  
{0x0F12, 0x0041}, // #afit_uNoiseIndInDoor_0_
{0x0F12, 0x0063}, // #afit_uNoiseIndInDoor_1_ 
{0x0F12, 0x00C8}, // #afit_uNoiseIndInDoor_2_  
{0x0F12, 0x0151}, // #afit_uNoiseIndInDoor_3_
{0x0F12, 0x02A3}, // #afit_uNoiseIndInDoor_4_
{0x002A, 0x0964}, 
{0x0F12, 0x099C}, // #TVAR_afit_pBaseVals  

{0x002A, 0x099C}, //// SET AFIT 
{0x0F12, 0x0000}, //0000// #AFIT16_BRIGHTNESS //x5  
{0x0F12, 0x0000}, //0000// #AFIT16_CONTRAST  
{0x0F12, 0x0000}, //0000// #AFIT16_SATURATION
{0x0F12, 0x0005}, //0005// #AFIT16_SHARP_BLUR
{0x0F12, 0xFFFB}, //FFFB// #AFIT16_GLAMOUR
{0x0F12, 0x0080}, //0080// #AFIT16_DDD_edge_high
{0x0F12, 0x03FF}, //03FF// #AFIT16_Demosaicing_iSatVal
{0x0F12, 0x0000}, //0000// #AFIT16_Sharpening_iLowSharpClamp
{0x0F12, 0x005A}, //005A// #AFIT16_Sharpening_iLowSharpClamp_Bin  
{0x0F12, 0x0000}, //0000// #AFIT16_Sharpening_iHighSharpClamp  
{0x0F12, 0x0040}, //0040// #AFIT16_Sharpening_iHighSharpClamp_Bin 
{0x0F12, 0x0019}, //0019// #AFIT16_Sharpening_iReduceEdgeThresh
{0x0F12, 0x0100}, //0100// #AFIT16_DemSharpMix_iRGBOffset
{0x0F12, 0x01F4}, //01F4// #AFIT16_DemSharpMix_iDemClamp 
{0x0F12, 0x00D5}, //00D5// #AFIT16_DemSharpMix_iLowThreshold
{0x0F12, 0x00DE}, //00DE// #AFIT16_DemSharpMix_iHighThreshold  
{0x0F12, 0x0100}, //0100// #AFIT16_DemSharpMix_iLowBright
{0x0F12, 0x03E8}, //03E8// #AFIT16_DemSharpMix_iHighBright  
{0x0F12, 0x0020}, //0020// #AFIT16_DemSharpMix_iLowSat
{0x0F12, 0x0078}, //0078// #AFIT16_DemSharpMix_iHighSat  
{0x0F12, 0x0070}, //0070// #AFIT16_DemSharpMix_iTune  
{0x0F12, 0x000A}, //000A// #AFIT16_DemSharpMix_iHystThLow
{0x0F12, 0x000A}, //000A// #AFIT16_DemSharpMix_iHystThHigh  
{0x0F12, 0x01CE}, //01CE// #AFIT16_DemSharpMix_iHystCenter  
{0x0F12, 0x0008}, //0008// #AFIT8_DDD_edge_low, AFIT8_DDD_repl_thresh
{0x0F12, 0xF804}, //F804// #AFIT8_DDD_repl_force, AFIT8_DDD_sat_level
{0x0F12, 0x010C}, //010C// #AFIT8_DDD_sat_thr , AFIT8_DDD_sat_mpl 
{0x0F12, 0x0003}, //0003// #AFIT8_DDD_sat_noise , AFIT8_DDD_iClustThresh_H 
{0x0F12, 0x0000}, //0000// #AFIT8_DDD_iClustThresh_H_Bin,  AFIT8_DDD_iClustThresh_C 
{0x0F12, 0x0000}, //0000// #AFIT8_DDD_iClustThresh_C_Bin,  AFIT8_DDD_iClustMulT_H
{0x0F12, 0x0000}, //0000// #AFIT8_DDD_iClustMulT_H_Bin  ,  AFIT8_DDD_iClustMulT_C
{0x0F12, 0x0100}, //0100// #AFIT8_DDD_iClustMulT_C_Bin  ,  AFIT8_DDD_nClustLevel_H  
{0x0F12, 0x0002}, //0002// #AFIT8_DDD_nClustLevel_H_Bin ,  AFIT8_DDD_iMaxSlopeAllowed  
{0x0F12, 0x0000}, //0000// #AFIT8_DDD_iHotThreshHigh ,  AFIT8_DDD_iHotThreshLow  
{0x0F12, 0x0000}, //0000// #AFIT8_DDD_iColdThreshHigh,  AFIT8_DDD_iColdThreshLow 
{0x0F12, 0x8EFF}, //8EFF// #AFIT8_DDD_DispTH_Low, AFIT8_DDD_DispTH_Low_Bin 
{0x0F12, 0x8EFF}, //8EFF// #AFIT8_DDD_DispTH_High ,  AFIT8_DDD_DispTH_High_Bin
{0x0F12, 0xC8FB}, //C8FB// #AFIT8_DDD_iDenThreshLow  ,  AFIT8_DDD_iDenThreshLow_Bin  
{0x0F12, 0xC8FA}, //C8FA// #AFIT8_DDD_iDenThreshHigh ,  AFIT8_DDD_iDenThreshHigh_Bin 
{0x0F12, 0x0000}, //0000// #AFIT8_DDD_AddNoisePower1 ,  AFIT8_DDD_AddNoisePower2 
{0x0F12, 0x00FF}, //00FF// #AFIT8_DDD_iSatSat , AFIT8_DDD_iRadialTune
{0x0F12, 0x0F32}, //0F32// #AFIT8_DDD_iRadialLimit,  AFIT8_DDD_iRadialPower
{0x0F12, 0x2828}, //2828// #AFIT8_DDD_iLowMaxSlopeAllowed  , AFIT8_DDD_iHighMaxSlopeAllowed  
{0x0F12, 0x0606}, //0606// #AFIT8_DDD_iLowSlopeThresh,  AFIT8_DDD_iHighSlopeThresh  
{0x0F12, 0x8002}, //8002// #AFIT8_DDD_iSquaresRounding  ,  AFIT8_Demosaicing_iCentGrad 
{0x0F12, 0x0620}, //0620// #AFIT8_Demosaicing_iMonochrom,  AFIT8_Demosaicing_iDecisionThresh 
{0x0F12, 0x001E}, //001E// #AFIT8_Demosaicing_iDesatThresh ,  AFIT8_Demosaicing_iEnhThresh
{0x0F12, 0x0000}, //0000// #AFIT8_Demosaicing_iGRDenoiseVal,  AFIT8_Demosaicing_iGBDenoiseVal
{0x0F12, 0x0505}, //0505// #AFIT8_Demosaicing_iEdgeDesat,  AFIT8_Demosaicing_iEdgeDesat_Bin  
{0x0F12, 0x0000}, //0000// #AFIT8_Demosaicing_iEdgeDesatThrLow , AFIT8_Demosaicing_iEdgeDesatThrLow_Bin  
{0x0F12, 0x0A0A}, //0A0A// #AFIT8_Demosaicing_iEdgeDesatThrHigh , AFIT8_Demosaicing_iEdgeDesatThrHigh_Bin 
{0x0F12, 0x0404}, //0404// #AFIT8_Demosaicing_iEdgeDesatLimit , AFIT8_Demosaicing_iEdgeDesatLimit_Bin 
{0x0F12, 0x2306}, //2303// #AFIT8_Demosaicing_iNearGrayDesat  , AFIT8_Demosaicing_iDemSharpenLow
{0x0F12, 0x3214}, //3214// #AFIT8_Demosaicing_iDemSharpenLow_Bin , AFIT8_Demosaicing_iDemSharpenHigh  
{0x0F12, 0x370A}, //370A// #AFIT8_Demosaicing_iDemSharpenHigh_Bin , AFIT8_Demosaicing_iDemSharpThresh 
{0x0F12, 0x2080}, //2080// #AFIT8_Demosaicing_iDemSharpThresh_Bin , AFIT8_Demosaicing_iDemShLowLimit  
{0x0F12, 0x0408}, //0408// #AFIT8_Demosaicing_iDemShLowLimit_Bin , AFIT8_Demosaicing_iDespeckleForDemsharp  
{0x0F12, 0x1008}, //1008// #AFIT8_Demosaicing_iDespeckleForDemsharp_Bin , AFIT8_Demosaicing_iDFD_ReduceCoeff
{0x0F12, 0xFF1E}, //FF1E// #AFIT8_Demosaicing_iDemBlurLow  ,  AFIT8_Demosaicing_iDemBlurLow_Bin 
{0x0F12, 0xFF1E}, //FF1E// #AFIT8_Demosaicing_iDemBlurHigh ,  AFIT8_Demosaicing_iDemBlurHigh_Bin
{0x0F12, 0x0606}, //0606// #AFIT8_Demosaicing_iDemBlurRange,  AFIT8_Demosaicing_iDemBlurRange_Bin  
{0x0F12, 0x1900}, //1900// #AFIT8_Sharpening_iLowSharpPower,  AFIT8_Sharpening_iLowSharpPower_Bin  
{0x0F12, 0x4B00}, //4B00// #AFIT8_Sharpening_iHighSharpPower  , AFIT8_Sharpening_iHighSharpPower_Bin  
{0x0F12, 0x0000}, //0000// #AFIT8_Sharpening_iMSharpen  ,  AFIT8_Sharpening_iMShThresh 
{0x0F12, 0x2200}, //2200// #AFIT8_Sharpening_iWSharpen  ,  AFIT8_Sharpening_iWShThresh 
{0x0F12, 0x9A00}, //9A00// #AFIT8_Sharpening_nSharpWidth,  AFIT8_Sharpening_iLowShDenoise 
{0x0F12, 0x7C0A}, //7C0A// #AFIT8_Sharpening_iLowShDenoise_Bin , AFIT8_Sharpening_iHighShDenoise 
{0x0F12, 0x023C}, //023C// #AFIT8_Sharpening_iHighShDenoise_Bin , AFIT8_Sharpening_iReduceNegative 
{0x0F12, 0x4000}, //4000// #AFIT8_Sharpening_iShDespeckle  ,  AFIT8_Sharpening_iReduceEdgeMinMult  
{0x0F12, 0x0F10}, //0F10// #AFIT8_Sharpening_iReduceEdgeMinMult_Bin , AFIT8_Sharpening_iReduceEdgeSlope  
{0x0F12, 0x0004}, //0004// #AFIT8_Sharpening_iReduceEdgeSlope_Bin , AFIT8_DemSharpMix_iRGBMultiplier  
{0x0F12, 0x0F00}, //0F00// #AFIT8_DemSharpMix_iFilterPower,  AFIT8_DemSharpMix_iBCoeff 
{0x0F12, 0x0A18}, //0A18// #AFIT8_DemSharpMix_iGCoeff  ,  AFIT8_DemSharpMix_iWideMult  
{0x0F12, 0x0900}, //0900// #AFIT8_DemSharpMix_iNarrMult,  AFIT8_DemSharpMix_iWideFiltReduce  
{0x0F12, 0x0902}, //0902// #AFIT8_DemSharpMix_iWideFiltReduce_Bin , AFIT8_DemSharpMix_iNarrFiltReduce 
{0x0F12, 0x0403}, //0403// #AFIT8_DemSharpMix_iNarrFiltReduce_Bin , AFIT8_DemSharpMix_iHystFalloff 
{0x0F12, 0x0600}, //0600// #AFIT8_DemSharpMix_iHystMinMult,  AFIT8_DemSharpMix_iHystWidth 
{0x0F12, 0x0201}, //0201// #AFIT8_DemSharpMix_iHystFallLow,  AFIT8_DemSharpMix_iHystFallHigh 
{0x0F12, 0x5004}, //5004// #AFIT8_DemSharpMix1_iHystTune,  AFIT8_byr_gras_iShadingPower
{0x0F12, 0x0080}, //0080// #AFIT8_RGBGamma2_iLinearity  ,  AFIT8_RGBGamma2_iDarkReduce 
{0x0F12, 0x003C}, //003C// #AFIT8_ccm_oscar_iSaturation ,  AFIT8_RGB2YUV_iYOffset
{0x0F12, 0x0080}, //0080// #AFIT8_RGB2YUV_iRGBGain , AFIT8_RGB2YUV_iSaturation
{0x0F12, 0x0080}, //0080// #AFIT8_DRx_iBlendingSupress
{0x0F12, 0x0000}, //0000// #AFIT16_BRIGHTNESS //x4  
{0x0F12, 0x0000}, //0000// #AFIT16_CONTRAST  
{0x0F12, 0x0000}, //0000// #AFIT16_SATURATION
{0x0F12, 0x0005}, //0005// #AFIT16_SHARP_BLUR
{0x0F12, 0xFFFB}, //FFFB// #AFIT16_GLAMOUR
{0x0F12, 0x0080}, //0080// #AFIT16_DDD_edge_high
{0x0F12, 0x03FF}, //03FF// #AFIT16_Demosaicing_iSatVal
{0x0F12, 0x0081}, //0081// #AFIT16_Sharpening_iLowSharpClamp
{0x0F12, 0x005A}, //005A// #AFIT16_Sharpening_iLowSharpClamp_Bin  
{0x0F12, 0x0016}, //0016// #AFIT16_Sharpening_iHighSharpClamp  
{0x0F12, 0x0040}, //0040// #AFIT16_Sharpening_iHighSharpClamp_Bin 
{0x0F12, 0x0019}, //0019// #AFIT16_Sharpening_iReduceEdgeThresh
{0x0F12, 0x0100}, //0100// #AFIT16_DemSharpMix_iRGBOffset
{0x0F12, 0x01F4}, //01F4// #AFIT16_DemSharpMix_iDemClamp 
{0x0F12, 0x00D5}, //00D5// #AFIT16_DemSharpMix_iLowThreshold
{0x0F12, 0x00DE}, //00DE// #AFIT16_DemSharpMix_iHighThreshold  
{0x0F12, 0x0100}, //0100// #AFIT16_DemSharpMix_iLowBright
{0x0F12, 0x03E8}, //03E8// #AFIT16_DemSharpMix_iHighBright  
{0x0F12, 0x0020}, //0020// #AFIT16_DemSharpMix_iLowSat
{0x0F12, 0x0078}, //0078// #AFIT16_DemSharpMix_iHighSat  
{0x0F12, 0x0070}, //0070// #AFIT16_DemSharpMix_iTune  
{0x0F12, 0x0005}, //0005// #AFIT16_DemSharpMix_iHystThLow
{0x0F12, 0x000A}, //000A// #AFIT16_DemSharpMix_iHystThHigh  
{0x0F12, 0x01CE}, //01CE// #AFIT16_DemSharpMix_iHystCenter  
{0x0F12, 0x0008}, //0008// #AFIT8_DDD_edge_low, AFIT8_DDD_repl_thresh
{0x0F12, 0xF804}, //F804// #AFIT8_DDD_repl_force, AFIT8_DDD_sat_level
{0x0F12, 0x010C}, //010C// #AFIT8_DDD_sat_thr , AFIT8_DDD_sat_mpl 
{0x0F12, 0x1003}, //1003// #AFIT8_DDD_sat_noise , AFIT8_DDD_iClustThresh_H 
{0x0F12, 0x2010}, //2010// #AFIT8_DDD_iClustThresh_H_Bin,  AFIT8_DDD_iClustThresh_C 
{0x0F12, 0x05FF}, //05FF// #AFIT8_DDD_iClustThresh_C_Bin,  AFIT8_DDD_iClustMulT_H
{0x0F12, 0x0000}, //0000// #AFIT8_DDD_iClustMulT_H_Bin  ,  AFIT8_DDD_iClustMulT_C
{0x0F12, 0x0100}, //0100// #AFIT8_DDD_iClustMulT_C_Bin  ,  AFIT8_DDD_nClustLevel_H  
{0x0F12, 0x0002}, //0002// #AFIT8_DDD_nClustLevel_H_Bin ,  AFIT8_DDD_iMaxSlopeAllowed  
{0x0F12, 0x0000}, //0000// #AFIT8_DDD_iHotThreshHigh ,  AFIT8_DDD_iHotThreshLow  
{0x0F12, 0x0000}, //0000// #AFIT8_DDD_iColdThreshHigh,  AFIT8_DDD_iColdThreshLow 
{0x0F12, 0x8E32}, //8E32// #AFIT8_DDD_DispTH_Low, AFIT8_DDD_DispTH_Low_Bin 
{0x0F12, 0x8E32}, //8E32// #AFIT8_DDD_DispTH_High ,  AFIT8_DDD_DispTH_High_Bin
{0x0F12, 0x6450}, //6450// #AFIT8_DDD_iDenThreshLow  ,  AFIT8_DDD_iDenThreshLow_Bin 
{0x0F12, 0x6432}, //6432// #AFIT8_DDD_iDenThreshHigh ,  AFIT8_DDD_iDenThreshHigh_Bin
{0x0F12, 0x0000}, //0000// #AFIT8_DDD_AddNoisePower1 ,  AFIT8_DDD_AddNoisePower2 
{0x0F12, 0x00FF}, //00FF// #AFIT8_DDD_iSatSat , AFIT8_DDD_iRadialTune
{0x0F12, 0x0F32}, //0F32// #AFIT8_DDD_iRadialLimit,  AFIT8_DDD_iRadialPower
{0x0F12, 0x2828}, //2828// #AFIT8_DDD_iLowMaxSlopeAllowed  , AFIT8_DDD_iHighMaxSlopeAllowed  
{0x0F12, 0x0606}, //0606// #AFIT8_DDD_iLowSlopeThresh,  AFIT8_DDD_iHighSlopeThresh  
{0x0F12, 0x8002}, //8002// #AFIT8_DDD_iSquaresRounding  ,  AFIT8_Demosaicing_iCentGrad 
{0x0F12, 0x0620}, //0620// #AFIT8_Demosaicing_iMonochrom,  AFIT8_Demosaicing_iDecisionThresh 
{0x0F12, 0x001E}, //001E// #AFIT8_Demosaicing_iDesatThresh ,  AFIT8_Demosaicing_iEnhThresh
{0x0F12, 0x0000}, //0000// #AFIT8_Demosaicing_iGRDenoiseVal,  AFIT8_Demosaicing_iGBDenoiseVal
{0x0F12, 0x0505}, //0505// #AFIT8_Demosaicing_iEdgeDesat,  AFIT8_Demosaicing_iEdgeDesat_Bin  
{0x0F12, 0x0000}, //0000// #AFIT8_Demosaicing_iEdgeDesatThrLow , AFIT8_Demosaicing_iEdgeDesatThrLow_Bin  
{0x0F12, 0x0A0A}, //0A0A// #AFIT8_Demosaicing_iEdgeDesatThrHigh , AFIT8_Demosaicing_iEdgeDesatThrHigh_Bin 
{0x0F12, 0x0404}, //0404// #AFIT8_Demosaicing_iEdgeDesatLimit , AFIT8_Demosaicing_iEdgeDesatLimit_Bin 
{0x0F12, 0x2306}, //2303// #AFIT8_Demosaicing_iNearGrayDesat  , AFIT8_Demosaicing_iDemSharpenLow
{0x0F12, 0x3214}, //3214// #AFIT8_Demosaicing_iDemSharpenLow_Bin , AFIT8_Demosaicing_iDemSharpenHigh  
{0x0F12, 0x370A}, //370A// #AFIT8_Demosaicing_iDemSharpenHigh_Bin , AFIT8_Demosaicing_iDemSharpThresh 
{0x0F12, 0x2080}, //2080// #AFIT8_Demosaicing_iDemSharpThresh_Bin , AFIT8_Demosaicing_iDemShLowLimit  
{0x0F12, 0x0808}, //0808// #AFIT8_Demosaicing_iDemShLowLimit_Bin , AFIT8_Demosaicing_iDespeckleForDemsharp  
{0x0F12, 0x1008}, //1008// #AFIT8_Demosaicing_iDespeckleForDemsharp_Bin , AFIT8_Demosaicing_iDFD_ReduceCoeff
{0x0F12, 0xFF1E}, //FF1E// #AFIT8_Demosaicing_iDemBlurLow  ,  AFIT8_Demosaicing_iDemBlurLow_Bin 
{0x0F12, 0xFF19}, //FF19// #AFIT8_Demosaicing_iDemBlurHigh ,  AFIT8_Demosaicing_iDemBlurHigh_Bin
{0x0F12, 0x0604}, //0604// #AFIT8_Demosaicing_iDemBlurRange,  AFIT8_Demosaicing_iDemBlurRange_Bin  
{0x0F12, 0x191E}, //191E// #AFIT8_Sharpening_iLowSharpPower,  AFIT8_Sharpening_iLowSharpPower_Bin  
{0x0F12, 0x4B1E}, //4B1E// #AFIT8_Sharpening_iHighSharpPower  , AFIT8_Sharpening_iHighSharpPower_Bin  
{0x0F12, 0x0064}, //0064// #AFIT8_Sharpening_iMSharpen  ,  AFIT8_Sharpening_iMShThresh 
{0x0F12, 0x2228}, //2228// #AFIT8_Sharpening_iWSharpen  ,  AFIT8_Sharpening_iWShThresh 
{0x0F12, 0x3200}, //3200// #AFIT8_Sharpening_nSharpWidth,  AFIT8_Sharpening_iLowShDenoise 
{0x0F12, 0x140A}, //140A// #AFIT8_Sharpening_iLowShDenoise_Bin , AFIT8_Sharpening_iHighShDenoise
{0x0F12, 0x023C}, //023C// #AFIT8_Sharpening_iHighShDenoise_Bin , AFIT8_Sharpening_iReduceNegative 
{0x0F12, 0x4000}, //4000// #AFIT8_Sharpening_iShDespeckle  ,  AFIT8_Sharpening_iReduceEdgeMinMult  
{0x0F12, 0x0F10}, //0F10// #AFIT8_Sharpening_iReduceEdgeMinMult_Bin , AFIT8_Sharpening_iReduceEdgeSlope  
{0x0F12, 0x0004}, //0004// #AFIT8_Sharpening_iReduceEdgeSlope_Bin , AFIT8_DemSharpMix_iRGBMultiplier  
{0x0F12, 0x0F00}, //0F00// #AFIT8_DemSharpMix_iFilterPower,  AFIT8_DemSharpMix_iBCoeff 
{0x0F12, 0x0A18}, //0A18// #AFIT8_DemSharpMix_iGCoeff  ,  AFIT8_DemSharpMix_iWideMult  
{0x0F12, 0x0900}, //0900// #AFIT8_DemSharpMix_iNarrMult,  AFIT8_DemSharpMix_iWideFiltReduce  
{0x0F12, 0x0902}, //0902// #AFIT8_DemSharpMix_iWideFiltReduce_Bin , AFIT8_DemSharpMix_iNarrFiltReduce 
{0x0F12, 0x0003}, //0003// #AFIT8_DemSharpMix_iNarrFiltReduce_Bin , AFIT8_DemSharpMix_iHystFalloff 
{0x0F12, 0x0600}, //0600// #AFIT8_DemSharpMix_iHystMinMult,  AFIT8_DemSharpMix_iHystWidth 
{0x0F12, 0x0201}, //0201// #AFIT8_DemSharpMix_iHystFallLow,  AFIT8_DemSharpMix_iHystFallHigh 
{0x0F12, 0x5A03}, //5A03// #AFIT8_DemSharpMix1_iHystTune,  AFIT8_byr_gras_iShadingPower
{0x0F12, 0x0080}, //0080// #AFIT8_RGBGamma2_iLinearity  ,  AFIT8_RGBGamma2_iDarkReduce 
{0x0F12, 0x0050}, //0050// #AFIT8_ccm_oscar_iSaturation ,  AFIT8_RGB2YUV_iYOffset
{0x0F12, 0x0080}, //0080// #AFIT8_RGB2YUV_iRGBGain , AFIT8_RGB2YUV_iSaturation
{0x0F12, 0x0040}, //0040// #AFIT8_DRx_iBlendingSupress
{0x0F12, 0x0000}, //0000// #AFIT16_BRIGHTNESS //x3 
{0x0F12, 0x0005}, //0005// #AFIT16_CONTRAST  
{0x0F12, 0x0000}, //0000// #AFIT16_SATURATION
{0x0F12, 0x0005}, //0005// #AFIT16_SHARP_BLUR
{0x0F12, 0xFFFB}, //FFFB// #AFIT16_GLAMOUR
{0x0F12, 0x0080}, //0080// #AFIT16_DDD_edge_high
{0x0F12, 0x03FF}, //03FF// #AFIT16_Demosaicing_iSatVal
{0x0F12, 0x0081}, //0081// #AFIT16_Sharpening_iLowSharpClamp
{0x0F12, 0x005A}, //005A// #AFIT16_Sharpening_iLowSharpClamp_Bin  
{0x0F12, 0x0031}, //0031// #AFIT16_Sharpening_iHighSharpClamp  
{0x0F12, 0x0040}, //0040// #AFIT16_Sharpening_iHighSharpClamp_Bin 
{0x0F12, 0x0019}, //0019// #AFIT16_Sharpening_iReduceEdgeThresh
{0x0F12, 0x0100}, //0100// #AFIT16_DemSharpMix_iRGBOffset
{0x0F12, 0x01F4}, //01F4// #AFIT16_DemSharpMix_iDemClamp 
{0x0F12, 0x0009}, //0009// #AFIT16_DemSharpMix_iLowThreshold
{0x0F12, 0x0012}, //0012// #AFIT16_DemSharpMix_iHighThreshold  
{0x0F12, 0x0100}, //0100// #AFIT16_DemSharpMix_iLowBright
{0x0F12, 0x03E8}, //03E8// #AFIT16_DemSharpMix_iHighBright  
{0x0F12, 0x0020}, //0020// #AFIT16_DemSharpMix_iLowSat
{0x0F12, 0x0078}, //0078// #AFIT16_DemSharpMix_iHighSat  
{0x0F12, 0x0070}, //0070// #AFIT16_DemSharpMix_iTune  
{0x0F12, 0x0000}, //0000// #AFIT16_DemSharpMix_iHystThLow
{0x0F12, 0x0005}, //0005// #AFIT16_DemSharpMix_iHystThHigh  
{0x0F12, 0x01CE}, //01CE// #AFIT16_DemSharpMix_iHystCenter  
{0x0F12, 0x0008}, //0008// #AFIT8_DDD_edge_low, AFIT8_DDD_repl_thresh
{0x0F12, 0xF804}, //F804// #AFIT8_DDD_repl_force, AFIT8_DDD_sat_level
{0x0F12, 0x010C}, //010C// #AFIT8_DDD_sat_thr , AFIT8_DDD_sat_mpl 
{0x0F12, 0x1003}, //1003// #AFIT8_DDD_sat_noise , AFIT8_DDD_iClustThresh_H 
{0x0F12, 0x2010}, //2010// #AFIT8_DDD_iClustThresh_H_Bin,  AFIT8_DDD_iClustThresh_C 
{0x0F12, 0x05FF}, //05FF// #AFIT8_DDD_iClustThresh_C_Bin,  AFIT8_DDD_iClustMulT_H
{0x0F12, 0x0305}, //0305// #AFIT8_DDD_iClustMulT_H_Bin  ,  AFIT8_DDD_iClustMulT_C
{0x0F12, 0x0103}, //0103// #AFIT8_DDD_iClustMulT_C_Bin  ,  AFIT8_DDD_nClustLevel_H  
{0x0F12, 0x0001}, //0001// #AFIT8_DDD_nClustLevel_H_Bin ,  AFIT8_DDD_iMaxSlopeAllowed  
{0x0F12, 0x0000}, //0000// #AFIT8_DDD_iHotThreshHigh ,  AFIT8_DDD_iHotThreshLow  
{0x0F12, 0x0007}, //0007// #AFIT8_DDD_iColdThreshHigh,  AFIT8_DDD_iColdThreshLow 
{0x0F12, 0x2823}, //2823// #AFIT8_DDD_DispTH_Low, AFIT8_DDD_DispTH_Low_Bin 
{0x0F12, 0x2823}, //2823// #AFIT8_DDD_DispTH_High ,  AFIT8_DDD_DispTH_High_Bin
{0x0F12, 0x4628}, //4628// #AFIT8_DDD_iDenThreshLow  ,  AFIT8_DDD_iDenThreshLow_Bin 
{0x0F12, 0x4628}, //4628// #AFIT8_DDD_iDenThreshHigh ,  AFIT8_DDD_iDenThreshHigh_Bin
{0x0F12, 0x0000}, //0000// #AFIT8_DDD_AddNoisePower1 ,  AFIT8_DDD_AddNoisePower2 
{0x0F12, 0x00FF}, //00FF// #AFIT8_DDD_iSatSat , AFIT8_DDD_iRadialTune
{0x0F12, 0x0F32}, //0F32// #AFIT8_DDD_iRadialLimit,  AFIT8_DDD_iRadialPower
{0x0F12, 0x2828}, //2828// #AFIT8_DDD_iLowMaxSlopeAllowed  , AFIT8_DDD_iHighMaxSlopeAllowed  
{0x0F12, 0x0606}, //0606// #AFIT8_DDD_iLowSlopeThresh,  AFIT8_DDD_iHighSlopeThresh  
{0x0F12, 0x8007}, //8007// #AFIT8_DDD_iSquaresRounding  ,  AFIT8_Demosaicing_iCentGrad 
{0x0F12, 0x0620}, //0620// #AFIT8_Demosaicing_iMonochrom,  AFIT8_Demosaicing_iDecisionThresh 
{0x0F12, 0x001E}, //001E// #AFIT8_Demosaicing_iDesatThresh ,  AFIT8_Demosaicing_iEnhThresh
{0x0F12, 0x0000}, //0000// #AFIT8_Demosaicing_iGRDenoiseVal,  AFIT8_Demosaicing_iGBDenoiseVal
{0x0F12, 0x0505}, //0505// #AFIT8_Demosaicing_iEdgeDesat,  AFIT8_Demosaicing_iEdgeDesat_Bin  
{0x0F12, 0x0000}, //0000// #AFIT8_Demosaicing_iEdgeDesatThrLow , AFIT8_Demosaicing_iEdgeDesatThrLow_Bin  
{0x0F12, 0x0A0A}, //0A0A// #AFIT8_Demosaicing_iEdgeDesatThrHigh , AFIT8_Demosaicing_iEdgeDesatThrHigh_Bin 
{0x0F12, 0x0404}, //0404// #AFIT8_Demosaicing_iEdgeDesatLimit , AFIT8_Demosaicing_iEdgeDesatLimit_Bin 
{0x0F12, 0x2306}, //2303// #AFIT8_Demosaicing_iNearGrayDesat  , AFIT8_Demosaicing_iDemSharpenLow
{0x0F12, 0x3214}, //3214// #AFIT8_Demosaicing_iDemSharpenLow_Bin , AFIT8_Demosaicing_iDemSharpenHigh  
{0x0F12, 0x2D0A}, //2D0A// #AFIT8_Demosaicing_iDemSharpenHigh_Bin , AFIT8_Demosaicing_iDemSharpThresh  
{0x0F12, 0x0880}, //0880// #AFIT8_Demosaicing_iDemSharpThresh_Bin , AFIT8_Demosaicing_iDemShLowLimit  
{0x0F12, 0x0808}, //0808// #AFIT8_Demosaicing_iDemShLowLimit_Bin , AFIT8_Demosaicing_iDespeckleForDemsharp  
{0x0F12, 0x1008}, //1008// #AFIT8_Demosaicing_iDespeckleForDemsharp_Bin , AFIT8_Demosaicing_iDFD_ReduceCoeff
{0x0F12, 0xFF1E}, //FF1E// #AFIT8_Demosaicing_iDemBlurLow  ,  AFIT8_Demosaicing_iDemBlurLow_Bin 
{0x0F12, 0xFF19}, //FF19// #AFIT8_Demosaicing_iDemBlurHigh ,  AFIT8_Demosaicing_iDemBlurHigh_Bin
{0x0F12, 0x0604}, //0604// #AFIT8_Demosaicing_iDemBlurRange,  AFIT8_Demosaicing_iDemBlurRange_Bin  
{0x0F12, 0x191E}, //191E// #AFIT8_Sharpening_iLowSharpPower,  AFIT8_Sharpening_iLowSharpPower_Bin  
{0x0F12, 0x4B1E}, //4B1E// #AFIT8_Sharpening_iHighSharpPower  , AFIT8_Sharpening_iHighSharpPower_Bin  
{0x0F12, 0x006F}, //006F// #AFIT8_Sharpening_iMSharpen  ,  AFIT8_Sharpening_iMShThresh 
{0x0F12, 0x223C}, //223C// #AFIT8_Sharpening_iWSharpen  ,  AFIT8_Sharpening_iWShThresh 
{0x0F12, 0x2800}, //2800// #AFIT8_Sharpening_nSharpWidth,  AFIT8_Sharpening_iLowShDenoise 
{0x0F12, 0x0A0A}, //0A0A// #AFIT8_Sharpening_iLowShDenoise_Bin , AFIT8_Sharpening_iHighShDenoise 
{0x0F12, 0x013C}, //013C// #AFIT8_Sharpening_iHighShDenoise_Bin , AFIT8_Sharpening_iReduceNegative 
{0x0F12, 0x4000}, //4000// #AFIT8_Sharpening_iShDespeckle  ,  AFIT8_Sharpening_iReduceEdgeMinMult  
{0x0F12, 0x0F10}, //0F10// #AFIT8_Sharpening_iReduceEdgeMinMult_Bin , AFIT8_Sharpening_iReduceEdgeSlope  
{0x0F12, 0x0004}, //0004// #AFIT8_Sharpening_iReduceEdgeSlope_Bin , AFIT8_DemSharpMix_iRGBMultiplier  
{0x0F12, 0x0F00}, //0F00// #AFIT8_DemSharpMix_iFilterPower,  AFIT8_DemSharpMix_iBCoeff 
{0x0F12, 0x0A18}, //0A18// #AFIT8_DemSharpMix_iGCoeff  ,  AFIT8_DemSharpMix_iWideMult  
{0x0F12, 0x0900}, //0900// #AFIT8_DemSharpMix_iNarrMult,  AFIT8_DemSharpMix_iWideFiltReduce  
{0x0F12, 0x0902}, //0902// #AFIT8_DemSharpMix_iWideFiltReduce_Bin , AFIT8_DemSharpMix_iNarrFiltReduce 
{0x0F12, 0x0003}, //0003// #AFIT8_DemSharpMix_iNarrFiltReduce_Bin , AFIT8_DemSharpMix_iHystFalloff 
{0x0F12, 0x0600}, //0600// #AFIT8_DemSharpMix_iHystMinMult,  AFIT8_DemSharpMix_iHystWidth 
{0x0F12, 0x0201}, //0201// #AFIT8_DemSharpMix_iHystFallLow,  AFIT8_DemSharpMix_iHystFallHigh 
{0x0F12, 0x5A03}, //5A03// #AFIT8_DemSharpMix1_iHystTune,  AFIT8_byr_gras_iShadingPower
{0x0F12, 0x0080}, //0080// #AFIT8_RGBGamma2_iLinearity  ,  AFIT8_RGBGamma2_iDarkReduce 
{0x0F12, 0x0080}, //0080// #AFIT8_Ccm_oscar_iSaturation ,  AFIT8_RGB2YUV_iYOffset
{0x0F12, 0x0080}, //0080// #AFIT8_RGB2YUV_iRGBGain , AFIT8_RGB2YUV_iSaturation
{0x0F12, 0x0020}, //0020// #AFIT8_DRx_iBlendingSupress
{0x0F12, 0x0000}, //0000// #AFIT16_BRIGHTNESS //x2 
{0x0F12, 0x0005}, //0005// #AFIT16_CONTRAST  
{0x0F12, 0x0000}, //0000// #AFIT16_SATURATION
{0x0F12, 0x0005}, //0005// #AFIT16_SHARP_BLUR
{0x0F12, 0xFFFB}, //FFFB// #AFIT16_GLAMOUR
{0x0F12, 0x0080}, //0080// #AFIT16_DDD_edge_high
{0x0F12, 0x03FF}, //03FF// #AFIT16_Demosaicing_iSatVal
{0x0F12, 0x00F3}, //00F3// #AFIT16_Sharpening_iLowSharpClamp
{0x0F12, 0x005A}, //005A// #AFIT16_Sharpening_iLowSharpClamp_Bin  
{0x0F12, 0x004B}, //004B// #AFIT16_Sharpening_iHighSharpClamp  
{0x0F12, 0x0040}, //0040// #AFIT16_Sharpening_iHighSharpClamp_Bin 
{0x0F12, 0x0019}, //0019// #AFIT16_Sharpening_iReduceEdgeThresh
{0x0F12, 0x0100}, //0100// #AFIT16_DemSharpMix_iRGBOffset
{0x0F12, 0x01F4}, //01F4// #AFIT16_DemSharpMix_iDemClamp 
{0x0F12, 0x0009}, //0009// #AFIT16_DemSharpMix_iLowThreshold
{0x0F12, 0x0012}, //0012// #AFIT16_DemSharpMix_iHighThreshold  
{0x0F12, 0x0100}, //0100// #AFIT16_DemSharpMix_iLowBright
{0x0F12, 0x03E8}, //03E8// #AFIT16_DemSharpMix_iHighBright  
{0x0F12, 0x0020}, //0020// #AFIT16_DemSharpMix_iLowSat
{0x0F12, 0x0078}, //0078// #AFIT16_DemSharpMix_iHighSat  
{0x0F12, 0x0070}, //0070// #AFIT16_DemSharpMix_iTune  
{0x0F12, 0x0000}, //0000// #AFIT16_DemSharpMix_iHystThLow
{0x0F12, 0x0000}, //0000// #AFIT16_DemSharpMix_iHystThHigh  
{0x0F12, 0x01CE}, //01CE// #AFIT16_DemSharpMix_iHystCenter  
{0x0F12, 0x0008}, //0008// #AFIT8_DDD_edge_low, AFIT8_DDD_repl_thresh
{0x0F12, 0xF804}, //F804// #AFIT8_DDD_repl_force, AFIT8_DDD_sat_level
{0x0F12, 0x010C}, //010C// #AFIT8_DDD_sat_thr , AFIT8_DDD_sat_mpl 
{0x0F12, 0x1003}, //1003// #AFIT8_DDD_sat_noise , AFIT8_DDD_iClustThresh_H 
{0x0F12, 0x2010}, //2010// #AFIT8_DDD_iClustThresh_H_Bin,  AFIT8_DDD_iClustThresh_C 
{0x0F12, 0x05FF}, //05FF// #AFIT8_DDD_iClustThresh_C_Bin,  AFIT8_DDD_iClustMulT_H
{0x0F12, 0x0305}, //0305// #AFIT8_DDD_iClustMulT_H_Bin  ,  AFIT8_DDD_iClustMulT_C
{0x0F12, 0x0103}, //0103// #AFIT8_DDD_iClustMulT_C_Bin  ,  AFIT8_DDD_nClustLevel_H  
{0x0F12, 0x0001}, //0001// #AFIT8_DDD_nClustLevel_H_Bin ,  AFIT8_DDD_iMaxSlopeAllowed  
{0x0F12, 0x0000}, //0000// #AFIT8_DDD_iHotThreshHigh ,  AFIT8_DDD_iHotThreshLow  
{0x0F12, 0x0007}, //0007// #AFIT8_DDD_iColdThreshHigh,  AFIT8_DDD_iColdThreshLow 
{0x0F12, 0x2823}, //2823// #AFIT8_DDD_DispTH_Low, AFIT8_DDD_DispTH_Low_Bin 
{0x0F12, 0x2823}, //2823// #AFIT8_DDD_DispTH_High ,  AFIT8_DDD_DispTH_High_Bin
{0x0F12, 0x3228}, //3228// #AFIT8_DDD_iDenThreshLow  ,  AFIT8_DDD_iDenThreshLow_Bin 
{0x0F12, 0x3228}, //3228// #AFIT8_DDD_iDenThreshHigh ,  AFIT8_DDD_iDenThreshHigh_Bin
{0x0F12, 0x0000}, //0000// #AFIT8_DDD_AddNoisePower1 ,  AFIT8_DDD_AddNoisePower2 
{0x0F12, 0x00FF}, //00FF// #AFIT8_DDD_iSatSat , AFIT8_DDD_iRadialTune
{0x0F12, 0x0F32}, //0F32// #AFIT8_DDD_iRadialLimit,  AFIT8_DDD_iRadialPower
{0x0F12, 0x2828}, //2828// #AFIT8_DDD_iLowMaxSlopeAllowed  , AFIT8_DDD_iHighMaxSlopeAllowed  
{0x0F12, 0x0606}, //0606// #AFIT8_DDD_iLowSlopeThresh,  AFIT8_DDD_iHighSlopeThresh  
{0x0F12, 0x8007}, //8007// #AFIT8_DDD_iSquaresRounding  ,  AFIT8_Demosaicing_iCentGrad 
{0x0F12, 0x0620}, //0620// #AFIT8_Demosaicing_iMonochrom,  AFIT8_Demosaicing_iDecisionThresh 
{0x0F12, 0x001E}, //001E// #AFIT8_Demosaicing_iDesatThresh ,  AFIT8_Demosaicing_iEnhThresh
{0x0F12, 0x0000}, //0000// #AFIT8_Demosaicing_iGRDenoiseVal,  AFIT8_Demosaicing_iGBDenoiseVal
{0x0F12, 0x0505}, //0505// #AFIT8_Demosaicing_iEdgeDesat,  AFIT8_Demosaicing_iEdgeDesat_Bin  
{0x0F12, 0x0000}, //0000// #AFIT8_Demosaicing_iEdgeDesatThrLow , AFIT8_Demosaicing_iEdgeDesatThrLow_Bin  
{0x0F12, 0x0A0A}, //0A0A// #AFIT8_Demosaicing_iEdgeDesatThrHigh , AFIT8_Demosaicing_iEdgeDesatThrHigh_Bin 
{0x0F12, 0x0404}, //0404// #AFIT8_Demosaicing_iEdgeDesatLimit , AFIT8_Demosaicing_iEdgeDesatLimit_Bin 
{0x0F12, 0x2306}, //2303// #AFIT8_Demosaicing_iNearGrayDesat  , AFIT8_Demosaicing_iDemSharpenLow
{0x0F12, 0x3214}, //3214// #AFIT8_Demosaicing_iDemSharpenLow_Bin , AFIT8_Demosaicing_iDemSharpenHigh  
{0x0F12, 0x2D0A}, //2D0A// #AFIT8_Demosaicing_iDemSharpenHigh_Bin , AFIT8_Demosaicing_iDemSharpThresh  
{0x0F12, 0x0880}, //0880// #AFIT8_Demosaicing_iDemSharpThresh_Bin , AFIT8_Demosaicing_iDemShLowLimit  
{0x0F12, 0x0508}, //0508// #AFIT8_Demosaicing_iDemShLowLimit_Bin , AFIT8_Demosaicing_iDespeckleForDemsharp  
{0x0F12, 0x0A08}, //0A08// #AFIT8_Demosaicing_iDespeckleForDemsharp_Bin , AFIT8_Demosaicing_iDFD_ReduceCoeff
{0x0F12, 0x801E}, //801E// #AFIT8_Demosaicing_iDemBlurLow  ,  AFIT8_Demosaicing_iDemBlurLow_Bin 
{0x0F12, 0x0019}, //0019// #AFIT8_Demosaicing_iDemBlurHigh ,  AFIT8_Demosaicing_iDemBlurHigh_Bin
{0x0F12, 0x0604}, //0604// #AFIT8_Demosaicing_iDemBlurRange,  AFIT8_Demosaicing_iDemBlurRange_Bin  
{0x0F12, 0x191E}, //191E// #AFIT8_Sharpening_iLowSharpPower,  AFIT8_Sharpening_iLowSharpPower_Bin  
{0x0F12, 0x4B1E}, //4B1E// #AFIT8_Sharpening_iHighSharpPower  , AFIT8_Sharpening_iHighSharpPower_Bin  
{0x0F12, 0x006F}, //006F// #AFIT8_Sharpening_iMSharpen  ,  AFIT8_Sharpening_iMShThresh 
{0x0F12, 0x113C}, //113C// #AFIT8_Sharpening_iWSharpen  ,  AFIT8_Sharpening_iWShThresh 
{0x0F12, 0x1E00}, //1E00// #AFIT8_Sharpening_nSharpWidth,  AFIT8_Sharpening_iLowShDenoise 
{0x0F12, 0x0A0A}, //0A0A// #AFIT8_Sharpening_iLowShDenoise_Bin , AFIT8_Sharpening_iHighShDenoise 
{0x0F12, 0x013C}, //013C// #AFIT8_Sharpening_iHighShDenoise_Bin , AFIT8_Sharpening_iReduceNegative 
{0x0F12, 0x4000}, //4000// #AFIT8_Sharpening_iShDespeckle  ,  AFIT8_Sharpening_iReduceEdgeMinMult  
{0x0F12, 0x0F10}, //0F10// #AFIT8_Sharpening_iReduceEdgeMinMult_Bin , AFIT8_Sharpening_iReduceEdgeSlope  
{0x0F12, 0x0004}, //0004// #AFIT8_Sharpening_iReduceEdgeSlope_Bin , AFIT8_DemSharpMix_iRGBMultiplier  
{0x0F12, 0x0F00}, //0F00// #AFIT8_DemSharpMix_iFilterPower,  AFIT8_DemSharpMix_iBCoeff 
{0x0F12, 0x0A18}, //0A18// #AFIT8_DemSharpMix_iGCoeff  ,  AFIT8_DemSharpMix_iWideMult  
{0x0F12, 0x0900}, //0900// #AFIT8_DemSharpMix_iNarrMult,  AFIT8_DemSharpMix_iWideFiltReduce  
{0x0F12, 0x0903}, //0903// #AFIT8_DemSharpMix_iWideFiltReduce_Bin , AFIT8_DemSharpMix_iNarrFiltReduce 
{0x0F12, 0x0002}, //0002// #AFIT8_DemSharpMix_iNarrFiltReduce_Bin , AFIT8_DemSharpMix_iHystFalloff 
{0x0F12, 0x0800}, //0800// #AFIT8_DemSharpMix_iHystMinMult,  AFIT8_DemSharpMix_iHystWidth 
{0x0F12, 0x0104}, //0104// #AFIT8_DemSharpMix_iHystFallLow,  AFIT8_DemSharpMix_iHystFallHigh 
{0x0F12, 0x6402}, //6402// #AFIT8_DemSharpMix1_iHystTune,  AFIT8_byr_gras_iShadingPower
{0x0F12, 0x0080}, //0080// #AFIT8_RGBGamma2_iLinearity  ,  AFIT8_RGBGamma2_iDarkReduce 
{0x0F12, 0x0080}, //0080// #AFIT8_ccm_oscar_iSaturation ,  AFIT8_RGB2YUV_iYOffset
{0x0F12, 0x0080}, //0080// #AFIT8_RGB2YUV_iRGBGain , AFIT8_RGB2YUV_iSaturation
{0x0F12, 0x0010}, //0010// #AFIT8_DRx_iBlendingSupress
{0x0F12, 0x0000}, //0000// #AFIT16_BRIGHTNESS //x1 
{0x0F12, 0x0005}, //0005// #AFIT16_CONTRAST  
{0x0F12, 0x0000}, //0000// #AFIT16_SATURATION
{0x0F12, 0x0005}, //0005// #AFIT16_SHARP_BLUR
{0x0F12, 0xFFFB}, //FFFB// #AFIT16_GLAMOUR
{0x0F12, 0x00C0}, //00C0// #AFIT16_DDD_edge_high
{0x0F12, 0x03FF}, //03FF// #AFIT16_Demosaicing_iSatVal
{0x0F12, 0x0144}, //0144// #AFIT16_Sharpening_iLowSharpClamp
{0x0F12, 0x005A}, //005A// #AFIT16_Sharpening_iLowSharpClamp_Bin  
{0x0F12, 0x004B}, //004B// #AFIT16_Sharpening_iHighSharpClamp  
{0x0F12, 0x0040}, //0040// #AFIT16_Sharpening_iHighSharpClamp_Bin 
{0x0F12, 0x0235}, //0235// #AFIT16_Sharpening_iReduceEdgeThresh
{0x0F12, 0x0100}, //0100// #AFIT16_DemSharpMix_iRGBOffset
{0x0F12, 0x01F4}, //01F4// #AFIT16_DemSharpMix_iDemClamp 
{0x0F12, 0x0009}, //0009// #AFIT16_DemSharpMix_iLowThreshold
{0x0F12, 0x0012}, //0012// #AFIT16_DemSharpMix_iHighThreshold  
{0x0F12, 0x0101}, //0101// #AFIT16_DemSharpMix_iLowBright
{0x0F12, 0x0341}, //0341// #AFIT16_DemSharpMix_iHighBright  
{0x0F12, 0x0020}, //0020// #AFIT16_DemSharpMix_iLowSat
{0x0F12, 0x0078}, //0078// #AFIT16_DemSharpMix_iHighSat  
{0x0F12, 0x0070}, //0070// #AFIT16_DemSharpMix_iTune  
{0x0F12, 0x0000}, //0000// #AFIT16_DemSharpMix_iHystThLow
{0x0F12, 0x0000}, //0000// #AFIT16_DemSharpMix_iHystThHigh  
{0x0F12, 0x01CE}, //01CE// #AFIT16_DemSharpMix_iHystCenter  
{0x0F12, 0x0008}, //0008// #AFIT8_DDD_edge_low, AFIT8_DDD_repl_thresh
{0x0F12, 0xF804}, //F804// #AFIT8_DDD_repl_force, AFIT8_DDD_sat_level
{0x0F12, 0x010C}, //010C// #AFIT8_DDD_sat_thr , AFIT8_DDD_sat_mpl 
{0x0F12, 0x2003}, //2003// #AFIT8_DDD_sat_noise , AFIT8_DDD_iClustThresh_H 
{0x0F12, 0x4020}, //4020// #AFIT8_DDD_iClustThresh_H_Bin,  AFIT8_DDD_iClustThresh_C 
{0x0F12, 0x05FF}, //05FF// #AFIT8_DDD_iClustThresh_C_Bin,  AFIT8_DDD_iClustMulT_H
{0x0F12, 0x0305}, //0305// #AFIT8_DDD_iClustMulT_H_Bin  ,  AFIT8_DDD_iClustMulT_C
{0x0F12, 0x0103}, //0103// #AFIT8_DDD_iClustMulT_C_Bin  ,  AFIT8_DDD_nClustLevel_H  
{0x0F12, 0x0001}, //0001// #AFIT8_DDD_nClustLevel_H_Bin ,  AFIT8_DDD_iMaxSlopeAllowed  
{0x0F12, 0x0000}, //0000// #AFIT8_DDD_iHotThreshHigh ,  AFIT8_DDD_iHotThreshLow  
{0x0F12, 0x0007}, //0007// #AFIT8_DDD_iColdThreshHigh,  AFIT8_DDD_iColdThreshLow 
{0x0F12, 0x1923}, //1923// #AFIT8_DDD_DispTH_Low, AFIT8_DDD_DispTH_Low_Bin 
{0x0F12, 0x1923}, //1923// #AFIT8_DDD_DispTH_High ,  AFIT8_DDD_DispTH_High_Bin
{0x0F12, 0x141E}, //141E// #AFIT8_DDD_iDenThreshLow  ,  AFIT8_DDD_iDenThreshLow_Bin 
{0x0F12, 0x141E}, //141E// #AFIT8_DDD_iDenThreshHigh ,  AFIT8_DDD_iDenThreshHigh_Bin
{0x0F12, 0x0000}, //0000// #AFIT8_DDD_AddNoisePower1 ,  AFIT8_DDD_AddNoisePower2 
{0x0F12, 0x00FF}, //00FF// #AFIT8_DDD_iSatSat , AFIT8_DDD_iRadialTune
{0x0F12, 0x0F32}, //0F32// #AFIT8_DDD_iRadialLimit,  AFIT8_DDD_iRadialPower
{0x0F12, 0x2878}, //2878// #AFIT8_DDD_iLowMaxSlopeAllowed  , AFIT8_DDD_iHighMaxSlopeAllowed  
{0x0F12, 0x0006}, //0006// #AFIT8_DDD_iLowSlopeThresh,  AFIT8_DDD_iHighSlopeThresh  
{0x0F12, 0x8107}, //8107// #AFIT8_DDD_iSquaresRounding  ,  AFIT8_Demosaicing_iCentGrad 
{0x0F12, 0x0620}, //0620// #AFIT8_Demosaicing_iMonochrom,  AFIT8_Demosaicing_iDecisionThresh 
{0x0F12, 0x041E}, //041E// #AFIT8_Demosaicing_iDesatThresh ,  AFIT8_Demosaicing_iEnhThresh
{0x0F12, 0x0202}, //0202// #AFIT8_Demosaicing_iGRDenoiseVal,  AFIT8_Demosaicing_iGBDenoiseVal
{0x0F12, 0x0532}, //0532// #AFIT8_Demosaicing_iEdgeDesat,  AFIT8_Demosaicing_iEdgeDesat_Bin  
{0x0F12, 0x0000}, //0000// #AFIT8_Demosaicing_iEdgeDesatThrLow , AFIT8_Demosaicing_iEdgeDesatThrLow_Bin  
{0x0F12, 0x0A00}, //0A00// #AFIT8_Demosaicing_iEdgeDesatThrHigh , AFIT8_Demosaicing_iEdgeDesatThrHigh_Bin
{0x0F12, 0x0404}, //0404// #AFIT8_Demosaicing_iEdgeDesatLimit , AFIT8_Demosaicing_iEdgeDesatLimit_Bin 
{0x0F12, 0x2306}, //2303// #AFIT8_Demosaicing_iNearGrayDesat  , AFIT8_Demosaicing_iDemSharpenLow
{0x0F12, 0x3214}, //3214// #AFIT8_Demosaicing_iDemSharpenLow_Bin , AFIT8_Demosaicing_iDemSharpenHigh  
{0x0F12, 0x1E0A}, //1E0A// #AFIT8_Demosaicing_iDemSharpenHigh_Bin , AFIT8_Demosaicing_iDemSharpThresh 
{0x0F12, 0x0880}, //0880// #AFIT8_Demosaicing_iDemSharpThresh_Bin , AFIT8_Demosaicing_iDemShLowLimit  
{0x0F12, 0x0008}, //0008// #AFIT8_Demosaicing_iDemShLowLimit_Bin , AFIT8_Demosaicing_iDespeckleForDemsharp  
{0x0F12, 0x0A08}, //0A08// #AFIT8_Demosaicing_iDespeckleForDemsharp_Bin , AFIT8_Demosaicing_iDFD_ReduceCoeff
{0x0F12, 0x0005}, //0005// #AFIT8_Demosaicing_iDemBlurLow  ,  AFIT8_Demosaicing_iDemBlurLow_Bin 
{0x0F12, 0x0005}, //0005// #AFIT8_Demosaicing_iDemBlurHigh ,  AFIT8_Demosaicing_iDemBlurHigh_Bin
{0x0F12, 0x0602}, //0602// #AFIT8_Demosaicing_iDemBlurRange,  AFIT8_Demosaicing_iDemBlurRange_Bin  
{0x0F12, 0x1928}, //1928// #AFIT8_Sharpening_iLowSharpPower,  AFIT8_Sharpening_iLowSharpPower_Bin  
{0x0F12, 0x4B28}, //4B28// #AFIT8_Sharpening_iHighSharpPower , AFIT8_Sharpening_iHighSharpPower_Bin
{0x0F12, 0x006F}, //006F// #AFIT8_Sharpening_iMSharpen  ,  AFIT8_Sharpening_iMShThresh 
{0x0F12, 0x0028}, //0028// #AFIT8_Sharpening_iWSharpen  ,  AFIT8_Sharpening_iWShThresh 
{0x0F12, 0x0A00}, //0A00// #AFIT8_Sharpening_nSharpWidth,  AFIT8_Sharpening_iLowShDenoise 
{0x0F12, 0x0005}, //0005// #AFIT8_Sharpening_iLowShDenoise_Bin , AFIT8_Sharpening_iHighShDenoise
{0x0F12, 0x0005}, //0005// #AFIT8_Sharpening_iHighShDenoise_Bin , AFIT8_Sharpening_iReduceNegative 
{0x0F12, 0x13FF}, //13FF// #AFIT8_Sharpening_iShDespeckle  ,  AFIT8_Sharpening_iReduceEdgeMinMult  
{0x0F12, 0x0910}, //0910// #AFIT8_Sharpening_iReduceEdgeMinMult_Bin , AFIT8_Sharpening_iReduceEdgeSlope  
{0x0F12, 0x0004}, //0004// #AFIT8_Sharpening_iReduceEdgeSlope_Bin , AFIT8_DemSharpMix_iRGBMultiplier  
{0x0F12, 0x0F00}, //0F00// #AFIT8_DemSharpMix_iFilterPower,  AFIT8_DemSharpMix_iBCoeff 
{0x0F12, 0x0518}, //0518// #AFIT8_DemSharpMix_iGCoeff  ,  AFIT8_DemSharpMix_iWideMult  
{0x0F12, 0x0900}, //0900// #AFIT8_DemSharpMix_iNarrMult,  AFIT8_DemSharpMix_iWideFiltReduce  
{0x0F12, 0x0903}, //0903// #AFIT8_DemSharpMix_iWideFiltReduce_Bin , AFIT8_DemSharpMix_iNarrFiltReduce 
{0x0F12, 0x0002}, //0002// #AFIT8_DemSharpMix_iNarrFiltReduce_Bin , AFIT8_DemSharpMix_iHystFalloff 
{0x0F12, 0x0800}, //0800// #AFIT8_DemSharpMix_iHystMinMult,  AFIT8_DemSharpMix_iHystWidth 
{0x0F12, 0x0104}, //0104// #AFIT8_DemSharpMix_iHystFallLow,  AFIT8_DemSharpMix_iHystFallHigh 
{0x0F12, 0x6402}, //6402// #AFIT8_DemSharpMix1_iHystTune,  AFIT8_byr_gras_iShadingPower
{0x0F12, 0x0080}, //0080// #AFIT8_RGBGamma2_iLinearity  ,  AFIT8_RGBGamma2_iDarkReduce 
{0x0F12, 0x0080}, //0080// #AFIT8_ccm_oscar_iSaturation ,  AFIT8_RGB2YUV_iYOffset
{0x0F12, 0x0080}, //0080// #AFIT8_RGB2YUV_iRGBGain , AFIT8_RGB2YUV_iSaturation
{0x0F12, 0x0000}, //0000// #AFIT8_DRx_iBlendingSupress
{0x0F12, 0x3E1A}, 
{0x0F12, 0xFFBF}, 
{0x0F12, 0xBE7C}, 
{0x0F12, 0xB5BD}, 
{0x0F12, 0x3C02}, 
{0x0F12, 0x0001}, 
{0x002A, 0x193E}, 
{0x0F12, 0x0001}, // #DRx_bDRxBypass:DRxfunctionoff
};


unsigned short s5k4eagx_init_reg46[][2] = {
{0x0028, 0x7000},
{0x002A, 0x0238},  
{0x0F12, 0x5DC0},	// #REG_TC_IPRM_InClockLSBs  //24MHZ
//{0x0F12, 0x9c40},	// #REG_TC_IPRM_InClockLSBs  //40MHZ

{0x0F12, 0x0000},	  
{0x002A, 0x0252},	  
{0x0F12, 0x0003},	// #REG_TC_IPRM_UseNPviClocks 
{0x0F12, 0x0000},	// #REG_TC_IPRM_UseNMipiClocks
{0x0F12, 0x0000},	// #REG_TC_IPRM_NumberOfMipiLanes
{0x002A, 0x025A},	  
{0x0F12, 0x2EE0},    // #REG_TC_IPRM_sysClocks_0
{0x0F12, 0x2ED0},	// #REG_TC_IPRM_MinOutRate4KHz_0 
{0x0F12, 0x2EF0},	// #REG_TC_IPRM_MaxOutRate4KHz_0                          
{0x0F12, 0x2EE0},	// #REG_TC_IPRM_sysClocks_1                               4
{0x0F12, 0x59d8},	// #REG_TC_IPRM4_MinOutRate4KHz_1 
{0x0F12, 0x59d8},	// #REG_TC_IPRM1_MaxOutRate4KHz_1
{0x0F12, 0x59D8},	// #REG_TC_IPRM_sysClocks_2      
{0x0F12, 0x445C},	// #REG_TC_IPRM_MinOutRate4KHz_2 
{0x0F12, 0x445C},	// #REG_TC_IPRM_MaxOutRate4KHz_2 

{0x002A, 0x026C},	
{0x0F12, 0x0001},	// #REG_TC_IPRM_InitParamsUpdated

{0x002A, 0x1680},	//Jpeg/////////////
{0x0F12, 0x018E},	//jpeg_BrcMaxQuality							0x70001690	// 0x017C
{0x002A, 0x16C6},
{0x0F12, 0x0001},	//jpeg_MinQtblValue
//{0x002A, 0x04BA},
//{0x0F12, 0x0008},	//jpeg_BRC
{0x002A, 0x04BE},
{0x0F12, 50},	//REG_TC_BRC_usCaptureQuality					0x700004C0	// 0x0055

{0x002A, 0x02E0},	//PREVIEW CONFIGURATION 0 (1280x960, YUV, 15fps)
{0x0F12, 0x0500},	// #REG_0TC_PCFG_usWidth
{0x0F12, 0x03C0},	// #REG_0TC_PCFG_usHeight
{0x0F12, 0x0005},	// #REG_0TC_PCFG_Format  
{0x0F12, 0x2EF0},	// #REG_0TC_PCFG_usMaxOut4KHzRate 
{0x0F12, 0x2ED0},	// #REG_0TC_PCFG_usMinOut4KHzRate 
{0x0F12, 0x0100},	// #REG_0TC_PCFG_OutClkPerPix88
{0x0F12, 0x0300},	// #REG_0TC_PCFG_uBpp88  
{0x0F12, 0x0042},	// #REG_0TC_PCFG_PVIMask 
{0x0F12, 0x0000},	// #REG_0TC_PCFG_OIFMask 
{0x0F12, 0x01E0},	// #REG_0TC_PCF_usJpegPacketSize  
{0x0F12, 0x0000},	// #REG_0TC_PCF_usJpegTotalPackets
{0x0F12, 0x0000},	// #REG_0TC_PCF_uClockInd
{0x0F12, 0x0000},	// #REG_0TC_PCF_usFrTimeType
{0x0F12, 0x0001},	// #REG_0TC_PCF_FrRateQualityType 
{0x0F12, 0x029A},	// #REG_0TC_PCFG_usMaxFrTimeMsecMult10 //15fps
{0x0F12, 0x029A},	// #REG_0TC_PCFG_usMinFrTimeMsecMult10 //15 fps  
{0x0F12, 0x0000},	// #REG_0TC_PCFG_sSaturation
{0x0F12, 0x0000},	// #REG_0TC_PCFG_sSharpBlur 
{0x0F12, 0x0000},	// #REG_0TC_PCFG_sGlamour
{0x0F12, 0x0000},	// #REG_0TC_PCFG_sColorTemp 
{0x0F12, 0x0000},	// #REG_0TC_PCFG_uDeviceGammaIndex
{0x0F12, 0x0000},	// #REG_0TC_PCFG_uPrevMirror
{0x0F12, 0x0000},	// #REG_0TC_PCFG_uCaptureMirror
{0x0F12, 0x0000},	// #REG_0TC_PCFG_uRotation  
 

{0x002A, 0x0310},	//PREVIEW CONFIGURATION 1 (2592x1944, YUV, 7.5fps) 
{0x0F12, 0x0A20}, // #REG_0TC_PCFG_usWidth                            
{0x0F12, 0x0798}, // #REG_0TC_PCFG_usHeight                           
{0x0F12, 0x0005}, // #REG_0TC_PCFG_Format                             
{0x0F12, 0x59d8}, // #REG_0TC_PCFG_usMaxOut4KHzRate                   
{0x0F12, 0x59d8}, // #REG_0TC_PCFG_usMinOut4KHzRate                   
{0x0F12, 0x0100}, // #REG_0TC_PCFG_OutClkPerPix88                     
{0x0F12, 0x0300}, // #REG_0TC_PCFG_uBpp88                             
{0x0F12, 0x0042}, // #REG_0TC_PCFG_PVIMask                            
{0x0F12, 0x0000}, // #REG_0TC_PCFG_OIFMask                            
{0x0F12, 0x01E0}, // #REG_0TC_PCF_usJpegPacketSize                    
{0x0F12, 0x0000}, // #REG_0TC_PCF_usJpegTotalPackets                  
{0x0F12, 0x0001}, // #REG_0TC_PCF_uClockInd                           
{0x0F12, 0x0000}, // #REG_0TC_PCF_usFrTimeType                        
{0x0F12, 0x0002}, // #REG_0TC_PCF_FrRateQualityType                   
{0x0F12, 0x0535}, // #REG_0TC_PCFG_usMaxFrTimeMsecMult10 //10fps      
{0x0F12, 0x0535}, // #REG_0TC_PCFG_usMinFrTimeMsecMult10 //20 fps     
{0x0F12, 0x0000}, // #REG_0TC_PCFG_sSaturation                        
{0x0F12, 0x0000}, // #REG_0TC_PCFG_sSharpBlur                         
{0x0F12, 0x0000}, // #REG_0TC_PCFG_sGlamour                           
{0x0F12, 0x0000}, // #REG_0TC_PCFG_sColorTemp                         
{0x0F12, 0x0000}, // #REG_0TC_PCFG_uDeviceGammaIndex                  
{0x0F12, 0x0000}, // #REG_0TC_PCFG_uPrevMirror                        
{0x0F12, 0x0000}, // #REG_0TC_PCFG_uCaptureMirror                     
{0x0F12, 0x0000}, // #REG_0TC_PCFG_uRotation                          
 


//002A, 0310	//PREVIEW CONFIGURATION 1 (1280x960, YUV, 24fps) 
//0F12, 0500	// #REG_1TC_PCFG_usWidth
//0F12, 03C0	// #REG_1TC_PCFG_usHeight
//0F12, 0005	// #REG_1TC_PCFG_Format  
//0F12, 445C	// #REG_1TC_PCFG_usMaxOut4KHzRate 
//0F12, 445C	// #REG_1TC_PCFG_usMinOut4KHzRate 
//0F12, 0100	// #REG_1TC_PCFG_OutClkPerPix88
//0F12, 0300	// #REG_1TC_PCFG_uBpp88  
//0F12, 0042	// #REG_1TC_PCFG_PVIMask 
//0F12, 0000	// #REG_1TC_PCFG_OIFMask 
//0F12, 01E0	// #REG_1TC_PCF_usJpegPacketSize  
//0F12, 0000	// #REG_1TC_PCF_usJpegTotalPackets
//0F12, 0001	// #REG_1TC_PCF_uClockInd
//0F12, 0000	// #REG_1TC_PCF_usFrTimeType
//0F12, 0001	// #REG_1TC_PCF_FrRateQualityType 
//0F12, 01A0	// #REG_1TC_PCFG_usMaxFrTimeMsecMult10 //24fps
//0F12, 01A0	// #REG_1TC_PCFG_usMinFrTimeMsecMult10 //24fps  
//0F12, 0000	// #REG_1TC_PCFG_sSaturation
//0F12, 0000	// #REG_1TC_PCFG_sSharpBlur 
//0F12, 0000	// #REG_1TC_PCFG_sGlamour
//0F12, 0000	// #REG_1TC_PCFG_sColorTemp 
//0F12, 0000	// #REG_1TC_PCFG_uDeviceGammaIndex
//0F12, 0000	// #REG_1TC_PCFG_uPrevMirror
//0F12, 0000	// #REG_1TC_PCFG_uCaptureMirror
//0F12, 0000	// #REG_1TC_PCFG_uRotation  

{0x002A, 0x0340},	//PREVIEW CONFIGURATION 2 (640x480, YUV, 30fps) 
{0x0F12, 800},//0x0280},	// #REG_2TC_PCFG_usWidth
{0x0F12, 600},//0x01E0},	// #REG_2TC_PCFG_usHeight
{0x0F12, 0x0005},	// #REG_2TC_PCFG_Format  
{0x0F12, 0x445C},	// #REG_2TC_PCFG_usMaxOut4KHzRate 
{0x0F12, 0x445C},	// #REG_2TC_PCFG_usMinOut4KHzRate 
{0x0F12, 0x0100},	// #REG_2TC_PCFG_OutClkPerPix88
{0x0F12, 0x0300},	// #REG_2TC_PCFG_uBpp88  
{0x0F12, 0x0042},	// #REG_2TC_PCFG_PVIMask 
{0x0F12, 0x0000},	// #REG_2TC_PCFG_OIFMask 
{0x0F12, 0x01E0},	// #REG_2TC_PCF_usJpegPacketSize  
{0x0F12, 0x0000},	// #REG_2TC_PCF_usJpegTotalPackets
{0x0F12, 0x0002},	// #REG_2TC_PCF_uClockInd
{0x0F12, 0x0000},	// #REG_2TC_PCF_usFrTimeType
{0x0F12, 0x0001},	// #REG_2TC_PCF_FrRateQualityType 
{0x0F12, 0x015E},	// #REG_2TC_PCFG_usMaxFrTimeMsecMult10 //30fps
{0x0F12, 0x015E},	// #REG_2TC_PCFG_usMinFrTimeMsecMult10 //30 fps  
{0x0F12, 0x0000},	// #REG_2TC_PCFG_sSaturation
{0x0F12, 0x0000},	// #REG_2TC_PCFG_sSharpBlur 
{0x0F12, 0x0000},	// #REG_2TC_PCFG_sGlamour
{0x0F12, 0x0000},	// #REG_2TC_PCFG_sColorTemp 
{0x0F12, 0x0000},	// #REG_2TC_PCFG_uDeviceGammaIndex
{0x0F12, 0x0002},	// #REG_2TC_PCFG_uPrevMirror
{0x0F12, 0x0002},	// #REG_2TC_PCFG_uCaptureMirror
{0x0F12, 0x0000},	// #REG_2TC_PCFG_uRotation

{0x002A, 0x1782},	
{0x0F12, 0x0001},	// #senHal_SenBinShifter
{0x002A, 0x053E},	
{0x0F12, 0x0001},	// #REG_HIGH_FPS_UseHighSpeedAng 
{0x002A, 0x1792},	
{0x0F12, 0x05C9},	// #senHal_uHighSpeedMinColsBin
{0x002A, 0x1796},	 
{0x0F12, 0x05C9},	// #senHal_uHighSpeedMinColsNoBin 
{0x002A, 0x179A},	
{0x0F12, 0x0260},	// #senHal_uHighSpeedMinColsAddAnalogBin  
{0x002A, 0x06F0},	
{0x0F12, 0x0003},	// #skl_usConfigStbySettings //for STBY current 
{0x0028, 0xD000},	
{0x002A, 0x109C},	
{0x0F12, 0x0000},	// For PKG Bayer  
 // CAPTURE 0
{0x0028, 0x7000},	
{0x002A, 0x03D0},	
{0x0F12, 0x0001},	// #REG_0TC_CCFG_uCaptureMode 
{0x0F12, 0x0000},	// #REG_0TC_CCFG_bUseMechShut 
{0x0F12, 2592},//1600},//0x0A20},	// #REG_0TC_CCFG_usWidth 
{0x0F12, 1944},//1200},//0x0798},	// #REG_0TC_CCFG_usHeight
{0x0F12, 0x0005},	// #REG_0TC_CCFG_Format 
{0x0F12, 0x59d8},	// #REG_0TC_CCFG_usMaxOut4KHzRate 
{0x0F12, 0x59d8},	// #REG_0TC_CCFG_usMinOut4KHzRate 
{0x0F12, 0x0100},	// #REG_0TC_CCFG_OutClkPerPix88  
{0x0F12, 0x0800},	// #REG_0TC_CCFG_uBpp88 
{0x0F12, 0x0042},	// #REG_0TC_CCFG_PVIMask
{0x0F12, 0x0050},	// #REG_0TC_CCFG_OIFMask
{0x0F12, 0x01E0},	// #REG_0TC_CCFG_usJpegPacketSize   
{0x0F12, 0x0960},	// #REG_0TC_CCFG_usJpegTotalPackets 
{0x0F12, 0x0001},	// #REG_0TC_CCFG_uClockInd 
{0x0F12, 0x0000},	// #REG_0TC_CCFG_usFrTimeType 
{0x0F12, 0x0002},	// #REG_0TC_CCFG_FrRateQualityType  
{0x0F12, 0x0535},	// #REG_0TC_CCFG_usMaxFrTimeMsecMult10  //7.5fps
{0x0F12, 0x0535},	// #REG_0TC_CCFG_usMinFrTimeMsecMult10  //7.5fps
{0x0F12, 0x0000},	// #REG_0TC_CCFG_sSaturation  
{0x0F12, 0x0000},	// #REG_0TC_CCFG_sSharpBlur
{0x0F12, 0x0000},	// #REG_0TC_CCFG_sGlamour  
{0x0F12, 0x0000},	// #REG_0TC_CCFG_sColorTemp
{0x0F12, 0x0000},	// #REG_0TC_CCFG_uDeviceGammaIndex  

{0x002A, 0x02A2},	                            
{0x0F12, 0x0002},	// #REG_TC_GP_ActivePrevConfig 
{0x002A, 0x02A6},	                            
{0x0F12, 0x0001},	// #REG_TC_GP_PrevOpenAfterChange 
{0x002A, 0x028E},	                            
{0x0F12, 0x0001},	// #REG_TC_GP_NewConfigSync 
{0x002A, 0x02A4},	
{0x0F12, 0x0001},	// #REG_TC_GP_PrevConfigChanged
{0x002A, 0x027E},	
{0x0F12, 0x0001},	// #REG_TC_GP_EnablePreview 
{0x0F12, 0x0001},	// #REG_TC_GP_EnablePreviewChanged
{0x002A, 0x02AC},	
{0x0F12, 0x0001},	// #REG_TC_GP_CapConfigChanged
{0x002A, 0x04E0},	
{0x0F12, 0x0A00},	// #REG_TC_PZOOM_CapZoomReqInputWidth  
{0x0F12, 0x0780},	// #REG_TC_PZOOM_CapZoomReqInputHeight 
{0x0F12, 0x0010},	// #REG_TC_PZOOM_CapZoomReqInputWidthOfs  
{0x0F12, 0x000C},	// #REG_TC_PZOOM_CapZoomReqInputHeightOfs 
{0x002A, 0x02A0},	
{0x0F12, 0x0001},	// #REG_TC_GP_InputsChangeRequest  

};



// preview preset
unsigned char s5k4eagx_preview_preset_0[][4] = { 
{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
{0x00, 0x2A, 0x02, 0xA2},	{0x0F, 0x12, 0x00, 0x00},	//REG_TC_GP_ActivePrevConfig  
{0x00, 0x2A, 0x02, 0xA6},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_PrevOpenAfterChange
{0x00, 0x2A, 0x02, 0x8E},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_NewConfigSync
{0x00, 0x2A, 0x02, 0xA4},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_PrevConfigChanged
{0x00, 0x2A, 0x02, 0xAC},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_CapConfigChanged
{0x00, 0x2A, 0x02, 0x7E},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreview
{0x00, 0x2A, 0x02, 0x80},	{0x0F, 0x12, 0x00, 0x01}	//REG_TC_GP_EnablePreviewChanged
};
unsigned char s5k4eagx_preview_preset_1[][4] = { 
{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
{0x00, 0x2A, 0x02, 0xA2},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_ActivePrevConfig  
{0x00, 0x2A, 0x02, 0xA6},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_PrevOpenAfterChange
{0x00, 0x2A, 0x02, 0x8E},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_NewConfigSync
{0x00, 0x2A, 0x02, 0xA4},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_PrevConfigChanged
{0x00, 0x2A, 0x02, 0xAC},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_CapConfigChanged
{0x00, 0x2A, 0x02, 0x7E},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreview
{0x00, 0x2A, 0x02, 0x80},	{0x0F, 0x12, 0x00, 0x01}	//REG_TC_GP_EnablePreviewChanged
};

unsigned char s5k4eagx_preview_preset_2[][4] = { 
{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
//{0x00, 0x2A, 0x02, 0x7E},	{0x0F, 0x12, 0x00, 0x00},	//REG_TC_GP_EnablePreview
//{0x00, 0x2A, 0x02, 0x80},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreviewChanged
//{REG_DELAY, 500, 0, 0}, // p300		

{0x00, 0x2A, 0x03, 0x40},	{0x0F, 0x12, 0x02, 0x80},// #REG_2TC_PCFG_usWidth //640
{0x00, 0x2A, 0x03, 0x42},	{0x0F, 0x12, 0x01, 0xe0},// #REG_2TC_PCFG_usHeight //480


{0x00, 0x2A, 0x02, 0xA2},	{0x0F, 0x12, 0x00, 0x02},	//REG_TC_GP_ActivePrevConfig  
{0x00, 0x2A, 0x02, 0xA6},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_PrevOpenAfterChange
{0x00, 0x2A, 0x02, 0x8E},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_NewConfigSync
{0x00, 0x2A, 0x02, 0xA4},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_PrevConfigChanged
{0x00, 0x2A, 0x02, 0xAC},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_CapConfigChanged

{0x00, 0x2A, 0x02, 0x7E},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreview
{0x00, 0x2A, 0x02, 0x80},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreviewChanged

{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x03},	//REG_TC_AF_AfCmd
{REG_DELAY, 200, 0, 0}, // p300
{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x06},	//5:single AF, 6:continus AF
};

unsigned char s5k4eagx_preview_preset_800x480[][4] = { 
{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
{0x00, 0x2A, 0x02, 0x7E},	{0x0F, 0x12, 0x00, 0x00},	//REG_TC_GP_EnablePreview
{0x00, 0x2A, 0x02, 0x80},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreviewChanged
{REG_DELAY, 500, 0, 0}, // p300		

{0x00, 0x2A, 0x03, 0x40},	{0x0F, 0x12, 0x03, 0x20},// #REG_2TC_PCFG_usWidth //800
{0x00, 0x2A, 0x03, 0x42},	{0x0F, 0x12, 0x01, 0xe0},// #REG_2TC_PCFG_usHeight //480

{0x00, 0x2A, 0x02, 0xA2},	{0x0F, 0x12, 0x00, 0x02},	//REG_TC_GP_ActivePrevConfig  
{0x00, 0x2A, 0x02, 0xA6},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_PrevOpenAfterChange
{0x00, 0x2A, 0x02, 0x8E},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_NewConfigSync
{0x00, 0x2A, 0x02, 0xA4},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_PrevConfigChanged
{0x00, 0x2A, 0x02, 0xAC},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_CapConfigChanged

{0x00, 0x2A, 0x02, 0x7E},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreview
{0x00, 0x2A, 0x02, 0x80},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreviewChanged

{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x03},	//REG_TC_AF_AfCmd
{REG_DELAY, 200, 0, 0}, // p300
{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x06},	//5:single AF, 6:continus AF
};

unsigned char s5k4eagx_preview_preset_2592x1944[][4] = { 
{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
{0x00, 0x2A, 0x02, 0x7E},	{0x0F, 0x12, 0x00, 0x00},	//REG_TC_GP_EnablePreview
{0x00, 0x2A, 0x02, 0x80},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreviewChanged
{REG_DELAY, 500, 0, 0}, // p300		

{0x00, 0x2A, 0x03, 0x40},	{0x0F, 0x12, 0x0a, 0x20},// #REG_2TC_PCFG_usWidth //2592
{0x00, 0x2A, 0x03, 0x42},	{0x0F, 0x12, 0x07, 0x98},// #REG_2TC_PCFG_usHeight //1944

{0x00, 0x2A, 0x02, 0xA2},	{0x0F, 0x12, 0x00, 0x02},	//REG_TC_GP_ActivePrevConfig  
{0x00, 0x2A, 0x02, 0xA6},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_PrevOpenAfterChange
{0x00, 0x2A, 0x02, 0x8E},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_NewConfigSync
{0x00, 0x2A, 0x02, 0xA4},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_PrevConfigChanged
{0x00, 0x2A, 0x02, 0xAC},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_CapConfigChanged

{0x00, 0x2A, 0x02, 0x7E},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreview
{0x00, 0x2A, 0x02, 0x80},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreviewChanged

{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x03},	//REG_TC_AF_AfCmd
{REG_DELAY, 200, 0, 0}, // p300
{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x06},	//5:single AF, 6:continus AF
};

unsigned char s5k4eagx_preview_preset_2048x1536[][4] = { 
{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
{0x00, 0x2A, 0x02, 0x7E},	{0x0F, 0x12, 0x00, 0x00},	//REG_TC_GP_EnablePreview
{0x00, 0x2A, 0x02, 0x80},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreviewChanged
{REG_DELAY, 500, 0, 0}, // p300		

{0x00, 0x2A, 0x03, 0x40},	{0x0F, 0x12, 0x08, 0x00},// #REG_2TC_PCFG_usWidth //2048
{0x00, 0x2A, 0x03, 0x42},	{0x0F, 0x12, 0x06, 0x00},// #REG_2TC_PCFG_usHeight //1536

{0x00, 0x2A, 0x02, 0xA2},	{0x0F, 0x12, 0x00, 0x02},	//REG_TC_GP_ActivePrevConfig  
{0x00, 0x2A, 0x02, 0xA6},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_PrevOpenAfterChange
{0x00, 0x2A, 0x02, 0x8E},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_NewConfigSync
{0x00, 0x2A, 0x02, 0xA4},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_PrevConfigChanged
{0x00, 0x2A, 0x02, 0xAC},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_CapConfigChanged

{0x00, 0x2A, 0x02, 0x7E},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreview
{0x00, 0x2A, 0x02, 0x80},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreviewChanged

{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x03},	//REG_TC_AF_AfCmd
{REG_DELAY, 200, 0, 0}, // p300
{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x06},	//5:single AF, 6:continus AF
};
unsigned char s5k4eagx_preview_preset_1280x720[][4] = { 
{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
{0x00, 0x2A, 0x02, 0x7E},	{0x0F, 0x12, 0x00, 0x00},	//REG_TC_GP_EnablePreview
{0x00, 0x2A, 0x02, 0x80},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreviewChanged
{REG_DELAY, 500, 0, 0}, // p300		

{0x00, 0x2A, 0x03, 0x40},	{0x0F, 0x12, 0x05, 0x00},// #REG_2TC_PCFG_usWidth //1280
{0x00, 0x2A, 0x03, 0x42},	{0x0F, 0x12, 0x02, 0xd0},// #REG_2TC_PCFG_usHeight //720

{0x00, 0x2A, 0x02, 0xA2},	{0x0F, 0x12, 0x00, 0x02},	//REG_TC_GP_ActivePrevConfig  
{0x00, 0x2A, 0x02, 0xA6},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_PrevOpenAfterChange
{0x00, 0x2A, 0x02, 0x8E},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_NewConfigSync
{0x00, 0x2A, 0x02, 0xA4},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_PrevConfigChanged
{0x00, 0x2A, 0x02, 0xAC},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_CapConfigChanged

{0x00, 0x2A, 0x02, 0x7E},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreview
{0x00, 0x2A, 0x02, 0x80},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreviewChanged

{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x03},	//REG_TC_AF_AfCmd
{REG_DELAY, 300, 0, 0}, // p300
{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x06},	//5:single AF, 6:continus AF
};

unsigned char s5k4eagx_preview_preset_1600x1200[][4] = { 
{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
{0x00, 0x2A, 0x02, 0x7E},	{0x0F, 0x12, 0x00, 0x00},	//REG_TC_GP_EnablePreview
{0x00, 0x2A, 0x02, 0x80},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreviewChanged
{REG_DELAY, 500, 0, 0}, // p300		

{0x00, 0x2A, 0x03, 0x40},	{0x0F, 0x12, 0x06, 0x40},// #REG_2TC_PCFG_usWidth //2048
{0x00, 0x2A, 0x03, 0x42},	{0x0F, 0x12, 0x04, 0xb0},// #REG_2TC_PCFG_usHeight //1536

{0x00, 0x2A, 0x02, 0xA2},	{0x0F, 0x12, 0x00, 0x02},	//REG_TC_GP_ActivePrevConfig  
{0x00, 0x2A, 0x02, 0xA6},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_PrevOpenAfterChange
{0x00, 0x2A, 0x02, 0x8E},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_NewConfigSync
{0x00, 0x2A, 0x02, 0xA4},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_PrevConfigChanged
{0x00, 0x2A, 0x02, 0xAC},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_CapConfigChanged

{0x00, 0x2A, 0x02, 0x7E},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreview
{0x00, 0x2A, 0x02, 0x80},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreviewChanged

{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x03},	//REG_TC_AF_AfCmd
{REG_DELAY, 300, 0, 0}, // p300
{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x06},	//5:single AF, 6:continus AF
};


unsigned char s5k4eagx_movie_preset_0[][4] = { 
{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
{0x00, 0x2A, 0x02, 0x7E},	{0x0F, 0x12, 0x00, 0x00},	//REG_TC_GP_EnablePreview
{0x00, 0x2A, 0x02, 0x80},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreviewChanged
{REG_DELAY, 500, 0, 0}, // p300		

{0x00, 0x2A, 0x03, 0x40},	{0x0F, 0x12, 0x02, 0xd0},// #REG_2TC_PCFG_usWidth //720
{0x00, 0x2A, 0x03, 0x42},	{0x0F, 0x12, 0x01, 0xe0},// #REG_2TC_PCFG_usHeight //480

{0x00, 0x2A, 0x02, 0xA2},	{0x0F, 0x12, 0x00, 0x02},	//REG_TC_GP_ActivePrevConfig  
{0x00, 0x2A, 0x02, 0xA6},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_PrevOpenAfterChange
{0x00, 0x2A, 0x02, 0x8E},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_NewConfigSync
{0x00, 0x2A, 0x02, 0xA4},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_PrevConfigChanged
{0x00, 0x2A, 0x02, 0xAC},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_CapConfigChanged

{0x00, 0x2A, 0x02, 0x7E},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreview
{0x00, 0x2A, 0x02, 0x80},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreviewChanged

{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x03},	//REG_TC_AF_AfCmd
{REG_DELAY, 200, 0, 0}, // p300
{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x05},	//5:single AF, 6:continus AF
};


unsigned char s5k4eagx_preview_preset_3[][4] = { 
{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx

//{0x00, 0x2A, 0x03, 0x9A},	{0x0F, 0x12, 0x00, 0x0f},	//REG_3TC_PCFG_uPrevMirror					
//{0x00, 0x2A, 0x03, 0x9C},	{0x0F, 0x12, 0x00, 0x0f},	//REG_3TC_PCFG_uCaptureMirror		

{0x00, 0x2A, 0x02, 0xA2},	{0x0F, 0x12, 0x00, 0x03},	//REG_TC_GP_ActivePrevConfig  
{0x00, 0x2A, 0x02, 0xA6},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_PrevOpenAfterChange
{0x00, 0x2A, 0x02, 0x8E},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_NewConfigSync
{0x00, 0x2A, 0x02, 0xA4},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_PrevConfigChanged
{0x00, 0x2A, 0x02, 0xAC},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_CapConfigChanged
{0x00, 0x2A, 0x02, 0x7E},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreview
{0x00, 0x2A, 0x02, 0x80},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreviewChanged

//{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x03},	//REG_TC_AF_AfCmd
//{REG_DELAY, 500, 0, 0}, // p300
//{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x06},	//5:single AF, 6:continus AF
};

// capture preset
unsigned char s5k4eagx_capture_preset_0[][4] = { 
{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
{0x00, 0x2A, 0x02, 0x7E},	{0x0F, 0x12, 0x00, 0x00},	//REG_TC_GP_EnablePreview
{0x00, 0x2A, 0x02, 0x80},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreviewChanged

{0x00, 0x2A, 0x02, 0xAA},	{0x0F, 0x12, 0x00, 0x00},	//REG_TC_GP_ActiveCaptureConfig  
{0x00, 0x2A, 0x02, 0x82},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnableCapture
{0x00, 0x2A, 0x02, 0x84},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnableCaptureChanged    
{0x00, 0x2A, 0x02, 0xAC},	{0x0F, 0x12, 0x00, 0x01},	//Synchronize FW with new capture configuration
};

unsigned char s5k4eagx_capture_jpeg[][4] = { 
{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
{0x00, 0x2A, 0x02, 0x7E},	{0x0F, 0x12, 0x00, 0x00},	//REG_TC_GP_EnablePreview
{0x00, 0x2A, 0x02, 0x80},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnablePreviewChanged
{0x00, 0x2A, 0x02, 0xAA},	{0x0F, 0x12, 0x00, 0x00},	//REG_TC_GP_ActiveCaptureConfig  
{0x00, 0x2A, 0x02, 0x82},	{0x0F, 0x12, 0x00, 0x01},	//REG_TC_GP_EnableCapture
{0x00, 0x2A, 0x02, 0x84},	{0x0F, 0x12, 0x00, 0x01},	// REG_TC_GP_EnableCaptureChanged    
{0x00, 0x2A, 0x02, 0xAC},	{0x0F, 0x12, 0x00, 0x01},	//Synchronize FW with new capture configuration
};

unsigned char s5k4eagx_capture_preset_1[][4] = { 
};
unsigned char s5k4eagx_capture_preset_2[][4] = { 
};
unsigned char s5k4eagx_capture_preset_3[][4] = { 
};

#define S5K4EAGX_INIT_REGS41	(sizeof(s5k4eagx_init_reg41) / sizeof(s5k4eagx_init_reg41[0]))
#define S5K4EAGX_INIT_REGS42	(sizeof(s5k4eagx_init_reg42) / sizeof(s5k4eagx_init_reg42[0]))
#define S5K4EAGX_INIT_REGS43	(sizeof(s5k4eagx_init_reg43) / sizeof(s5k4eagx_init_reg43[0]))
#define S5K4EAGX_INIT_REGS44	(sizeof(s5k4eagx_init_reg44) / sizeof(s5k4eagx_init_reg44[0]))
#define S5K4EAGX_INIT_REGS45	(sizeof(s5k4eagx_init_reg45) / sizeof(s5k4eagx_init_reg45[0]))
#define S5K4EAGX_INIT_REGS46	(sizeof(s5k4eagx_init_reg46) / sizeof(s5k4eagx_init_reg46[0]))


#define S5K4EAGX_PREVIEW_PRESET_0	(sizeof(s5k4eagx_preview_preset_0) / sizeof(s5k4eagx_preview_preset_0[0]))
#define S5K4EAGX_PREVIEW_PRESET_1	(sizeof(s5k4eagx_preview_preset_1) / sizeof(s5k4eagx_preview_preset_1[0]))
#define S5K4EAGX_PREVIEW_PRESET_2	(sizeof(s5k4eagx_preview_preset_2) / sizeof(s5k4eagx_preview_preset_2[0]))
#define S5K4EAGX_PREVIEW_PRESET_3	(sizeof(s5k4eagx_preview_preset_3) / sizeof(s5k4eagx_preview_preset_3[0]))

#define S5K4EAGX_CAPTURE_PRESET_0	(sizeof(s5k4eagx_capture_preset_0) / sizeof(s5k4eagx_capture_preset_0[0]))
#define S5K4EAGX_CAPTURE_PRESET_1	(sizeof(s5k4eagx_capture_preset_1) / sizeof(s5k4eagx_capture_preset_1[0]))
#define S5K4EAGX_CAPTURE_PRESET_2	(sizeof(s5k4eagx_capture_preset_2) / sizeof(s5k4eagx_capture_preset_2[0]))
#define S5K4EAGX_CAPTURE_PRESET_3	(sizeof(s5k4eagx_capture_preset_3) / sizeof(s5k4eagx_capture_preset_3[0]))
#define S5K4EAGX_PREVIEW_PRESET_800x480	(sizeof(s5k4eagx_preview_preset_800x480) / sizeof(s5k4eagx_preview_preset_800x480[0]))
#define S5K4EAGX_PREVIEW_PRESET_2592x1944	(sizeof(s5k4eagx_preview_preset_2592x1944) / sizeof(s5k4eagx_preview_preset_2592x1944[0]))




#define S5K4EAGX_MOVIE_PRESET_0	(sizeof(s5k4eagx_movie_preset_0) / sizeof(s5k4eagx_movie_preset_0[0]))
#define S5K4EAGX_CAPTURE_JPEG	\
		(sizeof(s5k4eagx_capture_jpeg) / sizeof(s5k4eagx_capture_jpeg[0]))

unsigned short s5k4eagx_sleep_reg[][2] = {
	{0x002A, S5K4EAGX_REG_TC_GP_EnablePreview},
	{0x0F12, 0x0000},
	{0x002A, S5K4EAGX_REG_TC_GP_EnablePreviewChanged},
	{0x0F12, 0x0001},
	{REG_DELAY, 400},
};

#define S5K4EAGX_SLEEP_REGS	\
	(sizeof(s5k4eagx_sleep_reg) / sizeof(s5k4eagx_sleep_reg[0]))

unsigned short s5k4eagx_wakeup_reg[][2] = {
	{0x002A, 0x02F0},
	{0x0F12, 0x0052},
	{0x002A, S5K4EAGX_REG_TC_GP_PrevConfigChanged},
	{0x0F12, 0x0001},
};

#define S5K4EAGX_WAKEUP_REGS	\
	(sizeof(s5k4eagx_wakeup_reg) / sizeof(s5k4eagx_wakeup_reg[0]))

/*
 * EV bias
 */

static const struct s5k4eagx_reg s5k4eagx_ev_m6[] = {
};

static const struct s5k4eagx_reg s5k4eagx_ev_m5[] = {
};

static const struct s5k4eagx_reg s5k4eagx_ev_m4[] = {
};

static const struct s5k4eagx_reg s5k4eagx_ev_m3[] = {
};

static const struct s5k4eagx_reg s5k4eagx_ev_m2[] = {
};

static const struct s5k4eagx_reg s5k4eagx_ev_m1[] = {
};

static const struct s5k4eagx_reg s5k4eagx_ev_default[] = {
};

static const struct s5k4eagx_reg s5k4eagx_ev_p1[] = {
};

static const struct s5k4eagx_reg s5k4eagx_ev_p2[] = {
};

static const struct s5k4eagx_reg s5k4eagx_ev_p3[] = {
};

static const struct s5k4eagx_reg s5k4eagx_ev_p4[] = {
};

static const struct s5k4eagx_reg s5k4eagx_ev_p5[] = {
};

static const struct s5k4eagx_reg s5k4eagx_ev_p6[] = {
};

#ifdef S5K4EAGX_COMPLETE
/* Order of this array should be following the querymenu data */
static const unsigned char *s5k4eagx_regs_ev_bias[] = {
	(unsigned char *)s5k4eagx_ev_m6, (unsigned char *)s5k4eagx_ev_m5,
	(unsigned char *)s5k4eagx_ev_m4, (unsigned char *)s5k4eagx_ev_m3,
	(unsigned char *)s5k4eagx_ev_m2, (unsigned char *)s5k4eagx_ev_m1,
	(unsigned char *)s5k4eagx_ev_default, (unsigned char *)s5k4eagx_ev_p1,
	(unsigned char *)s5k4eagx_ev_p2, (unsigned char *)s5k4eagx_ev_p3,
	(unsigned char *)s5k4eagx_ev_p4, (unsigned char *)s5k4eagx_ev_p5,
	(unsigned char *)s5k4eagx_ev_p6,
};

/*
 * Auto White Balance configure
 */
static const struct s5k4eagx_reg s5k4eagx_awb_off[] = {
};

static const struct s5k4eagx_reg s5k4eagx_awb_on[] = {
};

static const unsigned char *s5k4eagx_regs_awb_enable[] = {
	(unsigned char *)s5k4eagx_awb_off,
	(unsigned char *)s5k4eagx_awb_on,
};

/*
 * Manual White Balance (presets)
 */
static const struct s5k4eagx_reg s5k4eagx_wb_tungsten[] = {

};

static const struct s5k4eagx_reg s5k4eagx_wb_fluorescent[] = {

};

static const struct s5k4eagx_reg s5k4eagx_wb_sunny[] = {

};

static const struct s5k4eagx_reg s5k4eagx_wb_cloudy[] = {

};

/* Order of this array should be following the querymenu data */
static const unsigned char *s5k4eagx_regs_wb_preset[] = {
	(unsigned char *)s5k4eagx_wb_tungsten,
	(unsigned char *)s5k4eagx_wb_fluorescent,
	(unsigned char *)s5k4eagx_wb_sunny,
	(unsigned char *)s5k4eagx_wb_cloudy,
};

/*
 * Color Effect (COLORFX)
 */
 unsigned char s5k4eagx_color_none[][4] = {
	{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
	{0x00, 0x2A, 0x02, 0x7c},	{0x0F, 0x12, 0x00, 0x00}
};
unsigned char s5k4eagx_color_sepia[][4] = {
	{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
	{0x00, 0x2A, 0x02, 0x7c},	{0x0F, 0x12, 0x00, 0x04}
};

unsigned char s5k4eagx_color_aqua[][4] = {
	{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
	{0x00, 0x2A, 0x02, 0x7c},	{0x0F, 0x12, 0x00, 0x05}
};

unsigned char s5k4eagx_color_monochrome[][4] = {
	{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
	{0x00, 0x2A, 0x02, 0x7c},	{0x0F, 0x12, 0x00, 0x01}
};

unsigned char s5k4eagx_color_negative[][4] = {
	{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
	{0x00, 0x2A, 0x02, 0x7c},	{0x0F, 0x12, 0x00, 0x03}
};

unsigned char s5k4eagx_color_sketch[][4] = {
	{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
	{0x00, 0x2A, 0x02, 0x7c},	{0x0F, 0x12, 0x00, 0x09}
};

#define S5K4EAGX_COLOR	(sizeof(s5k4eagx_color_sepia) / sizeof(s5k4eagx_color_sepia[0]))


/* Order of this array should be following the querymenu data */
static const unsigned char *s5k4eagx_regs_color_effect[] = {
	(unsigned char *)s5k4eagx_color_none,
	(unsigned char *)s5k4eagx_color_sepia,
	(unsigned char *)s5k4eagx_color_aqua,
	(unsigned char *)s5k4eagx_color_monochrome,
	(unsigned char *)s5k4eagx_color_negative,
	(unsigned char *)s5k4eagx_color_sketch,
};

/* Zoom */
unsigned char s5k4eagx_zoom_idle[][4] = {

	{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
	{0x00, 0x2A, 0x04, 0xe8},	{0x0F, 0x12, 0x00, 0x00},	
};
unsigned char s5k4eagx_zoom_stepping_up[][4] = {
	{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
	{0x00, 0x2A, 0x04, 0xe8},	{0x0F, 0x12, 0x00, 0x01},	
};
unsigned char s5k4eagx_zoom_stepping_down[][4] = {
	{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
	{0x00, 0x2A, 0x04, 0xe8},	{0x0F, 0x12, 0x00, 0x02},	
};
unsigned char s5k4eagx_zoom_one_step_up[][4] = {
	{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
	{0x00, 0x2A, 0x04, 0xe8},	{0x0F, 0x12, 0x00, 0x03},	
};
unsigned char s5k4eagx_zoom_one_step_down[][4] = {
	{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
	{0x00, 0x2A, 0x04, 0xe8},	{0x0F, 0x12, 0x00, 0x04},	
};
unsigned char s5k4eagx_zoom_go_target[][4] = {
	{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
	{0x00, 0x2A, 0x04, 0xe8},	{0x0F, 0x12, 0x00, 0x05},	
};
unsigned char s5k4eagx_zoom_forece_out_crop[][4] = {
	{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
	{0x00, 0x2A, 0x04, 0xe8},	{0x0F, 0x12, 0x00, 0x06},	
};

#define S5K4EAGX_ZOOM	(sizeof(s5k4eagx_zoom_idle) / sizeof(s5k4eagx_zoom_idle[0]))

static const unsigned char *s5k4eagx_regs_zoom[] = {
	(unsigned char *)s5k4eagx_zoom_idle,
	(unsigned char *)s5k4eagx_zoom_stepping_up,
	(unsigned char *)s5k4eagx_zoom_stepping_down,
	(unsigned char *)s5k4eagx_zoom_one_step_up,
	(unsigned char *)s5k4eagx_zoom_one_step_down,
	(unsigned char *)s5k4eagx_zoom_go_target,
	(unsigned char *)s5k4eagx_zoom_forece_out_crop,
};

/* auto focus */
unsigned char s5k4eagx_focus_auto[][4] = {
	{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
//	{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x03},	//REG_TC_AF_AfCmd
//	{REG_DELAY, 500, 0, 0}, // p300
	{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x06},	//5:single AF, 6:continus AF
};
unsigned char s5k4eagx_zoom_fixed[][4] = {
	{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
//	{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x03},	//REG_TC_AF_AfCmd
//	{REG_DELAY, 500, 0, 0}, // p300
	{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x05},	//5:single AF, 6:continus AF

};
unsigned char s5k4eagx_zoom_infinity[][4] = {
	{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
//	{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x03},	//REG_TC_AF_AfCmd
//	{REG_DELAY, 500, 0, 0}, // p300
	{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x05},	//5:single AF, 6:continus AF

};
unsigned char s5k4eagx_zoom_macro[][4] = {
	{0x00, 0x2c, 0x70, 0x00}, //offset setting 0x7000 xxxx
//	{0x00, 0x2A, 0x02, 0xBC},	{0x0F, 0x12, 0x00, 0x03},	//REG_TC_AF_AfCmd
//	{REG_DELAY, 500, 0, 0}, // p300
	{0x00, 0x2A, 0x02, 0x8a},	{0x0F, 0x12, 0x00, 0x01},	// 1 MACRO
	
};

#define S5K4EAGX_FOCUS	(sizeof(s5k4eagx_focus_auto) / sizeof(s5k4eagx_focus_auto[0]))

static const unsigned char *s5k4eagx_regs_focus[] = {
	(unsigned char *)s5k4eagx_focus_auto,
	(unsigned char *)s5k4eagx_zoom_fixed,
	(unsigned char *)s5k4eagx_zoom_infinity,
	(unsigned char *)s5k4eagx_zoom_macro,
};


/*
 * Contrast bias
 */
static const struct s5k4eagx_reg s5k4eagx_contrast_m2[] = {
};

static const struct s5k4eagx_reg s5k4eagx_contrast_m1[] = {
};

static const struct s5k4eagx_reg s5k4eagx_contrast_default[] = {
};

static const struct s5k4eagx_reg s5k4eagx_contrast_p1[] = {
};

static const struct s5k4eagx_reg s5k4eagx_contrast_p2[] = {
};

static const unsigned char *s5k4eagx_regs_contrast_bias[] = {
	(unsigned char *)s5k4eagx_contrast_m2,
	(unsigned char *)s5k4eagx_contrast_m1,
	(unsigned char *)s5k4eagx_contrast_default,
	(unsigned char *)s5k4eagx_contrast_p1,
	(unsigned char *)s5k4eagx_contrast_p2,
};

/*
 * Saturation bias
 */
static const struct s5k4eagx_reg s5k4eagx_saturation_m2[] = {
};

static const struct s5k4eagx_reg s5k4eagx_saturation_m1[] = {
};

static const struct s5k4eagx_reg s5k4eagx_saturation_default[] = {
};

static const struct s5k4eagx_reg s5k4eagx_saturation_p1[] = {
};

static const struct s5k4eagx_reg s5k4eagx_saturation_p2[] = {
};

static const unsigned char *s5k4eagx_regs_saturation_bias[] = {
	(unsigned char *)s5k4eagx_saturation_m2,
	(unsigned char *)s5k4eagx_saturation_m1,
	(unsigned char *)s5k4eagx_saturation_default,
	(unsigned char *)s5k4eagx_saturation_p1,
	(unsigned char *)s5k4eagx_saturation_p2,
};

/*
 * Sharpness bias
 */
static const struct s5k4eagx_reg s5k4eagx_sharpness_m2[] = {
};

static const struct s5k4eagx_reg s5k4eagx_sharpness_m1[] = {
};

static const struct s5k4eagx_reg s5k4eagx_sharpness_default[] = {
};

static const struct s5k4eagx_reg s5k4eagx_sharpness_p1[] = {
};

static const struct s5k4eagx_reg s5k4eagx_sharpness_p2[] = {
};

static const unsigned char *s5k4eagx_regs_sharpness_bias[] = {
	(unsigned char *)s5k4eagx_sharpness_m2,
	(unsigned char *)s5k4eagx_sharpness_m1,
	(unsigned char *)s5k4eagx_sharpness_default,
	(unsigned char *)s5k4eagx_sharpness_p1,
	(unsigned char *)s5k4eagx_sharpness_p2,
};

int __s5k4eagx_init_4bytes(struct v4l2_subdev *sd, unsigned char *reg[], int total);
static int __s5k4eagx_init_2bytes(struct v4l2_subdev *sd, unsigned short *reg[], int total);
static int _error_check(struct i2c_client *client);
static unsigned short i2c_read_reg(struct i2c_client *client, unsigned short reg_h, unsigned short reg);

#endif /* S5K4EAGX_COMPLETE */

#endif
