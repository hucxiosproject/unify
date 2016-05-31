#include <stdlib.h>
#include <android/log.h>
#include <string>
#include "JniHelperUnify.h"
#include "CELibHelper.h"


#define  LOG_TAG    "CELibHelper"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

#define  CLASS_NAME "org/unify/helper/CELibHelper"


using namespace ce;
using namespace std;

string g_apkPath;

extern "C" {

    JNIEXPORT void JNICALL Java_org_unify_helper_CELibHelper_initCachedClassMap(JNIEnv*  env, jclass thiz) {
        LOGD("Java_org_unify_helper_CELibHelper_initCachedClassMap begin");
        JniHelper::initCachedClassMap(env);
        LOGD("Java_org_unify_helper_CELibHelper_initCachedClassMap end");

    }
    JNIEXPORT void JNICALL Java_org_unify_helper_CELibHelper_nativeSetApkPath(JNIEnv*  env, jclass thiz, jstring apkPath) {
        g_apkPath = JniHelper::jstring2string(apkPath);
    }

    JNIEXPORT jint JNI_OnLoad(JavaVM *vm, void *reserved) {

        LOGD(">>>>> unify JniHelper::setJavaVM(vm) begin");
        ce::JniHelper::setJavaVM(vm);
        LOGD(">>>>> unify JniHelper::setJavaVM(vm) end");
        return JNI_VERSION_1_4;
    }
}

const char * getApkPath() {
    return g_apkPath.c_str();
}

void terminateProcessJNI() {
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "terminateProcess", "()V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        //t.env->DeleteLocalRef(t.classID);//Class is global reference now
    }
}

std::string getPackageNameJNI() {
    JniMethodInfo t;
    std::string ret("");

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getPackageName", "()Ljava/lang/String;")) {
        jstring str = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
        //t.env->DeleteLocalRef(t.classID); //Class is global reference now
        ret = JniHelper::jstring2string(str);
        t.env->DeleteLocalRef(str);
    }
    return ret;
}

std::string getFileDirectoryJNI() {
    JniMethodInfo t;
    std::string ret("");

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getWritablePath", "()Ljava/lang/String;")) {
        jstring str = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
        //t.env->DeleteLocalRef(t.classID); //Class is global reference now
        ret = JniHelper::jstring2string(str);
        t.env->DeleteLocalRef(str);
    }
    
    return ret;
}

std::string getCurrentLanguageJNI() {
    JniMethodInfo t;
    std::string ret("");
    
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getCurrentLanguage", "()Ljava/lang/String;")) {
        jstring str = (jstring)t.env->CallStaticObjectMethod(t.classID, t.methodID);
        //t.env->DeleteLocalRef(t.classID); //Class is global reference now
        ret = JniHelper::jstring2string(str);
        t.env->DeleteLocalRef(str);
    }

    return ret;
}

void enableAccelerometerJNI() {
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "enableAccelerometer", "()V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        //t.env->DeleteLocalRef(t.classID); //Class is global reference now
    }
}

void setAccelerometerIntervalJNI(float interval) {
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setAccelerometerInterval", "(F)V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID, interval);
        //t.env->DeleteLocalRef(t.classID); //Class is global reference now
    }
}

void disableAccelerometerJNI() {
    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "disableAccelerometer", "()V")) {
        t.env->CallStaticVoidMethod(t.classID, t.methodID);
        //t.env->DeleteLocalRef(t.classID); //Class is global reference now
    }
}

bool inDirectoryExistsJNI(const char* path) {
    if (!path) return false;

    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "inDirectoryExists", "(Ljava/lang/String;)Z")) {
        jstring stringArg1;

        stringArg1 = t.env->NewStringUTF(path);
        jboolean ret = t.env->CallStaticBooleanMethod(t.classID, t.methodID, stringArg1);
        t.env->DeleteLocalRef(stringArg1);
        //t.env->DeleteLocalRef(t.classID); //Class is global reference now
        return ret;
    }

    return false;
}


// functions for CCUserDefault

namespace unify {
    bool getBoolForKeyJNI(const char *pKey, bool defaultValue) {
        JniMethodInfo t;

        if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getBoolForKey",
                                           "(Ljava/lang/String;Z)Z")) {
            jstring stringArg = t.env->NewStringUTF(pKey);
            jboolean ret = t.env->CallStaticBooleanMethod(t.classID, t.methodID, stringArg,
                                                          defaultValue);

            //t.env->DeleteLocalRef(t.classID); //Class is global reference now
            t.env->DeleteLocalRef(stringArg);

            return ret;
        }

        return defaultValue;
    }

    int getIntegerForKeyJNI(const char *pKey, int defaultValue) {
        JniMethodInfo t;

        if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getIntegerForKey",
                                           "(Ljava/lang/String;I)I")) {
            jstring stringArg = t.env->NewStringUTF(pKey);
            jint ret = t.env->CallStaticIntMethod(t.classID, t.methodID, stringArg, defaultValue);

            //t.env->DeleteLocalRef(t.classID); //Class is global reference now
            t.env->DeleteLocalRef(stringArg);

            return ret;
        }

        return defaultValue;
    }

    float getFloatForKeyJNI(const char *pKey, float defaultValue) {
        JniMethodInfo t;

        if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getFloatForKey",
                                           "(Ljava/lang/String;F)F")) {
            jstring stringArg = t.env->NewStringUTF(pKey);
            jfloat ret = t.env->CallStaticFloatMethod(t.classID, t.methodID, stringArg,
                                                      defaultValue);

            //t.env->DeleteLocalRef(t.classID); //Class is global reference now
            t.env->DeleteLocalRef(stringArg);

            return ret;
        }

        return defaultValue;
    }

    double getDoubleForKeyJNI(const char *pKey, double defaultValue) {
        JniMethodInfo t;

        if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getDoubleForKey",
                                           "(Ljava/lang/String;D)D")) {
            jstring stringArg = t.env->NewStringUTF(pKey);
            jdouble ret = t.env->CallStaticDoubleMethod(t.classID, t.methodID, stringArg);

            //t.env->DeleteLocalRef(t.classID); //Class is global reference now
            t.env->DeleteLocalRef(stringArg);

            return ret;
        }

        return defaultValue;
    }

    std::string getStringForKeyJNI(const char *pKey, const char *defaultValue) {
        JniMethodInfo t;
        std::string ret("");

        if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "getStringForKey",
                                           "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/String;")) {
            jstring stringArg1 = t.env->NewStringUTF(pKey);
            jstring stringArg2 = t.env->NewStringUTF(defaultValue);
            jstring str = (jstring) t.env->CallStaticObjectMethod(t.classID, t.methodID, stringArg1,
                                                                  stringArg2);
            ret = JniHelper::jstring2string(str);

            //t.env->DeleteLocalRef(t.classID); //Class is global reference now
            t.env->DeleteLocalRef(stringArg1);
            t.env->DeleteLocalRef(stringArg2);
            t.env->DeleteLocalRef(str);

            return ret;
        }

        return defaultValue;
    }

    void setBoolForKeyJNI(const char *pKey, bool value) {
        JniMethodInfo t;

        if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setBoolForKey",
                                           "(Ljava/lang/String;Z)V")) {
            jstring stringArg = t.env->NewStringUTF(pKey);
            t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg, value);

            //t.env->DeleteLocalRef(t.classID); //Class is global reference now
            t.env->DeleteLocalRef(stringArg);
        }
    }

    void setIntegerForKeyJNI(const char *pKey, int value) {
        JniMethodInfo t;

        if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setIntegerForKey",
                                           "(Ljava/lang/String;I)V")) {
            jstring stringArg = t.env->NewStringUTF(pKey);
            t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg, value);

            //t.env->DeleteLocalRef(t.classID); //Class is global reference now
            t.env->DeleteLocalRef(stringArg);
        }
    }

    void setFloatForKeyJNI(const char *pKey, float value) {
        JniMethodInfo t;

        if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setFloatForKey",
                                           "(Ljava/lang/String;F)V")) {
            jstring stringArg = t.env->NewStringUTF(pKey);
            t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg, value);

            //t.env->DeleteLocalRef(t.classID); //Class is global reference now
            t.env->DeleteLocalRef(stringArg);
        }
    }

    void setDoubleForKeyJNI(const char *pKey, double value) {
        JniMethodInfo t;

        if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setDoubleForKey",
                                           "(Ljava/lang/String;D)V")) {
            jstring stringArg = t.env->NewStringUTF(pKey);
            t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg, value);

            //t.env->DeleteLocalRef(t.classID); //Class is global reference now
            t.env->DeleteLocalRef(stringArg);
        }
    }

    void setStringForKeyJNI(const char *pKey, const char *value) {
        JniMethodInfo t;

        if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "setStringForKey",
                                           "(Ljava/lang/String;Ljava/lang/String;)V")) {
            jstring stringArg1 = t.env->NewStringUTF(pKey);
            jstring stringArg2 = t.env->NewStringUTF(value);
            t.env->CallStaticVoidMethod(t.classID, t.methodID, stringArg1, stringArg2);

            //t.env->DeleteLocalRef(t.classID); //Class is global reference now
            t.env->DeleteLocalRef(stringArg1);
            t.env->DeleteLocalRef(stringArg2);
        }
    }

} //namespace unify



void notifyToJni(const char* noticeName, const char* className, const char* data)
{
    JniMethodInfo t;
    if (JniHelper::getStaticMethodInfo(t, CLASS_NAME, "notify", "(Ljava/lang/String;Ljava/lang/String;Ljava/lang/Object;)V")) {
        jstring arg1 = t.env->NewStringUTF(noticeName);
        jstring arg2 = t.env->NewStringUTF(className);
        jstring arg3 = t.env->NewStringUTF(data);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, arg1, arg2, arg3);

        //t.env->DeleteLocalRef(t.classID); //Class is global reference now
        t.env->DeleteLocalRef(arg1);
        t.env->DeleteLocalRef(arg2);
        t.env->DeleteLocalRef(arg3);
    }
}
