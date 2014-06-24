package com.example.androidex;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;



public class FigureActivity extends Activity{

	LinearLayout linear;
	OnClickListener go_listener, main_listener, clear_listener;
	EditText input_text;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_figure);
		linear = (LinearLayout)findViewById(R.id.container);
		
		// Load C library
		System.loadLibrary("dangercloz_module");
		
		// Initialize objects from view found by ID
		Button btn_go = (Button)findViewById(R.id.btn_go);
		Button btn_main = (Button)findViewById(R.id.btn_main);
		Button btn_clear = (Button)findViewById(R.id.btn_clear);
		input_text = (EditText)findViewById(R.id.figure_input);
		
		// Initialize listeners for each buttons
		go_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				String text;
				String[] columns = null;
				
				// Extract input from edit text
				text = input.getText().toString();
				
				// Continue if user typed input
				if(text.length() != 0){
					columns = text.split(" ");	// split inputs
					
					// Continue only if 3 inputs are in the string
					if(columns.length == 3){
						int time, num;
						char option[4];
						
						// Check if inputs are numbers
						Pattern ps = Pattern.compile("^[0-9]+$");
						if(ps.matcher(columns[0]).matches())
							time = Integer.parseInt(columns[0]);
						if(ps.matcher(columns[1]).matches())
							num = Integer.parseInt(columns[1]);
						//if(ps.matcher(columns[2]).matches())
						// copy string to option
						
						// TODO : check if another thread is running
						// if yes, stop the thread, and start this one with new time, value, option
						// if no, just start this one with given parameters
					}
				}
			}
		};
		
		main_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				// TODO : stop updating devices
				onBackPressed();
			}
		};
		
		clear_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
			}
		};
		
		// Add button listener to the corresponding button
		btn_go.setOnClickListener(go_listener);
		btn_main.setOnClickListener(main_listener);
		btn_clear.setOnClickListener(clear_listener);
	}

}
