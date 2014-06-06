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
import android.widget.LinearLayout;
import android.widget.TextView;

public class ProcActivity extends Activity{
	LinearLayout linear;
	Button btn_read, btn_start, btn_stop;
	OnClickListener read_listener, start_listener, stop_listener;
	CPUThread thread;
	static TextView text_info, text_usage;
	boolean flag = false;
	
	protected void onCreate(Bundle savedInstanceState){
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_proc);
		
		// Create entities in activity found by ID
		linear = (LinearLayout)findViewById(R.id.container);
		btn_read = (Button)findViewById(R.id.file_read);
		btn_start = (Button)findViewById(R.id.parsing_start);
		btn_stop = (Button)findViewById(R.id.parsing_stop);
		text_info = (TextView)findViewById(R.id.stat_contents);
		text_usage = (TextView)findViewById(R.id.cpu_usage);
		thread = new CPUThread(mHandler);
		
		// Add listener for read CPU usage
		read_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				CPUThread.CPUUsages();
			}
		};
		btn_read.setOnClickListener(read_listener);
		
		// Add listener for start to read CPU usage
		start_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				if(flag == false){
					thread.setDaemon(true);
					thread.start();
					flag = true;
				}
			}
		};
		btn_start.setOnClickListener(start_listener);
		
		// Add listener for stop reading CPU usage and go back to previous activity
		stop_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				thread.interrupt();
				onBackPressed();
			}
		};
		btn_stop.setOnClickListener(stop_listener);
	}
	
	// If physical back button pressed, stop thread
	public boolean onKeyDown(int keyCode, android.view.KeyEvent event){
		if(!thread.isInterrupted()){
			thread.interrupt();
		}
		
		return super.onKeyDown(keyCode, event);
	}
	
	// Interrupt handler for printing proc parsing
	@SuppressLint("HandlerLeak")
	Handler mHandler = new Handler(){
		public void handleMessage(Message msg){
			if(msg.what == 0){
				try{
					FileReader fis = new FileReader("/proc/stat");
					BufferedReader br = new BufferedReader(fis);
					
					String str = null;
					String[] columns = null;
					
					// Read /proc/stat/ then split strings into tokens
					str = br.readLine();
					columns = str.split(" ");
					
					// Calculate CPU usage
					int total_jiffies = 0;
					int usage;
					total_jiffies += Integer.parseInt(columns[2]);
					total_jiffies += Integer.parseInt(columns[3]);
					total_jiffies += Integer.parseInt(columns[4]);
					total_jiffies += Integer.parseInt(columns[5]);
					usage = Integer.parseInt(columns[5])*100 / total_jiffies;
					usage = 100 - usage;	// 100 - idle time
					
					// Change text label
					text_info.setText("/proc/stat : " + str);
					text_usage.setText("cpu usage : " + String.valueOf(usage) + "%");
					
					fis.close();
					br.close();
				} catch(IOException e){}
			}
		}
	};
}

class CPUThread extends Thread{
	static Handler mHandler;
	
	// Constructor for handler
	CPUThread(Handler handler){
		mHandler = handler;
	}
	
	public void run(){
		try{
			while(!Thread.currentThread().isInterrupted()){
				CPUUsages();
				Thread.sleep(1000);
			}
		} catch(InterruptedException e){;}
	}
	
	// Make message for CPU usage running
	public static void CPUUsages(){
		Message msg = Message.obtain();
		msg.what = 0;
		mHandler.sendMessage(msg);
	}
}