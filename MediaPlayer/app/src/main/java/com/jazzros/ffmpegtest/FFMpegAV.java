package com.jazzros.ffmpegtest;

// AsyncTask acress rotated activity
// http://www.androiddesignpatterns.com/2013/04/retaining-objects-across-config-changes.html

import android.app.Activity;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.util.Log;
import android.view.View;
import android.view.Window;
import android.view.WindowManager;
import android.widget.TextView;

import com.jazzros.ffmpegtest.enums.PlayState;

import org.libsdl.app.SDLActivity;

public class FFMpegAV extends SDLActivity
{
    private final static String 	TAG = "FFMpegAV";

    private FFMpegView  			mVideoView;
    private Boolean                 goFullScreen = false;
    private ActPreferences          mPrefs;

    private class ActPreferences {
        private final String PREFERENCES_FULLSCREEN = "pref_fullscreen";
        private final String PREFERENCES_PLAYERSTATE = "pref_playerstate";

        private SharedPreferences       sPref;

        public ActPreferences() {
            loadPreferences();
        }

        @Override
        protected void finalize() throws Throwable {
            savePreferences();
            super.finalize();
        }

        private void savePreferences() {
            sPref = getPreferences(MODE_PRIVATE);
            SharedPreferences.Editor ed = sPref.edit();

            ed.putString(PREFERENCES_FULLSCREEN, goFullScreen.toString());
            ed.putString(PREFERENCES_PLAYERSTATE, mVideoView.getPlayState().toString());

            ed.commit();
        }

        private void loadPreferences() {
            sPref = getPreferences(MODE_PRIVATE);

            goFullScreen = Boolean.valueOf(sPref.getString(PREFERENCES_FULLSCREEN, "false"));

            PlayState ps =  PlayState.valueOf(sPref.getString(PREFERENCES_PLAYERSTATE, "STOPPED"));
            switch (ps) {
                case STOPPED:
                    mVideoView.playPause();
                    break;
                case PAUSED:
                    mVideoView.playPause();
                    break;
                case RUN:
                    mVideoView.playResume();
                    break;
            }
        }
    }

    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);


        mVideoView = (FFMpegView)findViewById(R.id.video);
//        mVideoView.setTextView((TextView) findViewById(R.id.text));

        mVideoView.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View view) {

                //mVideoView.SwitchPause();
                switch (mVideoView.getPlayState()) {
                    case RUN:
                        mVideoView.playPause();
                        break;
                    case PAUSED:
                        mVideoView.playResume();
                        break;
                }
            }
        });
        mVideoView.setOnLongClickListener(new View.OnLongClickListener() {
            @Override
            public boolean onLongClick(View view) {

                goFullScreen = !goFullScreen;
                toggleFullScreen();

                return true;
            }
        });
        //
        // Load activity preferences
        // \mVideoView shoule be exist becuase pref could start player
        mPrefs = new ActPreferences();

        toggleFullScreen(); // apply fullscreen state
    }

    /*
    public void onConfigurationChanged(Configuration newConfig)
    {
        Log.i(TAG, "onConfigurationChanged()");
        setRequestedOrientation(ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE);
        super.onConfigurationChanged(newConfig);
    }
    */
/*
    @Override
    protected void onResume() {
        mVideoView.playResume();
        super.onResume();
    }
*/
    @Override
    protected void onDestroy() {
        Log.d(TAG, "onDestroy");

        try {
            mPrefs.finalize(); // Save preferences
            mVideoView.playPause();
        } catch (Throwable throwable) {
            //throwable.printStackTrace();
            Log.e(TAG, "", throwable);
        }

        super.onDestroy();
    }

    private void toggleFullScreen(){
        if(goFullScreen){
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
            getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FORCE_NOT_FULLSCREEN);
        }else{
            getWindow().addFlags(WindowManager.LayoutParams.FLAG_FORCE_NOT_FULLSCREEN);
            getWindow().clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
        }

        mVideoView.requestLayout();
    }

    public static native int nativeInit(Object arguments);

    static {
        System.loadLibrary("avutil");
        System.loadLibrary("swresample");
        System.loadLibrary("swscale");
        System.loadLibrary("avcodec");
        System.loadLibrary("avformat");
        System.loadLibrary("avdevice");

        System.loadLibrary("SDL2");

        System.loadLibrary("avjni");
    }
};

