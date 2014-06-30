package com.example.androidex;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

public class ModeActivity extends Activity{

	public native String btnSwitch();
	public native void printNumber(int count);
	
	LinearLayout linear;
	switchThread switch_thread;
	Button btn_1, btn_2, btn_3, btn_4, btn_5, btn_6, btn_7, btn_8, btn_9;
	Button btn_modify, btn_main;
	boolean[] btn_flag = new boolean[9];
	Button[] btn_array = new Button[9];
	OnClickListener btn_listener, modify_listener, main_listener;
	TextView text_num;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_mode);
		linear = (LinearLayout)findViewById(R.id.container);
		
		// Load C Library
		System.loadLibrary("dangercloz_module");
		
		// Initialize objects find by ID
		btn_1 = (Button)findViewById(R.id.btn_1);
		btn_2 = (Button)findViewById(R.id.btn_2);
		btn_3 = (Button)findViewById(R.id.btn_3);
		btn_4 = (Button)findViewById(R.id.btn_4);
		btn_5 = (Button)findViewById(R.id.btn_5);
		btn_6 = (Button)findViewById(R.id.btn_6);
		btn_7 = (Button)findViewById(R.id.btn_7);
		btn_8 = (Button)findViewById(R.id.btn_8);
		btn_9 = (Button)findViewById(R.id.btn_9);
		btn_modify = (Button)findViewById(R.id.btn_modify);
		btn_main = (Button)findViewById(R.id.btn_main);
		text_num = (TextView)findViewById(R.id.text_num);
		
		btn_array[0] = btn_1;	btn_array[1] = btn_2;	btn_array[2] = btn_3;
		btn_array[3] = btn_4;	btn_array[4] = btn_5;	btn_array[5] = btn_6;
		btn_array[6] = btn_7;	btn_array[7] = btn_8;	btn_array[8] = btn_9;
		
		// Create listener event
		btn_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				Button temp = (Button)v;
				int num = Integer.parseInt(temp.getText().toString()) - 1;
				
				if(btn_flag[num] == true)
					btn_flag[num] = false;
				else
					btn_flag[num] = true;
				
				runOnUiThread(new Runnable() {
					@Override
					public void run() {
						for(int i=0;i<9;i++){
							if(btn_flag[i] == true)
								btn_array[i].setBackgroundResource(android.R.drawable.btn_default);
							else
								btn_array[i].setBackgroundColor(Color.BLACK);
						}
					}
				});
			}
		};
		
		modify_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				int count = 0;
				for(int i=0;i<9;i++){
					if(btn_flag[i] == false)
						count++;
				}
				
				text_num.setText(String.valueOf(count));
				printNumber(count);
			}
		};
		
		main_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				switch_thread.flag = false;
				printNumber(0);
				onBackPressed();
			}
		};
		
		// Register listener on button
		btn_modify.setOnClickListener(modify_listener);
		btn_main.setOnClickListener(main_listener);
		for(int i=0;i<9;i++)
			btn_array[i].setOnClickListener(btn_listener);
		
		for(int i=0;i<9;i++)
			btn_flag[i] = true;
		
		switch_thread = new switchThread();
		switch_thread.setDaemon(true);
		switch_thread.start();
	}
	
	class switchThread extends Thread{
		boolean flag, zeroes;
		String temp;
		char[] temp2;
		char[] switch_input = new char[9];
		
		public void run(){
			flag = true;
			
			while(true){
				zeroes = true;
				
				temp = btnSwitch();
				temp2 = temp.toCharArray();
				
				// Find if there are inputs
				for(int i=0;i<9;i++){
					if(temp2[i] != '0'){
						switch_input[i] = temp2[i];
						zeroes = false;
					}
				}
				
				// If input is finished
				if(zeroes){
					int count = 0;
					for(int i=0;i<9;i++){
						if(switch_input[i] == '1')
							count++;
					}
					
					// Only one input is allowed
					if(count == 1){
						int i;
						for (i = 0; i < 9; i++)
							if (switch_input[i] == '1')
								break;
						
						if(btn_flag[i] == true)
							btn_flag[i] = false;
						else
							btn_flag[i] = true;
						
						runOnUiThread(new Runnable() {
							@Override
							public void run() {
								for(int i=0;i<9;i++){
									if(btn_flag[i] == true)
										btn_array[i].setBackgroundResource(android.R.drawable.btn_default);
									else
										btn_array[i].setBackgroundColor(Color.BLACK);
								}
							}
						});
					}
					
					// set to default value
					for(int i=0;i<9;i++)
						switch_input[i] = 0;
				}
				
				try{
					Thread.sleep(10);
				} catch(InterruptedException e){}
			}
		}
	}
	
	// If physical back button pressed, clear devices
		public boolean onKeyDown(int keyCode, android.view.KeyEvent event){
			switch_thread.flag = false;
			printNumber(0);
			
			return super.onKeyDown(keyCode, event);
		}
}
