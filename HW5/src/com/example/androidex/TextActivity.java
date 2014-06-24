package com.example.androidex;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;



public class TextActivity extends Activity{

	public native void TextEditor(String string);
	
	LinearLayout linear;
	Button btn_modify, btn_clear, btn_main, btn_usage;
	EditText text, usage_text, length_text;
	OnClickListener modify_listener, clear_listener, main_listener, usage_listener;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_text);
		linear = (LinearLayout)findViewById(R.id.container);
		
		// Load C library
		System.loadLibrary("dangercloz_module");
		
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
	
	// If physical back button pressed, clear devices
	public boolean onKeyDown(int keyCode, android.view.KeyEvent event){
		TextEditor("");
		
		return super.onKeyDown(keyCode, event);
	}
}
