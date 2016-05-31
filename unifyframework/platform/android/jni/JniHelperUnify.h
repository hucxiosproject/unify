#ifndef __ANDROID_JNI_HELPER_UNIFY_H__
#define __ANDROID_JNI_HELPER_UNIFY_H__

#include <string>
#include <jni.h>
#include "include/CCPlatformMacrosUnify.h"
#include <map>

NS_CE_BEGIN

    extern std::map<std::string, jclass> cachedClassMap;

typedef struct JniMethodInfo_
{
    JNIEnv *    env;
    jclass      classID;
    jmethodID   methodID;
} JniMethodInfo;


class CC_DLL JniHelper
{
public:
    static JavaVM* getJavaVM();

    static void setJavaVM(JavaVM *javaVM);

    static void initCachedClassMap(JNIEnv *env);

    static jclass getClassID(const char *className, JNIEnv *env);

    static bool getStaticMethodInfo(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode);

    static bool getMethodInfo(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode);

    static std::string jstring2string(jstring str);

    // convert jobject to primitive value
    // return std::pair<is success, result>
    static std::pair<bool, int> jobject2int(JNIEnv* env, jobject obj);
    static std::pair<bool, double> jobject2number(JNIEnv* env, jobject obj);
    static std::pair<bool, bool> jobject2bool(JNIEnv* env, jobject obj);
    static std::pair<bool, std::string> jobject2string(JNIEnv* env, jobject obj);

    // convert primitive value to jobject
    static jobject int2jobject(int arg);
    static jobject number2jobject(double arg);
    static jobject bool2jobject(bool arg);
    static jobject string2jobject(const char* arg);

    static JNIEnv* getJNIEnv();

    static void addClass(const std::string& className, JNIEnv *env);

private:
    static JavaVM *m_psJavaVM;
    static jobject *jobj;


};

NS_CE_END

#endif // __ANDROID_JNI_HELPER_UNIFY_H__
