package com.jazzros.ffmpegtest;

import android.content.Context;
import android.os.Handler;
import android.os.Message;
import android.util.AttributeSet;
import android.view.SurfaceHolder;
import android.view.SurfaceView;
import android.widget.TextView;

import com.jazzros.ffmpegtest.enums.PlayState;

public class AVView extends SurfaceView implements SurfaceHolder.Callback
{
    //private final static String TAG = "FFMpeg::AVView";

    private TextView mTextView;
//    private AVThread mThread = null;

    public AVView(Context aContext, AttributeSet aAttrs)
    {
        super(aContext, aAttrs);

        getHolder().addCallback(this); //register SurfaceHolder.Callback
        getHolder().setKeepScreenOn(true); //keep screen on when this SurfaceView is displayed
    }

    public void setTextView(TextView aTextView)
    {
        mTextView = aTextView;
    }

    //SurfaceHolder.Callback
    public void surfaceChanged(SurfaceHolder aHolder, int aFormat, int aWidth, int aHeight)
    {
        AVThread.getInstance().setSurfaceSize(aHolder, aWidth, aHeight);
    }

    //SurfaceHolder.Callback
    public void surfaceCreated(SurfaceHolder aHolder)
    {
        //mThread = AVThread.getInstance();
/*
        mThread.updateHolder(aHolder, new Handler() {
            public void handleMessage(Message m) {
                mTextView.setVisibility(m.getData().getInt("viz"));
                mTextView.setText(m.getData().getString("text"));
            }
        });
*/
        AVThread.getInstance().updateHolder(aHolder, null);

        if (AVThread.getInstance().getPlayState() == PlayState.STOPPED)
            AVThread.getInstance().playResume();
    }

    //SurfaceHolder.Callback
    public void surfaceDestroyed(SurfaceHolder aHolder)
    {
        /*
        AVThread.getInstance().finish();
        try {
            AVThread.getInstance().join();
        }
        catch (InterruptedException e)
        {
        }
        */
    }
    /*
    public void SwitchPause() {
        if (AVThread.getInstance() != null) {
            AVThread.getInstance().SwitchPause();
        }
    }
    */
    public PlayState getPlayState() {
        return AVThread.getInstance().getPlayState();
    }
    public void playPause() {
        AVThread.getInstance().playPause();
    }
    public void playResume() {
        AVThread.getInstance().playResume();
    }

};
