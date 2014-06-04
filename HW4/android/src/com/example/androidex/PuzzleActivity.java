package com.example.androidex;

import java.util.regex.Pattern;

import android.app.Activity;
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
	OnClickListener create_listener;
	
	protected void onCreate(Bundle savedInstanceState){
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_puzzle);
		
		linear = (LinearLayout)findViewById(R.id.container);
		linear.setOrientation(LinearLayout.VERTICAL);
		btn_create = (Button)findViewById(R.id.create_button);
		input_text = (EditText)findViewById(R.id.input_text);
		
		create_listener = new OnClickListener(){
			@Override
			public void onClick(View v){
				String text;
				String[] columns = null;
				
				// Extract input from edit text
				text = input_text.getText().toString();
				
				if(text.length() != 0){
					columns = text.split(" ");
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
								// TODO : create something!!!!
								create_buttons();
							}
						}
					}
				}
			}
		};
		btn_create.setOnClickListener(create_listener);
	}
	
	public void create_buttons(){
		for(int i=0;i<3;i++){
			LinearLayout row = new LinearLayout(this);
			row.setLayoutParams(new LayoutParams(LayoutParams.MATCH_PARENT, LayoutParams.WRAP_CONTENT));
			
			for(int j=0;j<4;j++){
				Button btnTag = new Button(this);
				btnTag.setLayoutParams(new LayoutParams(LayoutParams.WRAP_CONTENT, LayoutParams.WRAP_CONTENT));
				btnTag.setText("Button" + (j+1+(i*4)));
				row.addView(btnTag);
			}
			
			linear.addView(row);
		}
	}
}