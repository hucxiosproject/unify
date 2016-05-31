package org.unify.http;

import android.app.Application;
import android.content.Context;
import android.graphics.Bitmap;
import android.os.Environment;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.widget.ImageView;

import com.loopj.android.http.AsyncHttpClient;
import com.loopj.android.http.JsonHttpResponseHandler;
import com.loopj.android.http.RequestHandle;
import com.loopj.android.http.RequestParams;
import com.loopj.android.http.ResponseHandlerInterface;
import com.nostra13.universalimageloader.cache.disc.impl.UnlimitedDiskCache;
import com.nostra13.universalimageloader.cache.disc.naming.Md5FileNameGenerator;
import com.nostra13.universalimageloader.cache.memory.impl.LruMemoryCache;
import com.nostra13.universalimageloader.core.DisplayImageOptions;
import com.nostra13.universalimageloader.core.ImageLoader;
import com.nostra13.universalimageloader.core.ImageLoaderConfiguration;
import com.nostra13.universalimageloader.core.assist.FailReason;
import com.nostra13.universalimageloader.core.assist.ImageScaleType;
import com.nostra13.universalimageloader.core.assist.QueueProcessingType;
import com.nostra13.universalimageloader.core.display.FadeInBitmapDisplayer;
import com.nostra13.universalimageloader.core.display.RoundedBitmapDisplayer;
import com.nostra13.universalimageloader.core.download.BaseImageDownloader;
import com.nostra13.universalimageloader.core.listener.SimpleImageLoadingListener;
import com.nostra13.universalimageloader.utils.StorageUtils;
import com.vocinno.utils.MethodsExtra;
import com.vocinno.utils.MethodsFile;
import com.vocinno.utils.apputils.instance.AppConstant;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;
import org.unify.helper.AppHelper;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import javax.activation.MimetypesFileTypeMap;

/**
 * Created by lvshun on 15/9/23.
 */
public class ImageHttpUtils {
    private static String LOG_TAG = "http_file_upload";
    public static ImageLoader imageLoader;
    private static MimetypesFileTypeMap mimeType = new MimetypesFileTypeMap();

    public static DisplayImageOptions options = new DisplayImageOptions.Builder()
            .delayBeforeLoading(0)//设置延时多少时间后开始下载
            .cacheInMemory(true)//设置下载的图片是否缓存在内存中
            .cacheOnDisk(true)// 设置下载的资源是否缓存在SD卡中
            .considerExifParams(false)// 是否考虑JPEG图像EXIF参数（旋转，翻转）
            .imageScaleType(ImageScaleType.IN_SAMPLE_POWER_OF_2)//设置图片以何种编码方式显示
            .bitmapConfig(Bitmap.Config.RGB_565) // 设置图片的解码类型
            .displayer(new RoundedBitmapDisplayer(0))//是否设置为圆角，弧度为多少
            .displayer(new FadeInBitmapDisplayer(1))//是否图片加载好后渐入的动画时间
            .build();

    public static void uploadFiles(String url, String json, final CEImageResponseHandler responseHandler) {
        System.out.println("test url:" + url);
        System.out.println("test json:" + json);
        try {
            JSONObject imagesjson = new JSONObject(json);
            JSONArray images = imagesjson.getJSONArray("files");
            JSONObject params = imagesjson.getJSONObject("params");

            Map<String, String> fileMap = new HashMap<String, String>();
            for (int i = 0; i < images.length(); i++) {
                String filePath = (String) (images.getJSONObject(i).get("path"));
                fileMap.put(filePath, filePath);
            }

            Map<String, String> paramMap = new HashMap<String, String>();
            Iterator<String> keys = params.keys();
            while (keys.hasNext()) {
                String key = keys.next();
                paramMap.put(key, params.getString(key));
            }

            postFiles(url, fileMap, paramMap, responseHandler);
        } catch (JSONException e) {
            e.printStackTrace();
        }
    }

    /**
     * 以post上传文件
     *
     * @param url   上传url
     * @param files 键为上传表单对应字段的name，值为文件路径
     */
    public static void postFiles(final String url, final Map<String, String> files, final Map<String, String> params, final CEImageResponseHandler responseHandler) {

        CEHttpResponseHandler.looperThread.handler.post(new Runnable() {

            @Override
            public void run() {
                AsyncHttpClient client = new AsyncHttpClient();

                JsonHttpResponseHandler jsonHttpResponseHandler = new JsonHttpResponseHandler() {

                    @Override
                    public void onSuccess(int statusCode,
                                          cz.msebera.android.httpclient.Header[] headers,
                                          String responseString) {
                        Log.d("test response 1: ", responseString);
                        responseHandler.onSuccess(statusCode, responseString);
                    }

                    @Override
                    public void onSuccess(int statusCode, cz.msebera.android.httpclient.Header[] headers, JSONObject response) {
                        Log.d("test response 2: ", response.toString());
                        responseHandler.onSuccess(statusCode, response.toString());
                    }

                    @Override
                    public void onSuccess(int statusCode,
                                          cz.msebera.android.httpclient.Header[] headers,
                                          JSONArray response) {
                        Log.d("test response 3: ", response.toString());
                        Log.d("test response 3: ", statusCode + "");
                        Log.d("test response 3: ", headers + "");
                        responseHandler.onSuccess(statusCode, response.toString());
                    }


                    @Override
                    public void onFailure(int statusCode,
                                          cz.msebera.android.httpclient.Header[] headers,
                                          Throwable throwable, JSONObject errorResponse) {
                        Log.d("test response 4: ", statusCode + "");
                        Log.d("test response 4: ", headers + "");
                        Log.d("test response 4: ", errorResponse + "");
                        Log.d("test response 4: ", throwable + "");

                        responseHandler.onSuccess(statusCode, "");
                    }

                    @Override
                    public void onFailure(int statusCode,
                                          cz.msebera.android.httpclient.Header[] headers,
                                          String responseString, Throwable throwable) {
                        responseHandler.onSuccess(statusCode, responseString);
                    }

                    @Override
                    public void onFailure(int statusCode,
                                          cz.msebera.android.httpclient.Header[] headers,
                                          Throwable throwable, JSONArray errorResponse) {
                    }

                };
                RequestParams fileParams = getFileParams(files);
                setParams(fileParams, params);

                executeUpload(client, url, fileParams, jsonHttpResponseHandler);
            }
        });


    }

    public static void httpPost(String url, Map<String, String> params) {
        AsyncHttpClient client = new AsyncHttpClient();
        JsonHttpResponseHandler jsonHttpResponseHandler = new JsonHttpResponseHandler() {
            @Override
            public void onSuccess(int statusCode, cz.msebera.android.httpclient.Header[] headers, JSONObject response) {
                Log.d(LOG_TAG, "Post请求成功" + response);
            }

            @Override
            public void onSuccess(int statusCode, cz.msebera.android.httpclient.Header[] headers, JSONArray timeline) {
                try {
                    JSONObject firstEvent = timeline.getJSONObject(0);
                    String tweetText = firstEvent.getString("text");
                    System.out.println(tweetText);
                } catch (JSONException e) {

                }
            }
        };

        RequestParams postParams = getParams(params);
        setParams(postParams, params);

        executeUpload(client, url, postParams, jsonHttpResponseHandler);
    }

    /**
     * 为上传文件准备参数
     *
     * @param files 键为上传表单对应字段的name，值为文件路径
     * @return
     */
    private static RequestParams getFileParams(Map<String, String> files) {
        RequestParams params = new RequestParams();
        Iterator<Map.Entry<String, String>> entries = files.entrySet().iterator();
        try {
            while (entries.hasNext()) {
                Map.Entry<String, String> entry = entries.next();
                File file = new File(entry.getValue());
                String currMineType = getFileType(file);
                params.put(entry.getKey(), file, currMineType, entry.getKey());
            }
        } catch (FileNotFoundException e) {
            Log.e(LOG_TAG, "file not found " + e.getMessage());
        }
        params.setHttpEntityIsRepeatable(true);
        params.setUseJsonStreamer(false);
        return params;
    }

    private static String getFileType(File file) {
        String currMimeType = mimeType.getContentType(file);
        try {
            if (currMimeType.toLowerCase().equals("application/octet-stream")) {
                // right now MimetypesFileTypeMap doesn't support amr format
                String path = file.getPath();
                String[] paths = path.split("\\.");
                String format = paths[paths.length - 1];
                if (format.toLowerCase().equals("amr")) {
                    return "audio/amr";
                }
                //Xperimental -- need to setup proper MIME type recognition
                if (format.toLowerCase().equals("mp3")) {
                    return "audio/mpeg";
                }
            }
        } catch (Exception e) {
            System.err.println("error when get mime type from file, file path is " + file.getPath());
            e.printStackTrace();
        }
        return "application/octet-stream";
    }

    /**
     * 准备参数
     *
     * @param params 键为上传表单对应字段的name，值为value
     * @return
     */
    private static RequestParams getParams(Map<String, String> params) {
        RequestParams postParams = new RequestParams();
        Iterator<Map.Entry<String, String>> entries = params.entrySet().iterator();
        while (entries.hasNext()) {
            Map.Entry<String, String> entry = entries.next();
            postParams.put(entry.getKey(), entry.getValue());
        }
        return postParams;
    }

    private static RequestParams setParams(RequestParams postParams, Map<String, String> params) {
        Iterator<Map.Entry<String, String>> entries = params.entrySet().iterator();
        while (entries.hasNext()) {
            Map.Entry<String, String> entry = entries.next();
            params.put(entry.getKey(), entry.getValue());
        }
        return postParams;
    }


    /**
     * 执行上传操作
     *
     * @param client
     * @param params
     * @param URL
     * @param responseHandler
     * @return
     */
    private static RequestHandle executeUpload(AsyncHttpClient client, String URL, RequestParams params, ResponseHandlerInterface responseHandler) {
        return client.post(null, URL, params, responseHandler);
    }

    public static void downloadImage(String url, ImageView imageView) {
        if (imageLoader == null) {
            Application app = AppHelper.getAppliction();
            initImageLoader(app, true);
        }
        imageLoader.displayImage(url, imageView, options);
    }

    /**
     * 下载图片
     *
     * @param url
     * @param handler
     */
    public static void downloadImage(String url, Boolean isCached, final CEImageResponseHandler handler) {

        Application app = AppHelper.getAppliction();
        if (app != null && imageLoader == null)
            initImageLoader(app, isCached);

        imageLoader.loadImage(url, options, new SimpleImageLoadingListener() {
            @Override
            public void onLoadingComplete(String imageUri, View view, Bitmap loadedImage) {
                JSONObject json = new JSONObject();
                try {
                    json.put("url", imageUri);
                    handler.onSuccess(200, "");
                } catch (JSONException e) {
                    e.printStackTrace();
                    handler.onFailure(500, e.getMessage());
                }
            }

            @Override
            public void onLoadingFailed(String imageUri, View view,
                                        FailReason failReason) {
                handler.onFailure(500, failReason.toString());
            }

        });
    }

    public static void initImageLoader(Context context, boolean cached) {

        DisplayImageOptions.Builder optionBuild = new DisplayImageOptions.Builder();
        optionBuild.cacheOnDisk(cached);
        DisplayImageOptions option = optionBuild.build();

        ImageLoaderConfiguration.Builder config = new ImageLoaderConfiguration.Builder(context);
        //File cacheDir = StorageUtils.getCacheDirectory(context);
        File cacheDir = new File(MethodsFile.getAutoFileDirectory() + "imgCache/");
        System.out.println(cacheDir.getAbsolutePath());
        config.diskCache(new UnlimitedDiskCache(cacheDir));
        config.diskCacheFileNameGenerator(new Md5FileNameGenerator());

        config.threadPoolSize(6) // default
                .threadPriority(Thread.NORM_PRIORITY - 1) // default
                .tasksProcessingOrder(QueueProcessingType.FIFO) // default
                .denyCacheImageMultipleSizesInMemory()
                .memoryCache(new LruMemoryCache(5 * 1024 * 1024))
                .memoryCacheSizePercentage(13) // default
                .diskCacheSize(100 * 1024 * 1024)
                .imageDownloader(new BaseImageDownloader(context)) // default
                .defaultDisplayImageOptions(option) // default
                .writeDebugLogs();
        imageLoader
                = ImageLoader.getInstance();
        imageLoader.init(config.build());
    }

}
