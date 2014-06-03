package com.example.androidex;

import android.app.Activity;
import android.content.Intent;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;



public class MainActivity extends Activity{
	LinearLayout linear;
	Button btn_music, btn_proc, btn_puzzle;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		
		linear = (LinearLayout)findViewById(R.id.container);
		btn_music = (Button)findViewById(R.id.music_button);
		btn_proc = (Button)findViewById(R.id.proc_button);
		btn_puzzle = (Button)findViewById(R.id.puzzle_button);
		
		OnClickListener music_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				Intent intent = new Intent(MainActivity.this, MusicActivity.class);
				startActivity(intent);
			}
		};
		btn_music.setOnClickListener(music_listener);
		
		OnClickListener proc_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				Intent intent = new Intent(MainActivity.this, ProcActivity.class);
				startActivity(intent);
			}
		};
		btn_proc.setOnClickListener(proc_listener);
		
		OnClickListener puzzle_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				Intent intent = new Intent(MainActivity.this, PuzzleActivity.class);
				startActivity(intent);
			}
		};
		btn_puzzle.setOnClickListener(puzzle_listener);
	}

}
