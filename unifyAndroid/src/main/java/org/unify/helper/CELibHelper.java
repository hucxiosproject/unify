package org.unify.helper;

import android.app.Activity;
import android.content.Context;
import android.content.SharedPreferences;
import android.content.pm.ApplicationInfo;
import android.content.res.AssetManager;
import android.os.Build;
import android.os.Looper;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.WindowManager;

import com.vocinno.griefstore.util.js.MethodsJni;

import org.unify.http.CEHttpResponseHandler;

import java.util.Locale;

public class CELibHelper {
    // ===========================================================
    // Constants
    // ===========================================================
    private static final String PREFS_NAME = "CePrefsFile";

    // ===========================================================
    // Fields
    // ===========================================================

    private static AssetManager sAssetManager;
    private static String sPackageName;
    private static String sFileDirectory;
    private static Context sContext = null;

    // ===========================================================
    // Constructors
    // ===========================================================

    public static void init(final Context pContext) {
        final ApplicationInfo applicationInfo = pContext.getApplicationInfo();

        CELibHelper.initCachedClassMap(); //Must be called before everything!!!

        CEHttpResponseHandler.initLooper();

        CELibHelper.sContext = pContext;

        CELibHelper.sPackageName = applicationInfo.packageName;
        CELibHelper.sFileDirectory = pContext.getFilesDir().getAbsolutePath();
        CELibHelper.nativeSetApkPath(applicationInfo.sourceDir);

        CELibHelper.sAssetManager = pContext.getAssets();
    }

    // ===========================================================
    // Getter & Setter
    // ===========================================================

    // ===========================================================
    // Methods for/from SuperClass/Interfaces
    // ===========================================================

    // ===========================================================
    // Methods
    // ===========================================================

    private static native void initCachedClassMap();

    private static native void nativeSetApkPath(final String pApkPath);

    //private static native void initCallBackParams();

    public static String getPackageName() {
        return CELibHelper.sPackageName;
    }

    public static String getWritablePath() {
        return CELibHelper.sFileDirectory;
    }

    public static String getCurrentLanguage() {
        return Locale.getDefault().getLanguage();
    }

    public static String getDeviceModel() {
        return Build.MODEL;
    }

    public static AssetManager getAssetManager() {
        return CELibHelper.sAssetManager;
    }

    public static void terminateProcess() {
        android.os.Process.killProcess(android.os.Process.myPid());
    }

    public static int getDPI() {
        if (sContext != null) {
            DisplayMetrics metrics = new DisplayMetrics();
            WindowManager wm = ((Activity) sContext).getWindowManager();
            if (wm != null) {
                Display d = wm.getDefaultDisplay();
                if (d != null) {
                    d.getMetrics(metrics);
                    return (int) (metrics.density * 160.0f);
                }
            }
        }
        return -1;
    }

    // ===========================================================
    // Functions for CCUserDefault
    // ===========================================================

    public static boolean getBoolForKey(String key, boolean defaultValue) {
        SharedPreferences settings = ((Activity) sContext)
                .getSharedPreferences(CELibHelper.PREFS_NAME, 0);
        return settings.getBoolean(key, defaultValue);
    }

    public static int getIntegerForKey(String key, int defaultValue) {
        SharedPreferences settings = ((Activity) sContext)
                .getSharedPreferences(CELibHelper.PREFS_NAME, 0);
        return settings.getInt(key, defaultValue);
    }

    public static float getFloatForKey(String key, float defaultValue) {
        SharedPreferences settings = ((Activity) sContext)
                .getSharedPreferences(CELibHelper.PREFS_NAME, 0);
        return settings.getFloat(key, defaultValue);
    }

    public static double getDoubleForKey(String key, double defaultValue) {
        // SharedPreferences doesn't support saving double value
        SharedPreferences settings = ((Activity) sContext)
                .getSharedPreferences(CELibHelper.PREFS_NAME, 0);
        return settings.getFloat(key, (float) defaultValue);
    }

    public static String getStringForKey(String key, String defaultValue) {
        SharedPreferences settings = ((Activity) sContext)
                .getSharedPreferences(CELibHelper.PREFS_NAME, 0);
        return settings.getString(key, defaultValue);
    }

    public static void setBoolForKey(String key, boolean value) {
        SharedPreferences settings = ((Activity) sContext)
                .getSharedPreferences(CELibHelper.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putBoolean(key, value);
        editor.commit();
    }

    public static void setIntegerForKey(String key, int value) {
        SharedPreferences settings = ((Activity) sContext)
                .getSharedPreferences(CELibHelper.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putInt(key, value);
        editor.commit();
    }

    public static void setFloatForKey(String key, float value) {
        SharedPreferences settings = ((Activity) sContext)
                .getSharedPreferences(CELibHelper.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putFloat(key, value);
        editor.commit();
    }

    public static void setDoubleForKey(String key, double value) {
        // SharedPreferences doesn't support recording double value
        SharedPreferences settings = ((Activity) sContext)
                .getSharedPreferences(CELibHelper.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putFloat(key, (float) value);
        editor.commit();
    }

    public static void setStringForKey(String key, String value) {
        SharedPreferences settings = ((Activity) sContext)
                .getSharedPreferences(CELibHelper.PREFS_NAME, 0);
        SharedPreferences.Editor editor = settings.edit();
        editor.putString(key, value);
        editor.commit();
    }


    public static void notify(final String name, final String className, final Object data) {
        Log.d("unify", "===unify:" + name + "," + className + "," + (data == null ? "null" : data.toString()));

        ((Activity) (sContext)).runOnUiThread(new Runnable() {
            @Override
            public void run() {
                MethodsJni.notificationCallBack(name, className, data);
            }
        });

    }
}
