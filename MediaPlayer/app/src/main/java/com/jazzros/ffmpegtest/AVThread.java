
package com.jazzros.ffmpegtest;

import android.content.Context;
import android.graphics.Bitmap;
import android.graphics.Canvas;
import android.os.Bundle;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;
import android.view.SurfaceHolder;
import android.view.View;

import com.jazzros.ffmpegtest.enums.PlayState;

class AVThread
{
	private final static String TAG = "FFMpegAV";
	private final static int AUDIO_DATA_ID = 1;
	private final static int VIDEO_DATA_ID = 2;
	private final static int AVCODEC_MAX_AUDIO_FRAME_SIZE = 2000000;

	private int mFpsCounter = 0;
	private long mLastTime = System.currentTimeMillis();

	private Bitmap mBackground;
	//
	//
//	private Handler mHandler;
//	private boolean mRun;
	private PlayState   mState;
	private boolean mVideoOpened;
	private boolean mAudioOpened;
	private long 	videoFramesNb;
	private SurfaceHolder mSurfaceHolder = null;
	//
	private AudioThread mAudioThread = null;

	private static AVThread instance = null;
	public static AVThread getInstance() {
		if (instance == null) {
			instance = new AVThread();
			instance.init();
		}
		return instance;
	}
	private void init() {
		mState = PlayState.STOPPED;
		videoFramesNb = 0;
		//ARGB_8888 == AV_PIX_FMT_RGBA
		//RGB_565 == AV_PIX_FMT_RGB565LE
		mBackground = Bitmap.createBitmap(800, 480, Bitmap.Config.RGB_565); //RGB_565 or ARGB_8888

		//String fileName =Environment.getExternalStorageDirectory().getAbsolutePath() + "/Steve Jobs vs Bill Gates. Epic Rap Battles of History Season 2.mp4";
		//String fileName =Environment.getExternalStorageDirectory().getAbsolutePath() + "/Ozzy Osbourne - Mama, I'm Coming Home.mp4";
		//String fileName =Environment.getExternalStorageDirectory().getAbsolutePath() + "/Serenity - HD DVD Trailer.mp4";
		//String fileName =Environment.getExternalStorageDirectory().getAbsolutePath() + "/H264_test1_Talkinghead_mp4_480x360.mp4";
		//		String fileName =Environment.getExternalStorageDirectory().getAbsolutePath() + "/test2.mp4";
		//		String fileName =Environment.getExternalStorageDirectory().getAbsolutePath() + "/big_buck_bunny_720p_MPEG4_MP3_25fps_3300K_short.mp4";
		String fileName =Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM).getAbsolutePath() + "/Serenity - HD DVD Trailer.mp4";
		if (nativeOpenFromFile(fileName) != 0) {
			nativeClose();
			Log.i(TAG, "nativeOpen() failed, throwing RuntimeException");
			throw new RuntimeException();
		}
//		mAudioThread = new AudioThread(fileName, aHandler);
		mAudioThread = new AudioThread(fileName);

		//since both video and audio are optional here, we try them in turn
		//if both were required, we could use just one check like this:
		//if (nativeOpen(Environment.getExternalStorageDirectory().getAbsolutePath() + "/mediafilename") != 0
		//    || nativeOpenVideo(mBackground) != 0
		//    || nativeOpenAudio(mAudioFrameBuffer, mAudioFrameBufferDataLength) != 0)
		//    {
		//    nativeClose();
		//    throw new RuntimeException();
		//    }

		mVideoOpened = nativeOpenVideo(mBackground) == 0;
		if (!mVideoOpened)
			nativeCloseVideo();

		if (!mVideoOpened && !mAudioOpened)
		{
			Log.i(TAG, "unable to open a stream, throwing RuntimeException");
			throw new RuntimeException();
		}
	}
	public void updateHolder(SurfaceHolder aSurfaceHolder, Bitmap bitmap) {
		if (mSurfaceHolder == null) {
			mSurfaceHolder = aSurfaceHolder;
		}
		else {
			synchronized (mSurfaceHolder) {
				mSurfaceHolder = aSurfaceHolder;
			}
			if (bitmap != null) {
				synchronized (mBackground) {
					if (nativeSetBitmapBuffer(bitmap) == 0)
						mBackground = bitmap;
				}
			}
			updateCanvas();
		}
	}
/*
	public void AVThread(SurfaceHolder aSurfaceHolder, Handler aHandler)
	{
		mSurfaceHolder = aSurfaceHolder;
		mHandler = aHandler;

		//ARGB_8888 == AV_PIX_FMT_RGBA
		//RGB_565 == AV_PIX_FMT_RGB565LE
		mBackground = Bitmap.createBitmap(800, 480, Bitmap.Config.RGB_565); //RGB_565 or ARGB_8888

		//String fileName =Environment.getExternalStorageDirectory().getAbsolutePath() + "/Steve Jobs vs Bill Gates. Epic Rap Battles of History Season 2.mp4";
		//String fileName =Environment.getExternalStorageDirectory().getAbsolutePath() + "/Ozzy Osbourne - Mama, I'm Coming Home.mp4";
		//String fileName =Environment.getExternalStorageDirectory().getAbsolutePath() + "/Serenity - HD DVD Trailer.mp4";
		//String fileName =Environment.getExternalStorageDirectory().getAbsolutePath() + "/H264_test1_Talkinghead_mp4_480x360.mp4";
		//		String fileName =Environment.getExternalStorageDirectory().getAbsolutePath() + "/test2.mp4";
		//		String fileName =Environment.getExternalStorageDirectory().getAbsolutePath() + "/big_buck_bunny_720p_MPEG4_MP3_25fps_3300K_short.mp4";
		String fileName =Environment.getExternalStoragePublicDirectory(Environment.DIRECTORY_DCIM).getAbsolutePath() + "/Serenity - HD DVD Trailer.mp4";
		if (nativeOpenFromFile(fileName) != 0) {
			nativeClose();
			Log.i(TAG, "nativeOpen() failed, throwing RuntimeException");
			throw new RuntimeException();
		}
//		mAudioThread = new AudioThread(fileName, aHandler);
		mAudioThread = new AudioThread(fileName);

		//since both video and audio are optional here, we try them in turn
		//if both were required, we could use just one check like this:
		//if (nativeOpen(Environment.getExternalStorageDirectory().getAbsolutePath() + "/mediafilename") != 0
		//    || nativeOpenVideo(mBackground) != 0
		//    || nativeOpenAudio(mAudioFrameBuffer, mAudioFrameBufferDataLength) != 0)
		//    {
		//    nativeClose();
		//    throw new RuntimeException();
		//    }

		mVideoOpened = nativeOpenVideo(mBackground) == 0;
		if (!mVideoOpened)
			nativeCloseVideo();

		if (!mVideoOpened && !mAudioOpened)
		{
			Log.i(TAG, "unable to open a stream, throwing RuntimeException");
			throw new RuntimeException();
		}
	}
*/
	private void loop(Thread threadInstance) {
		Log.i(TAG, "entering run()");

		boolean isAudioStarted = false; // lazy start audio


		final double overFps = 1000.0 / nativeGetFps();
		long videoTime = 0;
		long distortion = 0;

		mState = PlayState.RUN;
		try {
			while (mState == PlayState.RUN) {
				int dispatch = 0;
//			synchronized (mSurfaceHolder)
				{


					//video can be closed and reopened by setSurfaceSize()
					//we therefore call all video processing from within the synchronisation block
					//we also re-check mRun within the block to make sure video has been reopened successfully.
					//nativeDecodeFrameFromFile() groups audio & video - since no other thread accesses audio
					//it could theoretically be moved out of the synchronisation block - requiring a split
					//of nativeDecodeFrameFromFile()'s functionality
					//if (mRun)
					{

						dispatch = nativeDecodeFrameFromFile();
						if (dispatch == VIDEO_DATA_ID) {
							++videoFramesNb;
							videoTime = (long)(overFps * videoFramesNb);
							distortion = mAudioThread.getCurrentMS() - videoTime;
							if (distortion < 40)
							{
								if (distortion < -40)
								{
									try {
										threadInstance.sleep(-distortion / 2);
									} catch (InterruptedException e) {
										// TODO Auto-generated catch block
										e.printStackTrace();
									}
								}

								updateCanvas();
								/*
								Canvas canvas = null;
								synchronized (mSurfaceHolder) {
									try {
										canvas = mSurfaceHolder.lockCanvas(null);
										if (canvas != null) {
											nativeUpdateBitmap();
											canvas.drawBitmap(mBackground, 0, 0, null);
										}
									} catch (java.lang.NullPointerException e) {
										Log.e(TAG, "", e);
									} finally {
										if (canvas != null) {
											mSurfaceHolder.unlockCanvasAndPost(canvas);
										}
									}
								}
								*/
							}
							if (isAudioStarted == false)
							{
								isAudioStarted = true;
								//mAudioThread.start();
								mAudioThread.playResume();
							}
						}
					}
				}
			}
		} catch (Exception e) {
			mState = PlayState.STOPPED;
			Log.d(TAG, "", e);
		}

		// switch track to necessary state
		switch (mState) {
			case PAUSED:
				mAudioThread.playPause();
				break;
			case STOPPED:
				mAudioThread.finish();
				break;
		}

		Log.i(TAG, "leaving run()");
	}
	private void updateCanvas() {
		if (mSurfaceHolder != null) {
			Canvas canvas = null;
			synchronized (mSurfaceHolder) {
				try {
					canvas = mSurfaceHolder.lockCanvas(null);
					if (canvas != null) {

						synchronized (mBackground) {

							nativeUpdateBitmap();

							canvas.drawBitmap(mBackground, 0, 0, null);
						}
					}
				} catch (java.lang.NullPointerException e) {
					Log.e(TAG, "", e);
				} finally {
					if (canvas != null) {
						mSurfaceHolder.unlockCanvasAndPost(canvas);
					}
				}
			}
		}
	}
	public void finish()
	{
		//Log.i(TAG, "finish()");
		//mRun = false;
		mState = PlayState.STOPPED;

		//once again synchronise nativeClose() with
		//nativeCloseVideo()/nativeOpenVideo() from setSurfaceSize()
		if (mSurfaceHolder != null) {
			synchronized (mSurfaceHolder) {
				nativeClose();
			}
		}
	}
/*
	public void sendText(String aText)
	{
		int vis = View.INVISIBLE;
		if (aText.length() > 0) //isEmpty() supported from API level 9
		{
			vis = View.VISIBLE;
		}

		Message msg = mHandler.obtainMessage();
		Bundle b = new Bundle();
		b.putString("text", aText);
		b.putInt("viz", vis);
		msg.setData(b);
		mHandler.sendMessage(msg);
	}
*/

	public void setSurfaceSize(SurfaceHolder holder, int aWidth, int aHeight)
	{
		Bitmap bitmap = Bitmap.createScaledBitmap(mBackground, aWidth, aHeight, true);
		updateHolder(holder, bitmap);
		/*
		synchronized (mSurfaceHolder)
		{
			mBackground = Bitmap.createScaledBitmap(mBackground, aWidth, aHeight, true);
			if (mVideoOpened)
			{
				//free old references to mBackground and other video resources
				nativeCloseVideo();
				mVideoOpened = nativeOpenVideo(mBackground) == 0;
				if (!mVideoOpened)
				{
					nativeCloseVideo();
					Log.i(TAG, "unable to reopen video");
					finish();
				}
			}
		}
		*/
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
/*
	public void SwitchPause() {

		switch (getPlayState()) {
			case PAUSED :
				playResume();
				break;
			case RUN :
				playPause();
				break;
		}
	}
*/
	//////////////////////////////////////////////////////////////////////////////////////////////
	private Thread  mThread = null;
	private class ThreadInstance extends Thread {
		public void run() {
			loop(this);
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

	//native methods are described in jni/avjni.c
	private native int nativeOpenFromFile(String aMediaFile);
	private native void nativeClose();
	private native int nativeSetBitmapBuffer(Object aBitmapRef);
	private native int nativeOpenVideo(Object aBitmapRef);
	private native void nativeCloseVideo();
	//	private native int nativeOpenAudio(byte[] aAudioFrameBufferRef, int[] aAudioFrameBufferCountRef);
	//	private native void nativeCloseAudio();
	private native int nativeDecodeFrameFromFile(); //never touch the bitmap here
	//	private native int nativeFFmpegDecodeAudio();
	private native int nativeUpdateBitmap();
	private native double nativeGetFps();
};

