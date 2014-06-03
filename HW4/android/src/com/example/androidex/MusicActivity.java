package com.example.androidex;

import android.app.Activity;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;

public class MusicActivity extends Activity{
	
	MediaPlayer mp1;
	Button btn_start, btn_pause, btn_stop;
	LinearLayout linear;
	OnClickListener start_listener, pause_listener, stop_listener;
	
	protected void onCreate(Bundle savedInstanceState){
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_music);
		linear = (LinearLayout)findViewById(R.id.container);
		
		btn_start = (Button)findViewById(R.id.btn_start);
		btn_pause = (Button)findViewById(R.id.btn_pause);
		btn_stop = (Button)findViewById(R.id.btn_stop);
		
		mp1 = MediaPlayer.create(this, R.raw.hello);
		
		start_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				mp1.start();
			}
		};
		btn_start.setOnClickListener(start_listener);
		
		pause_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				if(mp1.isPlaying()){
					mp1.pause();
				}
			}
		};
		btn_pause.setOnClickListener(pause_listener);
		
		stop_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				mp1.stop();
				onBackPressed();
			}
		};
		btn_stop.setOnClickListener(stop_listener);
	}
	
	public boolean onKeyDown(int keyCode, android.view.KeyEvent event){
		mp1.stop();
		
		return super.onKeyDown(keyCode, event);
	}
}