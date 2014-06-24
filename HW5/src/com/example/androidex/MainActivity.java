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
	Button btn_text, btn_figure, btn_watch, btn_puzzle, btn_mode, btn_quit;
	OnClickListener text_listener, figure_listener, watch_listener;
	OnClickListener puzzle_listener, mode_listener, quit_listener;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		linear = (LinearLayout)findViewById(R.id.container);
		
		// Initialize buttons from view found by ID
		btn_text = (Button)findViewById(R.id.btn_texteditor);
		btn_figure = (Button)findViewById(R.id.btn_figureswitch);
		btn_watch = (Button)findViewById(R.id.btn_watch);
		btn_puzzle = (Button)findViewById(R.id.btn_puzzle);
		btn_mode = (Button)findViewById(R.id.btn_mymode);
		btn_quit = (Button)findViewById(R.id.btn_quit);
		
		// Initialize listeners for each buttons
		text_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				Intent intent = new Intent(MainActivity.this, TextActivity.class);
				startActivity(intent);
			}
		};
		
		figure_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				Intent intent = new Intent(MainActivity.this, FigureActivity.class);
				startActivity(intent);
			}
		};
		
		watch_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				Intent intent = new Intent(MainActivity.this, WatchActivity.class);
				startActivity(intent);
			}
		};
		
		puzzle_listener = new OnClickListener(){
			@Override
			public void onClick(View v){				
				Intent intent = new Intent(MainActivity.this, PuzzleActivity.class);
				startActivity(intent);
			}
		};
		
		mode_listener = new OnClickListener(){
			@Override
			public void onClick(View v){				
				Intent intent = new Intent(MainActivity.this, ModeActivity.class);
				startActivity(intent);
			}
		};
		
		quit_listener = new OnClickListener(){
			@Override
			public void onClick(View v){				
				finish();
				moveTaskToBack(true);
			}
		};
		
		// Add button listener to the corresponding button
		btn_text.setOnClickListener(text_listener);
		btn_figure.setOnClickListener(figure_listener);
		btn_watch.setOnClickListener(watch_listener);
		btn_puzzle.setOnClickListener(puzzle_listener);
		btn_mode.setOnClickListener(mode_listener);
		btn_quit.setOnClickListener(quit_listener);
	}

}
