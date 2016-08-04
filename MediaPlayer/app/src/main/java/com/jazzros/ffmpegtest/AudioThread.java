package com.jazzros.ffmpegtest;

import android.media.AudioFormat;
import android.media.AudioManager;
import android.media.AudioTrack;
import android.util.Log;

import com.jazzros.ffmpegtest.enums.PlayState;

public class AudioThread {
    private final static String TAG = "FFMpegAV";
    private final static int AVCODEC_MAX_AUDIO_FRAME_SIZE = 2000000;
//    private Handler 	mHandler = null;
    private AudioTrack 	mAudioTrack = null;
    private PlayState   mState;
    private int 		minBufferSize;
    private String 		mfileName;
    private byte[] 		mAudioFrameBuffer = new byte[AVCODEC_MAX_AUDIO_FRAME_SIZE];
    private byte[] 		mAudioFrameBufferOut = new byte[AVCODEC_MAX_AUDIO_FRAME_SIZE];
    private int[] 		mAudioFrameBufferDataLength = new int[1];
    int					mAudioFrameBufferOutPosition = 0;
    long                mLastPlaybackPosition;

//    public AudioThread(String fileName, Handler aHandler)
    public AudioThread(String fileName)
    {
        //
        // Initialize parameters
        //
        mfileName = new String(fileName);
//        mHandler = aHandler;
        mState = PlayState.STOPPED;
        mLastPlaybackPosition = 0;

        //
        //
        //
        final int frequency = 44100;
        final int channelConfig = AudioFormat.CHANNEL_CONFIGURATION_MONO;
        final int channelFormat = AudioFormat.ENCODING_PCM_16BIT;
        minBufferSize = AudioTrack.getMinBufferSize(frequency,
                channelConfig,
                channelFormat);
        mAudioTrack = new AudioTrack(AudioManager.STREAM_MUSIC,
                frequency,
                channelConfig,
                channelFormat,
                minBufferSize,
                AudioTrack.MODE_STREAM);
        //
        if (nativeFFmpegOpenAudio(mfileName,
                mAudioFrameBuffer, mAudioFrameBufferDataLength,
                mAudioTrack.getChannelCount(),
                1/*AV_SAMPLE_FMT_S16*/,
                mAudioTrack.getSampleRate(),
                1000/*good for 44100 Hz*/) != 0) {
            nativeFFmpegCloseAudio();
            Log.i(TAG, "nativeOpen() failed, throwing RuntimeException");
            throw new RuntimeException();
        }
    }
    /*
    public void start () {
        final int frequency = 44100;
        final int channelConfig = AudioFormat.CHANNEL_CONFIGURATION_MONO;
        final int channelFormat = AudioFormat.ENCODING_PCM_16BIT;
        minBufferSize = AudioTrack.getMinBufferSize(frequency,
                channelConfig,
                channelFormat);
        mAudioTrack = new AudioTrack(AudioManager.STREAM_MUSIC,
                frequency,
                channelConfig,
                channelFormat,
                minBufferSize,
                AudioTrack.MODE_STREAM);
        //
        if (nativeFFmpegOpenAudio(mfileName,
                mAudioFrameBuffer, mAudioFrameBufferDataLength,
                mAudioTrack.getChannelCount(),
                1,// AV_SAMPLE_FMT_S16
                mAudioTrack.getSampleRate(),
                1000) != 0) { // 1000 good for 44100 Hz
            nativeFFmpegCloseAudio();
            Log.i(TAG, "nativeOpen() failed, throwing RuntimeException");
            throw new RuntimeException();
        }

        startThread();
    }
*/
    private void loop() {
        Log.i(TAG, "entering audio loop()");

        mAudioTrack.play();

        mState = PlayState.RUN;
        try {
            while (mState == PlayState.RUN) {
                //
                //
                //
                int err = nativeFFmpegDecodeAudio();
                if (err >= 0) {
                    final int bytesToAdd = mAudioFrameBufferDataLength[0];
                    if (mAudioTrack.write(mAudioFrameBuffer, 0, bytesToAdd) != bytesToAdd) {
                        Log.e(TAG, "mAudioTrack.write(): some data not written");
                    }
                } else {
                    mState = PlayState.STOPPED;
                    Log.i(TAG, "Audio decoding error");
                }
            }
        } catch (Exception e) {
            mState = PlayState.STOPPED;
            Log.e(TAG, "", e);
        }

        // save current position
        mLastPlaybackPosition = getCurrentMS();
        // switch track to necessary state
        switch (mState) {
            case PAUSED:
                mAudioTrack.pause();
                break;
            case STOPPED:
                mAudioTrack.flush();
                mAudioTrack.stop();
                mAudioTrack.release();
                break;
        }

        Log.i(TAG, "leaving audio loop()");
    }

    public long getCurrentMS() {
        if (mAudioTrack != null) {
            if (mAudioTrack.getState() != AudioTrack.STATE_UNINITIALIZED) {
                if (mAudioTrack.getPlayState() != AudioTrack.PLAYSTATE_STOPPED) {
                    return (long) (((double) mAudioTrack.getPlaybackHeadPosition() / (double) mAudioTrack.getSampleRate()) * 1000.0);
                }
            }
        }
        return mLastPlaybackPosition;
    }


    public void finish()
    {
        mState = PlayState.STOPPED;

        killThread();

        nativeFFmpegCloseAudio();
    }

    public void playResume() {
        if (mState == PlayState.PAUSED || mState == PlayState.STOPPED) {
            // todo: actuallu here would be better to seek stream to \mLastPlaybackPosition
            startThread();
        }
    }
    public void playPause() {
        if (mState == PlayState.RUN) {
            mState = PlayState.PAUSED;

            killThread();
        }
    }
    public PlayState getPlayState() {
        return mState;
    }

    //////////////////////////////////////////////////////////////////////////////////////////////
    private Thread  mThread = null;
    private class ThreadInstance extends Thread {
        public void run() {
            loop();
        }
    }
    private void startThread() {
        if (mThread != null)
            killThread();
        mThread = new ThreadInstance();
        mThread.start();
    }
    private void killThread() {
        try {
            if (mThread != null) {
                if (mThread.isAlive()) {
                    mThread.join(100);
                    mThread = null;
                }
            }
        } catch (InterruptedException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
        }
    }

    private native int nativeFFmpegOpenAudio(String aMediaFile, byte[] aAudioFrameBufferRef, int[] aAudioFrameBufferCountRef,
                                             int channelsNb,
                                             int sampleFmt,
                                             int sampleRate,
                                             int sampleNb);

    private native int nativeFFmpegDecodeAudio();
    private native int nativeFFmpegCloseAudio();
};
