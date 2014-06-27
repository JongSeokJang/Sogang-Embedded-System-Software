package com.example.androidex;

import java.util.Calendar;

import com.example.androidex.FigureActivity.hwThread;

import android.annotation.SuppressLint;
import android.app.Activity;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;



public class WatchActivity extends Activity{
	
	public native void Watch(String date, String time);
	public native void WatchFND(String stop);

	LinearLayout linear;
	Button btn_settime, btn_month, btn_day, btn_hour, btn_minute;
	Button btn_start, btn_pause, btn_stop, btn_main;
	TextView text_date, text_time, text_stopwatch;
	OnClickListener set_listener, month_listener, day_listener, hour_listener;
	OnClickListener minute_listener, start_listener, pause_listener, stop_listener;
	OnClickListener main_listener;
	int year, month, day, hour, minute;
	Calendar cal;
	stopwatch thread;
	
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_watch);
		linear = (LinearLayout)findViewById(R.id.container);
		
		// Load C library
		System.loadLibrary("dangercloz_module");
				
		// Initialize objects from view found by ID
		btn_settime = (Button)findViewById(R.id.btn_settime);
		btn_month = (Button)findViewById(R.id.btn_month);
		btn_day = (Button)findViewById(R.id.btn_day);
		btn_hour = (Button)findViewById(R.id.btn_hour);
		btn_minute = (Button)findViewById(R.id.btn_minute);
		btn_start = (Button)findViewById(R.id.btn_start);
		btn_pause = (Button)findViewById(R.id.btn_pause);
		btn_stop = (Button)findViewById(R.id.btn_stop);
		btn_main = (Button)findViewById(R.id.btn_main);
		text_date = (TextView)findViewById(R.id.text_date);
		text_time = (TextView)findViewById(R.id.text_time);
		text_stopwatch = (TextView)findViewById(R.id.text_stopwatch);
		
		thread = new stopwatch();
		
		// Get time on board
		cal = Calendar.getInstance();
		year = cal.get(Calendar.YEAR);
		month = cal.get(Calendar.MONTH);
		day = cal.get(Calendar.DAY_OF_MONTH);
		hour = cal.get(Calendar.HOUR_OF_DAY);
		minute = cal.get(Calendar.MINUTE);
		
		// Set initial time on application
		text_date.setText(year + "/" + String.format("%02d", month) + "/" + String.format("%02d", day));
		text_time.setText(String.format("%02d", hour) + ":" + String.format("%02d", minute));
		
		// Initialize listeners
		set_listener = new OnClickListener(){
			@Override
			@SuppressLint("DefaultLocale")
			public void onClick(View v){
				cal.set(Calendar.YEAR, year);
				cal.set(Calendar.MONTH, month);
				cal.set(Calendar.DAY_OF_MONTH, day);
				cal.set(Calendar.HOUR_OF_DAY, hour);
				cal.set(Calendar.MINUTE, minute);
				
				String jdate = String.format("%d/%02d/%02d      ", year, month, day);
				String jtime = String.format("%02d:%02d           ", hour, minute);
				Watch(jdate, jtime);
			}
		};
		
		month_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				month++;
				if(month > 12){
					month = 1;
					year++;
				}
				
				text_date.setText(year + "/" + String.format("%02d", month) + "/" + String.format("%02d", day));
			}
		};
		
		day_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				day++;
				if(day > 31)
					day = 1;
				
				text_date.setText(year + "/" + String.format("%02d", month) + "/" + String.format("%02d", day));
			}
		};
		
		hour_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				hour++;
				if(hour > 23)
					hour = 0;
				
				text_time.setText(String.format("%02d", hour) + ":" + String.format("%02d", minute));
			}
		};
		
		minute_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				minute++;
				if(minute > 59)
					minute = 0;
				
				text_time.setText(String.format("%02d", hour) + ":" + String.format("%02d", minute));
			}
		};
		
		start_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				if(thread.pause == false){
					if(!thread.isAlive()){
						thread.setDaemon(true);
						thread.start();
					}
				} else
					thread.pause = false;
			}
		};
		
		pause_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				thread.pause = true;
			}
		};
		
		stop_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				if(thread.isAlive()){
					thread.flag = false;
				}
				
				runOnUiThread(new Runnable() {
					@Override
					public void run() {
						text_stopwatch.setText("00:00");
					}
				});
				WatchFND("0000");
				
				thread = new stopwatch();
			}
		};
		
		main_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				Watch("N", "");
				WatchFND("0000");
				thread.flag = false;
				onBackPressed();
			}
		};
		
		// Add button listener to the corresponding button
		btn_settime.setOnClickListener(set_listener);
		btn_month.setOnClickListener(month_listener);
		btn_day.setOnClickListener(day_listener);
		btn_hour.setOnClickListener(hour_listener);
		btn_minute.setOnClickListener(minute_listener);
		btn_start.setOnClickListener(start_listener);
		btn_pause.setOnClickListener(pause_listener);
		btn_stop.setOnClickListener(stop_listener);
		btn_main.setOnClickListener(main_listener);
	}
	
	class stopwatch extends Thread{
		boolean flag = true;
		boolean pause = false;
		int stop_min = 0, stop_sec = 0;
		
		public void run(){
			while(flag){
				try{
					if(pause){}
					else{
						runOnUiThread(new Runnable() {
							@Override
							public void run() {
								text_stopwatch.setText(String.format("%02d", stop_min) + ":" + String.format("%02d", stop_sec));
							}
						});
						WatchFND(String.format("%02d%02d", stop_min, stop_sec));
						
						Thread.sleep(1000);
						
						stop_sec++;
						if(stop_sec > 59){
							stop_sec = 0;
							stop_min++;
						}
					}
				} catch(InterruptedException e){}
			}
		}
	}
	
	// If physical back button pressed, clear devices
	public boolean onKeyDown(int keyCode, android.view.KeyEvent event){
		Watch("N", "");
		WatchFND("0000");
		thread.flag = false;
		
		return super.onKeyDown(keyCode, event);
	}
}
