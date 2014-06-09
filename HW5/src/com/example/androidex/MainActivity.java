package com.example.androidex;

import com.example.androidex.R;

import android.app.Activity;
import android.media.MediaPlayer;
import android.os.Bundle;
import android.widget.LinearLayout;



public class MainActivity extends Activity{
	
	MediaPlayer mp1, mp2;

	LinearLayout linear;
	@Override
	protected void onCreate(Bundle savedInstanceState) {
		// TODO Auto-generated method stub
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		linear = (LinearLayout)findViewById(R.id.container);
	}

}
