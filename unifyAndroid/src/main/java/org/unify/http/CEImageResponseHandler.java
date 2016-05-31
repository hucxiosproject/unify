package org.unify.http;

import com.loopj.android.http.AsyncHttpResponseHandler;

public class CEImageResponseHandler extends AsyncHttpResponseHandler {
    public String context;

    public CEImageResponseHandler()
    {
        super(CEHttpResponseHandler.getInnerLooper());
    }

    private native void onJniSuccess(final String context, final int statusCode, final String responseString);

    private native void onJniFailure(final String context, final int statusCode, final String errorMessage);

    @Override
    public void onFailure(int statusCode,
                          cz.msebera.android.httpclient.Header[] arg1, byte[] responseData,
                          Throwable arg3) {
        String responseString = new String(responseData);
        this.onJniFailure(this.context, statusCode, responseString);
    }

    @Override
    public void onSuccess(int statusCode,
                          cz.msebera.android.httpclient.Header[] arg1, byte[] responseData) {
        String responseString = new String(responseData);
        this.onJniSuccess(this.context, statusCode, responseString);
    }

    public void onSuccess(int statusCode, String response) {
        this.onJniSuccess(this.context, statusCode, response);
    }

    public void onFailure(int statusCode, String response) {
        this.onJniFailure(this.context, statusCode, response);
    }
}
