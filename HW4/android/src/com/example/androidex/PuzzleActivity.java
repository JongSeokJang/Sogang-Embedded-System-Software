package com.example.androidex;

import java.util.Random;
import java.util.regex.Pattern;

import android.app.Activity;
import android.graphics.Color;
import android.os.Bundle;
import android.view.View;
import android.view.View.OnClickListener;
import android.view.ViewGroup.LayoutParams;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;

public class PuzzleActivity extends Activity{
	LinearLayout linear;
	Button btn_create;
	EditText input_text;
	OnClickListener create_listener, move_listener;
	Button dynamic_array[];
	
	protected void onCreate(Bundle savedInstanceState){
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_puzzle);
		
		// Create entities in activity found by ID
		linear = (LinearLayout)findViewById(R.id.container);
		linear.setOrientation(LinearLayout.VERTICAL);
		btn_create = (Button)findViewById(R.id.create_button);
		input_text = (EditText)findViewById(R.id.input_text);
		
		// Add listener for creating the puzzle
		create_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				String text;
				String[] columns = null;
				
				// Extract input from edit text
				text = input_text.getText().toString();
				
				if(text.length() != 0){
					columns = text.split(" ");	// split two inputs
					if(columns.length == 2){
						int row = 0, col = 0;
						
						// Check if both inputs are numbers between 1~5
						Pattern ps = Pattern.compile("^[1-5]+$");
						if(ps.matcher(columns[0]).matches()){
							row = Integer.parseInt(columns[0]);
						}
						if(ps.matcher(columns[1]).matches()){
							col = Integer.parseInt(columns[1]);
						}
						
						if(row != 0 && col != 0){
							// Nothing happens when row and col both are 1
							if(row == 1 && col == 1){}
							else{
								// Check if buttons have created or not
								if(dynamic_array != null){}	// Nothing happens
								else{
									// Create buttons if this is first time
									dynamic_array = new Button[row*col];
									create_buttons(row, col);
									mix_puzzle(row, col);
								}
							}
						}
					}
				}
			}
		};
		btn_create.setOnClickListener(create_listener);
	}
	
	// Check for finish of game
	public void check_finish(int row, int col){
		boolean flag = true;
		
		for(int i=0;i<row;i++){
			for(int j=0;j<col;j++){
				if(!(dynamic_array[(i*col)+j].getText().equals(String.valueOf((i*col)+j+1)))){
					flag = false;
				}
			}
		}
		
		// If all buttons are in right position
		// Go back to previous activity
		if(flag == true){
			onBackPressed();
		}
	}
	
	// Mixing puzzle algorithm (Making solvable puzzle)
	public void mix_puzzle(int r, int c){
		int count = 0;
		int row = r-1;
		int col = c-1;
		
		do{
			// Mix puzzle 1000 times
			while(true){
				Random ran = new Random();
				int move = ran.nextInt(4)+1;
				String num;
				
				switch(move){
					case 1:
						// Move up
						if(row-1 >= 0){
							num = dynamic_array[(row-1)*c + col].getText().toString();
							
							dynamic_array[(row-1)*c + col].setText(String.valueOf(r*c));
							dynamic_array[(row-1)*c + col].setBackgroundColor(Color.BLACK);
							
							dynamic_array[(row*c)+col].setText(num);
							dynamic_array[(row*c)+col].setBackgroundResource(android.R.drawable.btn_default);
							
							row = row - 1;
						}
						break;
					case 2:
						// Move down
						if(row+1 < r){
							num = dynamic_array[(row+1)*c + col].getText().toString();
							
							dynamic_array[(row+1)*c + col].setText(String.valueOf(r*c));
							dynamic_array[(row+1)*c + col].setBackgroundColor(Color.BLACK);
							
							dynamic_array[(row*c)+col].setText(num);
							dynamic_array[(row*c)+col].setBackgroundResource(android.R.drawable.btn_default);
							
							row = row + 1;
						}
						break;
					case 3:
						// Move left
						if(col-1 >= 0){
							num = dynamic_array[(row*c) + col-1].getText().toString();
							
							dynamic_array[(row*c) + col-1].setText(String.valueOf(r*c));
							dynamic_array[(row*c) + col-1].setBackgroundColor(Color.BLACK);
							
							dynamic_array[(row*c)+col].setText(num);
							dynamic_array[(row*c)+col].setBackgroundResource(android.R.drawable.btn_default);
							
							col = col - 1;
						}
						break;
					case 4:
						// Move right
						if(col+1 < c){
							num = dynamic_array[(row*c) + col+1].getText().toString();
							
							dynamic_array[(row*c) + col+1].setText(String.valueOf(r*c));
							dynamic_array[(row*c) + col+1].setBackgroundColor(Color.BLACK);
							
							dynamic_array[(row*c)+col].setText(num);
							dynamic_array[(row*c)+col].setBackgroundResource(android.R.drawable.btn_default);
							
							col = col + 1;
						}
						break;
				}
				
				break;
			}
			count++;
		}while(count < 1000);
	}
	
	// Create buttons from 0 to given number (row * col)
	public void create_buttons(final int r, final int c){
		for(int i=0;i<r;i++){
			LinearLayout row = new LinearLayout(this);
			
			LinearLayout.LayoutParams row_param = new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT, (float)1.0);
			row.setLayoutParams(row_param);
			
			for(int j=0;j<c;j++){
				// Create a button
				Button btnTag = new Button(this);
				
				LinearLayout.LayoutParams col_param = new LinearLayout.LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.MATCH_PARENT, (float)1.0);
				btnTag.setLayoutParams(col_param);
				
				// Copy text to button
				String num = String.valueOf(i*c+j+1);
				btnTag.setText(num);
				
				// Add button to row
				row.addView(btnTag);
				
				// Add button to the array
				dynamic_array[i*c+j] = btnTag;
				
				// Set background color to black for last button
				if(Integer.parseInt(num) == r*c){
					btnTag.setBackgroundColor(Color.BLACK);
				}
				
				// Create listener of button's movement
				move_listener = new OnClickListener(){
					@Override
					public void onClick(View v){
						int row = 0, col = 0;
						
						// Get button text of current button
						Button b = (Button)v;
						String current_button = b.getText().toString();
						
						// Find this button's current position (k, l)
						for(int k=0;k<r;k++){
							for(int l=0;l<c;l++){
								if((dynamic_array[(k*c)+l].getText().toString()).equals(current_button)){
									row = k;	col = l;
								}
							}
						}
						
						// Check for movement availability
						if(row-1 >= 0){
							if((dynamic_array[(row-1)*c + col]).getText().equals(String.valueOf(r*c))){
								// Exchange Button with above
								dynamic_array[(row*c)+col].setText(String.valueOf(r*c));
								dynamic_array[(row*c)+col].setBackgroundColor(Color.BLACK);
								
								dynamic_array[(row-1)*c + col].setText(current_button);
								dynamic_array[(row-1)*c + col].setBackgroundResource(android.R.drawable.btn_default);
							}
						}
						
						if(row+1 < r){
							if((dynamic_array[(row+1)*c + col]).getText().equals(String.valueOf(r*c))){
								// Exchange Button with below
								dynamic_array[(row*c)+col].setText(String.valueOf(r*c));
								dynamic_array[(row*c)+col].setBackgroundColor(Color.BLACK);
								
								dynamic_array[(row+1)*c + col].setText(current_button);
								dynamic_array[(row+1)*c + col].setBackgroundResource(android.R.drawable.btn_default);
							}
						}
						
						if(col-1 >= 0){
							if((dynamic_array[row*c + col - 1]).getText().equals(String.valueOf(r*c))){
								// Exchange Button with left
								dynamic_array[(row*c)+col].setText(String.valueOf(r*c));
								dynamic_array[(row*c)+col].setBackgroundColor(Color.BLACK);
								
								dynamic_array[row*c + col - 1].setText(current_button);
								dynamic_array[row*c + col - 1].setBackgroundResource(android.R.drawable.btn_default);
							}
						}
						
						if(col+1 < c){
							if((dynamic_array[row*c + col + 1]).getText().equals(String.valueOf(r*c))){
								// Exchange Button with right
								dynamic_array[(row*c)+col].setText(String.valueOf(r*c));
								dynamic_array[(row*c)+col].setBackgroundColor(Color.BLACK);
								
								dynamic_array[row*c + col + 1].setText(current_button);
								dynamic_array[row*c + col + 1].setBackgroundResource(android.R.drawable.btn_default);
							}
						}
						
						// Check for finish (only when last button is at button right side
						if((dynamic_array[r*c-1].getText().equals(String.valueOf(r*c)))){
							check_finish(r, c);
						}
					}
				};
				btnTag.setOnClickListener(move_listener);	// add button listener
			}
			
			linear.addView(row);
		}
	}
}