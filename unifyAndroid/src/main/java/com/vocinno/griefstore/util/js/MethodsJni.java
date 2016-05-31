package com.vocinno.griefstore.util.js;

import android.app.Activity;
import android.content.Context;
import android.os.Looper;
import android.util.Log;


import com.vocinno.griefstore.util.NotificationReceiverInterface;

import org.unify.helper.CELibHelper;
import org.unify.helper.JsHelper;

import java.util.ArrayList;
import java.util.List;
import java.util.concurrent.ConcurrentHashMap;

public final class MethodsJni {
    //key： activityId activity唯一标识 ， value：调用方法的activity
    public static ConcurrentHashMap<String, NotificationReceiverInterface> mNotificationMap = new ConcurrentHashMap<>();
    //key： activityId activity唯一标识 ， value：调用方法的activity
    public static ConcurrentHashMap<String, List<String>> mNotificationList = new ConcurrentHashMap<>();

    /**
     * 在使用jni之前调用，用于初始化jni
     *
     * @param act
     */
    public static void initJniLibrary(Context act) {
        System.loadLibrary("gnustl_shared");
        System.loadLibrary("unifylib");
        CELibHelper.init(act);
        JsHelper.executeJsString("require('main');");
    }

    /**
     * 调用JS的全局方法
     *
     * @param functionName 函数名
     * @param args         参数(对象数组)
     */
    public static String callJSGlobalFun(final String functionName,
                                         final Object... args) {
        return (String) JsHelper.callGlobalFunction(functionName, args);
    }

    /**
     * 调用方法
     *
     * @param proxyName
     * @param functionName
     * @param args
     */
    public static Object callProxyFun(String proxyName, String functionName,
                                      Object... args) {
        return JsHelper.callProxy(proxyName, functionName, args);
    }

    /**
     * 通知回调
     *
     * @param name
     * @param activityId
     * @param data
     */
    public static void notificationCallBack(final String name,
                                            final String activityId, final Object data) {

        if (Looper.myLooper() != Looper.getMainLooper())
        {
            Log.e("err", "in notificationCallBack error: Looper.myLooper() != Looper.getMainLooper()");
        }


        NotificationReceiverInterface receiver = mNotificationMap.get(activityId);

        if (receiver != null)
        {
            receiver.notify(name, data);
        } else {
            Log.e("err", "the activity that id is" + activityId + "have  destroyed , or this notification have been removed!");
        }


    }

    /**
     * 执行JS代码，参数为JS代码
     *
     * @param source
     * @return
     */
    public static int executeJsString(final String source) {
        return JsHelper.executeJsString(source);
    }

    /**
     * 执行js文件，参数为js的完整路径(apk和项目里的文件不能调用)
     *
     * @param jsFile
     * @return
     */
    public static int executeJsFile(final String jsFile) {
        return executeJsFile(jsFile);
    }

    /**
     * 添加通知
     *
     * @param name
     * @param activityId
     */
    public static void addNotificationObserver(final String name,
                                               final String activityId, NotificationReceiverInterface receiver) {
        if (!mNotificationMap.containsKey(activityId)) {
            mNotificationMap.put(activityId, receiver);
            mNotificationList.put(activityId, new ArrayList<String>());
        }
        mNotificationList.get(activityId).add(name);
        JsHelper.addNotificationObserver(name, activityId);
    }

    /**
     * 移除通知
     *
     * @param name
     * @param activityId
     */
    public static void removeNotificationObserver(final String name,
                                                  final String activityId) {
        JsHelper.removeNotificationObserver(name, activityId);
    }

    /**
     * 移除所有通知
     *
     * @param activityId
     */
    public static void removeAllNotifications(final String activityId) {
        mNotificationMap.remove(activityId);
        //
        List<String> list = mNotificationList.get(activityId);
        if (list != null) {
            for (int i = 0; i < list.size(); i++) {
                //jason写的removeAllNotifications()方法不管用，所以我们暂时这么写
                JsHelper.removeNotificationObserver(list.get(i), activityId);
            }
            mNotificationList.remove(activityId);
        }
        JsHelper.removeAllNotifications(activityId);
    }

}
