package org.unify.http;

import com.loopj.android.http.AsyncHttpClient;
import com.loopj.android.http.AsyncHttpResponseHandler;
import com.loopj.android.http.RequestParams;

import org.json.JSONException;
import org.json.JSONObject;
import org.unify.helper.AppHelper;

import java.util.Iterator;

import cz.msebera.android.httpclient.entity.ContentType;

public class SimpleHttpClient {

    private static AsyncHttpClient _client = new AsyncHttpClient();

    public static void get(String url, String data, AsyncHttpResponseHandler responseHandler) {
        _client.get(url, responseHandler);
    }

    public static void delete(String url, String data, AsyncHttpResponseHandler responseHandler) {
        _client.delete(url, responseHandler);
    }

    public static void post(String url, String postData, AsyncHttpResponseHandler responseHandler) {
        postWithJSON(url, postData, responseHandler);
    }

    private static void postWithForm(final String url, String postData, AsyncHttpResponseHandler responseHandler) {
        RequestParams params = getParamsByJsonString(postData);
        _client.addHeader("Content-Type", "application/x-www-form-urlencoded;charset=UTF-8");
        _client.post(url, params, responseHandler);
    }

    private static void postWithJSON(final String url, String postData, AsyncHttpResponseHandler responseHandler) {
        _client.addHeader("Content-Type", "application/json;charset=UTF-8");
        try {
            System.out.println("test post url:" + url + "   " + postData);
            ContentType type = ContentType.APPLICATION_JSON;
            cz.msebera.android.httpclient.entity.StringEntity entity = new cz.msebera.android.httpclient.entity.StringEntity(postData, type);
            System.out.println(entity);
            _client.post(AppHelper.getAppliction(), url, entity, "application/json;charset=UTF-8", responseHandler);
        } catch (Exception e) {
            System.out.println("error when post with json, json data is " + postData);
            e.printStackTrace();
        }

    }

    public static void put(final String url, String putData, AsyncHttpResponseHandler responseHandler) {
        putWithJSON(url, putData, responseHandler);
    }

    private static void putWithForm(final String url, String postData, AsyncHttpResponseHandler responseHandler) {
        RequestParams params = getParamsByJsonString(postData);
        _client.addHeader("Content-Type", "application/x-www-form-urlencoded;charset=UTF-8");
        _client.put(url, params, responseHandler);
    }

    private static void putWithJSON(final String url, String postData, AsyncHttpResponseHandler responseHandler) {
        try {
            ContentType type = ContentType.APPLICATION_JSON;
            cz.msebera.android.httpclient.entity.StringEntity entity = new cz.msebera.android.httpclient.entity.StringEntity(postData, type);
            System.out.println(entity);
            _client.put(null, url, entity, "application/json;charset=UTF-8", responseHandler);
        } catch (Exception e) {
            System.out.println("error when put with json, json data is " + postData);
            e.printStackTrace();
        }
    }

    private static RequestParams getParamsByJsonString(String data) {
        RequestParams params = new RequestParams();
        try {
            JSONObject jsonObject = new JSONObject(data);
            Iterator<?> it = jsonObject.keys();
            String key = "";
            String value = "";
            while (it.hasNext()) {
                key = it.next().toString();
                value = jsonObject.getString(key);
                params.add(key, value);
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return params;
    }

    private static JSONObject getJsonByJsonString(String data) {
        JSONObject jsonObj = new JSONObject();
        try {
            JSONObject jsonObject = new JSONObject(data);
            Iterator<?> it = jsonObject.keys();
            String key = "";
            String value = "";
            while (it.hasNext()) {
                key = it.next().toString();
                value = jsonObject.getString(key);
                jsonObj.put(key, value);
            }
        } catch (JSONException e) {
            e.printStackTrace();
        }
        return jsonObj;
    }
}
