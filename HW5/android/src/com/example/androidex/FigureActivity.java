package com.example.androidex;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.util.regex.Pattern;

import android.app.Activity;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.TextView;

public class FigureActivity extends Activity {

	public native void FigureSwitch(String option, String left);
	public native void TextPrint(String id, String name);
	public native String PushSwitch();

	LinearLayout linear;
	OnClickListener go_listener, main_listener, clear_listener;
	EditText input;
	TextView usage, textchar;
	int time, num;
	String option, temp;
	hwThread thread;
	usageThread usage_thread;
	getSwitch switch_thread;
	Button btn_go, btn_main, btn_clear;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_figure);
		linear = (LinearLayout) findViewById(R.id.container);

		// Load C library
		System.loadLibrary("dangercloz_module");
		
		// Initialize threads
		thread = new hwThread();
		usage_thread = new usageThread();
		switch_thread = new getSwitch();
		switch_thread.setDaemon(true);
		switch_thread.start();

		// Initialize objects from view found by ID
		btn_go = (Button) findViewById(R.id.btn_go);
		btn_main = (Button) findViewById(R.id.btn_main);
		btn_clear = (Button) findViewById(R.id.btn_clear);
		input = (EditText) findViewById(R.id.figure_input);
		usage = (TextView) findViewById(R.id.text_usage);
		textchar = (TextView) findViewById(R.id.text_char);

		// Initialize listeners for each buttons
		go_listener = new OnClickListener() {
			@Override
			public void onClick(View v) {
				String text;
				String[] columns = null;

				// Extract input from edit text
				text = input.getText().toString();

				// Continue if user typed input
				if (text.length() != 0) {
					columns = text.split(" "); // split two inputs

					// Continue only if 3 inputs are in the string
					if (columns.length == 3) {
						boolean option_ok = true;
						
						// Check if all inputs are numbers
						Pattern ps = Pattern.compile("^[0-9]+$");
						if (ps.matcher(columns[0]).matches())
							time = Integer.parseInt(columns[0]);
						if (ps.matcher(columns[1]).matches())
							num = Integer.parseInt(columns[1]);
						if (ps.matcher(columns[2]).matches()){
							option = columns[2];
							char[] temp = option.toCharArray();
							int count = 0;
							for(int i=0;i<4;i++){
								if(temp[i] == '0')
									count++;
								else if(temp[i] != '0'){
									String temp2 = String.valueOf(temp[i]);
									ps = Pattern.compile("^[1-8]+$");
									if(!ps.matcher(temp2).matches())
										option_ok = false;
								}
							}
							if(count != 3)
								option_ok = false;
						}

						// Start only when both time and num are not 0
						if (time != 0 && num != 0 && option_ok == true) {
							if(!thread.isAlive()){
								thread.setDaemon(true);
								thread.start();
							}
							
							if(!usage_thread.isAlive()){
								usage_thread.setDaemon(true);
								usage_thread.start();
							}
						}
					}
				}
			}
		};

		main_listener = new OnClickListener() {
			@Override
			public void onClick(View v) {
				if(thread.isAlive())
					thread.stop();
				
				if(usage_thread.isAlive())
					usage_thread.stop();
				
				switch_thread.flag = false;
				
				onBackPressed();
			}
		};

		clear_listener = new OnClickListener() {
			@Override
			public void onClick(View v) {
				if(thread.isAlive()){
					thread.i = num;
					thread = new hwThread();
				}
				
				if(usage_thread.isAlive()){
					usage_thread.flag = false;
					usage_thread = new usageThread();
				}
				
				input.setText("");
			}
		};

		// Add button listener to the corresponding button
		btn_go.setOnClickListener(go_listener);
		btn_main.setOnClickListener(main_listener);
		btn_clear.setOnClickListener(clear_listener);
	}

	class hwThread extends Thread {
		int i, flag1 = 1, flag2 = 1;
		String student_id = "20091648        ";
		String student_name = "Lee Jun Ho      ";
		
		public void run() {
			int j = 0;
			i = 0;

			// Run while num is bigger than i
			while (i < num) {
				char op[] = option.toCharArray(); // Copy string to char array

				try {
					// Find where the char is
					for (j = 0; j < 4; j++)
						if (op[j] != '0')
							break;

					// Modify index number of char on application
					temp = String.valueOf(op[j]);
					runOnUiThread(new Runnable() {
						@Override
						public void run() {
							textchar.setText(temp);
						}
					});

					FigureSwitch(option, String.valueOf(num-i)); // Call C library function
					TextPrint(student_id, student_name);
					Thread.sleep(time * 100); // Sleep for given time
					i++;

					// Modify option for given rule
					if (op[j] != '8') {
						op[j]++;
					} else {
						if (j != 3) {
							op[j + 1] = '1';
							op[j] = '0';
						} else {
							op[j] = '0';
							op[0] = '1';
						}
					}
					option = String.copyValueOf(op);
					
					// Calculate student id and student name
					char id[] = student_id.toCharArray();
					char name[] = student_name.toCharArray();
					
					if(flag1 == 1){	// Move string to right
						for(int k=15;k>0;k--)
							id[k] = id[k-1];
						id[0] = ' ';
						
						if(id[15] != ' ')	// Change direction
							flag1 = 0;
					} else{	// Move string to left
						for(int k=0;k<15;k++)
							id[k] = id[k+1];
						id[15] = ' ';
						
						if(id[0] != ' ')	// Change direction
							flag1 = 1;
					}
					
					if(flag2 == 1){	// Move string to right
						for(int k=15;k>0;k--)
							name[k] = name[k-1];
						name[0] = ' ';
						
						if(name[15] != ' ')	// Change direction
							flag2 = 0;
					} else{	// Move string to left
						for(int k=0;k<15;k++)
							name[k] = name[k+1];
						name[15] = ' ';
						
						if(name[0] != ' ')	// Change direction
							flag2 = 1;
					}
					student_id = String.copyValueOf(id);
					student_name = String.copyValueOf(name);

				} catch (InterruptedException e) {}
			}

			FigureSwitch("0000", String.valueOf(0)); // Turn off fnd driver when finished
			TextPrint("N", "");	// Turn off fpga text driver when finished
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					textchar.setText("");
				}
			});
			
			thread = new hwThread();
		}
	}

	class usageThread extends Thread {
		boolean flag;
		
		public void run() {
			flag = true;
			
			while(flag){
				try {
					// Modify CPU usage text
					runOnUiThread(new Runnable() {
						@Override
						public void run() {
							try {
								FileReader fis = new FileReader("/proc/stat");
								BufferedReader br = new BufferedReader(fis);

								String str = null;
								String[] columns = null;

								// Read /proc/stat then split strings into tokens
								str = br.readLine();
								columns = str.split(" ");

								// Calculate CPU Usage
								int total_jiffies = 0;
								int usage_c;

								total_jiffies += Integer.parseInt(columns[2]);
								total_jiffies += Integer.parseInt(columns[3]);
								total_jiffies += Integer.parseInt(columns[4]);
								total_jiffies += Integer.parseInt(columns[5]);
								usage_c = Integer.parseInt(columns[5]) * 100
										/ total_jiffies;
								usage_c = 100 - usage_c;

								// Modify Text
								usage.setText("CPU usage : "
										+ String.valueOf(usage_c) + "%");

								// Close file and buffer
								fis.close();
								br.close();
							} catch (IOException e) {}
						}
					});
					
					Thread.sleep(1000);
				} catch (InterruptedException e) {}
			}
			
			// Set text to default value
			runOnUiThread(new Runnable() {
				@Override
				public void run() {
					usage.setText("CPU usage : %");
				}
			});
		}
	}
	
	class getSwitch extends Thread{
		boolean flag, zeroes;
		String temp, text;
		char[] temp2;
		char[] switch_input = new char[9];
		
		public void run(){
			flag = true;
			
			while(flag){
				zeroes = true;
				
				temp = PushSwitch();
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
					
					if(count == 2){	// if there are 2 inputs
						int first = -1, second = -1;
						
						for(int i=0;i<9;i++){
							if(switch_input[i] == '1'){
								if(first == -1)
									first = i;
								else
									second = i;
							}
						}
						
						if(first == 1 && second == 2){
							// (2) & (3) switch
							thread.i = num;
							usage_thread.flag = false;
							switch_thread.flag = false;
							onBackPressed();
						} else if(first == 3 && second == 4){
							// (4) & (5) switch
							if(thread.isAlive()){
								thread.i = num;
								thread = new hwThread();
							}
							
							if(usage_thread.isAlive()){
								usage_thread.flag = false;
								usage_thread = new usageThread();
							}
							
							// print on board text
							runOnUiThread(new Runnable() {
								@Override
								public void run() {
									input.setText("");
								}
							});
						} else if(first == 4 && second == 5){
							// (5) & (6) switch (Spacing)
							// print on board text
							runOnUiThread(new Runnable() {
								@Override
								public void run() {
									text = input.getText().toString();
									
									if(text == null)
										text = " ";
									else
										text += " ";
									
									input.setText(text);
								}
							});
						} else if(first == 6 && second == 7){
							// (7) & (8) switch (Go button)
							btn_go.post(new Runnable() {
								@Override
								public void run() {
									btn_go.performClick();
								}
							});
						}
					} else if(count == 1){	// Only 1 input is in
						int i;
						for (i = 0; i < 9; i++)
							if (switch_input[i] == '1')
								break;
						
						if(i == 8)
							i = -1;
						
						text = input.getText().toString();
						
						if(text == null)
							text = String.valueOf(i+1);
						else
							text += String.valueOf(i+1);
						
						// print on board text
						runOnUiThread(new Runnable() {
							@Override
							public void run() {
								input.setText(text);
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
		thread.i = num;
		usage_thread.flag = false;
		switch_thread.flag = false;
		
		return super.onKeyDown(keyCode, event);
	}
}
