package com.example.androidex;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;



public class TextActivity extends Activity{

	public native void TextEditor(String string);
	public native String PushSwitch();
	
	LinearLayout linear;
	Button btn_modify, btn_clear, btn_main, btn_usage;
	EditText text, usage_text, length_text;
	OnClickListener modify_listener, clear_listener, main_listener, usage_listener;
	getSwitch thread;
	String insert_text;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_text);
		linear = (LinearLayout)findViewById(R.id.container);
		
		// Load C library
		System.loadLibrary("dangercloz_module");
		
		thread = new getSwitch();
		thread.setDaemon(true);
		thread.start();
		
		// Initialize objects from view found by ID
		text = (EditText)findViewById(R.id.text_edit);
		usage_text = (EditText)findViewById(R.id.text_usage);
		length_text = (EditText)findViewById(R.id.text_length);
		btn_modify = (Button)findViewById(R.id.btn_modify);
		btn_clear = (Button)findViewById(R.id.btn_clear);
		btn_main = (Button)findViewById(R.id.btn_main);
		btn_usage = (Button)findViewById(R.id.btn_usage);
		
		// Initialize listeners
		modify_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				length_text.setText("Text Length : " + String.valueOf(text.length()));
				TextEditor(text.getText().toString());
			}
		};
		
		clear_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				// Clear both application and board
				length_text.setText("Text Length : ");
				usage_text.setText("CPU Usage : ");
				text.setText("");
				
				TextEditor(text.getText().toString());
			}
		};
		
		main_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				// Clear board HW and back to Main Activity
				TextEditor("");
				thread.flag = false;
				onBackPressed();
			}
		};
		
		usage_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				Message msg = Message.obtain();
				msg.what = 0;
				mHandler.sendMessage(msg);
			}
		};
		
		// Add button listener to the corresponding button
		btn_modify.setOnClickListener(modify_listener);
		btn_clear.setOnClickListener(clear_listener);
		btn_main.setOnClickListener(main_listener);
		btn_usage.setOnClickListener(usage_listener);
	}
	
	// Get CPU Usage
	@SuppressLint("HandlerLeak")
	Handler mHandler = new Handler(){
		public void handleMessage(Message msg){
			if(msg.what == 0){
				try{
					FileReader fis = new FileReader("/proc/stat");
					BufferedReader br = new BufferedReader(fis);
					
					String str = null;
					String[] columns = null;
					
					// Read /proc/stat then split strings into tokens
					str = br.readLine();
					columns = str.split(" ");
					
					// Calculate CPU Usage
					int total_jiffies = 0;
					int usage;
					
					total_jiffies += Integer.parseInt(columns[2]);
					total_jiffies += Integer.parseInt(columns[3]);
					total_jiffies += Integer.parseInt(columns[4]);
					total_jiffies += Integer.parseInt(columns[5]);
					usage = Integer.parseInt(columns[5])*100 / total_jiffies;
					usage = 100 - usage;
					
					// Modify Text
					usage_text.setText("CPU Usage : " + String.valueOf(usage) + "%");
					
					// Close file and buffer
					fis.close();
					br.close();
				} catch (IOException e){}
			}
		}
	};
	
	class getSwitch extends Thread{
		boolean flag = true, zeroes;
		boolean modified = false;
		String temp;
		int num_flag = 1;
		char[] input = new char[9];
		char[] temp2;
		char[][] char_set = {{'.', 'q', 'z'},
												 {'a', 'b', 'c'},
												 {'d', 'e', 'f'},
												 {'g', 'h', 'i'},
												 {'j', 'k', 'l'},
												 {'m', 'n', 'o'},
												 {'p', 'r', 's'},
												 {'t', 'u', 'v'},
												 {'w', 'x', 'y'}};
		
		public void run(){
			while(flag){
				zeroes = true;
				
				temp = PushSwitch();
				temp2 = temp.toCharArray();
				
				// Find if there are inputs
				for(int i=0;i<9;i++){
					if(temp2[i] != '0'){
						input[i] = temp2[i];
						zeroes = false;
					}
				}
				
				if(zeroes){
					int count = 0;
					for(int i=0;i<9;i++){
						if(input[i] == '1')
							count++;
					}
					
					if(count == 2){	// if there are two inputs
						int first = -1, second = -1;
						for(int i=0;i<9;i++){
							if(input[i] == '1'){
								if(first == -1)
									first = i;
								else
									second = i;
							}
						}
						
						if(first == 1 && second == 2){
							// (2) & (3) switch
							TextEditor("");
							thread.flag = false;
							onBackPressed();
						} else if(first == 3 && second == 4){
							// (4) & (5) switch
							insert_text = "";
							TextEditor(insert_text);
							
							// print on board text
							runOnUiThread(new Runnable() {
								@Override
								public void run() {
									text.setText("");
									modified = true;
								}
							});
						} else if(first == 4 && second == 5){
							// (5) & (6) switch
							if(num_flag == 1)
								num_flag = 2;
							else
								num_flag = 1;
						}
					} else if(count == 1){	// if there is only one input
						if (num_flag == 1) {
							insert_text = text.getText().toString();
							int text_length = insert_text.length();

							if (text_length == 0) { // If this is first time to
													// write
								int i;
								for (i = 0; i < 9; i++)
									if (input[i] == '1')
										break;

								switch (i) {
								case 0:
									insert_text += ".";
									break;
								case 1:
									insert_text += "a";
									break;
								case 2:
									insert_text += "d";
									break;
								case 3:
									insert_text += "g";
									break;
								case 4:
									insert_text += "j";
									break;
								case 5:
									insert_text += "m";
									break;
								case 6:
									insert_text += "p";
									break;
								case 7:
									insert_text += "t";
									break;
								case 8:
									insert_text += "w";
									break;
								}
							} else { // If there are already string
								int i, j;
								boolean same = false;
								char[] temp3 = insert_text.toCharArray();
								char chr = temp3[text_length - 1];

								// check for input switch
								for (i = 0; i < 9; i++)
									if (input[i] == '1')
										break;

								// check for char set
								for (j = 0; j < 3; j++) {
									if (temp3[text_length - 1] == char_set[i][j]) {
										same = true;
										break;
									}
								}

								// modify to new char if same switch
								if (same) {
									if (j == 2)
										j = 0;
									else
										j++;

									temp3[text_length - 1] = char_set[i][j];
									insert_text = String.copyValueOf(temp3);
								} else {
									insert_text += char_set[i][0];
								}
							}

							// print on board text
							runOnUiThread(new Runnable() {
								@Override
								public void run() {
									text.setText(insert_text);
									modified = true;
								}
							});
						} else if(num_flag == 2){
							int i;
							for(i=0;i<9;i++)
								if(input[i] == '1')
									break;
							
							insert_text += String.valueOf(i+1);
							
							// print on board text
							runOnUiThread(new Runnable() {
								@Override
								public void run() {
									text.setText(insert_text);
									modified = true;
								}
							});
						}
					}
					
					// modify both board and application
					if(modified){
						TextEditor(text.getText().toString());
						modified = false;
					}
					
					// set to default value
					for(int i=0;i<9;i++)
						input[i] = 0;
				}
				
				try{
					Thread.sleep(10);
				} catch(InterruptedException e){}
			}
		}
	}
	
	// If physical back button pressed, clear devices
	public boolean onKeyDown(int keyCode, android.view.KeyEvent event){
		TextEditor("");
		thread.flag = false;
		
		return super.onKeyDown(keyCode, event);
	}
}
