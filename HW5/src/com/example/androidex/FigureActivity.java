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
	EditText input;
	
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
		input = (EditText)findViewById(R.id.figure_input);
		
		// Initialize listeners for each buttons
		go_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				String text;
				String[] columns = null;
				
				// Extract input from edit text
				/*
				 * String text;
				String[] columns = null;

				// Extract input from edit text
				text = input_text.getText().toString();

				// Continue if user typed input
				if (text.length() != 0) {
					columns = text.split(" "); // split two inputs

					// Continue only if 2 inputs are in the string
					if (columns.length == 2) {
						int row = 0, col = 0;

						// Check if both inputs are numbers between 1~5
						Pattern ps = Pattern.compile("^[1-5]+$");
						if (ps.matcher(columns[0]).matches())
							row = Integer.parseInt(columns[0]);
						if (ps.matcher(columns[1]).matches())
							col = Integer.parseInt(columns[1]);

						// Create puzzle if both inputs are available
						if (row != 0 && col != 0) {
							if (row == 1 && col == 1) {
							} // 1x1 puzzle is impossible
							else {
								// Check if buttons have created or not
								if (dynamic_array != null) {
								} // Nothing happens
								else {
									// Create buttons if this is first time
									dynamic_array = new Button[row * col];
									create_buttons(row, col);
									mix_puzzle(row, col);
									PuzzleCount();
								}
							}
						}
					}
				}
			}
		};
		btn_start.setOnClickListener(create_listener);
				 */
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
