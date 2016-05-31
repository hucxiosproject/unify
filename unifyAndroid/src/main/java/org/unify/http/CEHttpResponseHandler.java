package org.unify.http;

import android.os.Handler;
import android.os.Looper;

import com.loopj.android.http.AsyncHttpResponseHandler;

public class CEHttpResponseHandler extends AsyncHttpResponseHandler {
    public String context;


    public static class LooperThread extends Thread {

        public Handler handler;

        @Override
        public void run() {
            Looper.prepare();
            synchronized (innerLooperLock) {
                innerLooper = Looper.myLooper();
            }
            handler = new Handler();
            Looper.loop();
        }
    }

    public static LooperThread looperThread = null;
    public static final Object looperLock = new Object();

    public static Looper innerLooper = null;
    public static final Object innerLooperLock = new Object();

    public static void initLooper()
    {
        synchronized (looperLock)
        {
            if (looperThread == null)
            {
                looperThread = new LooperThread();
                looperThread.start();
            }
        }
    }

    public static Looper getInnerLooper()
    {
        Looper result = null;
        synchronized (innerLooperLock) {
            if (innerLooper != null)
            {
                 result = innerLooper;
            }
        }
        return result;
    }

    public CEHttpResponseHandler()
    {
        super(innerLooper);
    }

    private native void onJniSuccess(final String context, final int statusCode, final String responseString);

    private native void onJniFailure(final String context, final int statusCode, final String errorMessage);

    @Override
    public void onFailure(int statusCode,
                          cz.msebera.android.httpclient.Header[] arg1, byte[] responseData,
                          Throwable arg3) {
        String responseString = null;
        try {
            responseString = new String(responseData);
            System.out.println(statusCode);
            System.out.println(responseString);
        } catch (Exception s) {
        }

        if (statusCode == 0) {
            statusCode = -1;//《解忧杂货店》项目中 需要把网络错误的code统一为 －1
        }
        this.onJniFailure(this.context, statusCode, responseString);
    }

    @Override
    public void onSuccess(int statusCode,
                          cz.msebera.android.httpclient.Header[] arg1, byte[] responseData) {
        String responseString = new String(responseData);
        this.onJniSuccess(this.context, statusCode, responseString);
    }
}
