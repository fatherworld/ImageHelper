package com.alvasystems.ImageDC;

import android.app.Activity;
import android.app.NativeActivity;
import android.content.Intent;
import android.os.Bundle;
import android.util.Log;

public class Loader extends Activity{

	public void onCreate(Bundle savedInstanceState) {
	    super.onCreate(savedInstanceState);

	    try{
		    System.loadLibrary("gnustl_shared");
		    System.loadLibrary("ImageDC");
	    }
	    catch(Exception e){
	    	Log.e("fenglang", "load: " + e);
	    }
	    
	    Intent i = new Intent(this, NativeActivity.class);
        startActivity(i);
	}
}
