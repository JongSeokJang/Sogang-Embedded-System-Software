package com.example.androidex;

import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;



public class FigureActivity extends Activity{

	LinearLayout linear;
	OnClickListener go_listener, main_listener, clear_listener;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_figure);
		linear = (LinearLayout)findViewById(R.id.container);
		
		// Initialize buttons from view found by ID
		Button btn_go = (Button)findViewById(R.id.btn_go);
		Button btn_main = (Button)findViewById(R.id.btn_main);
		Button btn_clear = (Button)findViewById(R.id.btn_clear);
		
		// Initialize listeners for each buttons
		go_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
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
