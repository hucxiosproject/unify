#include "JniHelperUnify.h"
#include <android/log.h>
#include <string.h>

#if 1
#define  LOG_TAG    "JniHelperUnify"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)
#else
#define  LOGD(...) 
#endif

#define JAVAVM    ce::JniHelper::getJavaVM()

using namespace std;

extern "C"
{

    //////////////////////////////////////////////////////////////////////////
    // java vm helper function
    //////////////////////////////////////////////////////////////////////////

    static pthread_key_t s_threadKey;

    static void detach_current_thread (void *env) {
        JAVAVM->DetachCurrentThread();
    }
    
    static bool getEnv(JNIEnv **env)
    {
        bool bRet = false;

        switch(JAVAVM->GetEnv((void**)env, JNI_VERSION_1_4))
        {
        case JNI_OK:
            bRet = true;
                break;
        case JNI_EDETACHED:
            pthread_key_create (&s_threadKey, detach_current_thread);
            if (JAVAVM->AttachCurrentThread(env, 0) < 0)
            {
                LOGD("%s", "Failed to get the environment using AttachCurrentThread()");
                break;
            }
            if (pthread_getspecific(s_threadKey) == NULL)
                pthread_setspecific(s_threadKey, env); 
            bRet = true;
            break;
        default:
            LOGD("%s", "Failed to get the environment using GetEnv()");
            break;
        }      

        return bRet;
    }


    static string jstring2string_(jstring jstr)
    {
        if (jstr == NULL)
        {
            return "";
        }

        JNIEnv *env = 0;

        if (! getEnv(&env))
        {
            return 0;
        }

        const char* chars = env->GetStringUTFChars(jstr, NULL);
        string ret(chars);
        env->ReleaseStringUTFChars(jstr, chars);

        return ret;
    }

}

NS_CE_BEGIN

JavaVM* JniHelper::m_psJavaVM = NULL;

std::map<std::string, jclass> cachedClassMap;

JavaVM* JniHelper::getJavaVM()
{
    return m_psJavaVM;
}

void JniHelper::setJavaVM(JavaVM *javaVM)
{
    m_psJavaVM = javaVM;

}

jclass JniHelper::getClassID(const char *className, JNIEnv *env)
{

    LOGD("JniHelper::getClassID: begin: %s", className);

    if (env == nullptr) {
        LOGD("jclass JniHelper::getClassID: env is empty", className);
        return 0;
    }


    if (cachedClassMap.count(className) > 0)
    {
        LOGD("JniHelper::getClassID: get class from cache: %s %d", className, cachedClassMap[className]);
        return cachedClassMap[className];
    }

    jclass ret = 0;

    ret = env->FindClass(className);

    if (! ret)
    {
        LOGD("jclass JniHelper::getClassID: Failed to find class of %s", className);
    }

    LOGD("JniHelper::getClassID end");


    return ret;
}

bool JniHelper::getStaticMethodInfo(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode)
{

    LOGD("JniHelper::getStaticMethodInfo class = %s method = %s", className, methodName);

    JNIEnv *pEnv = 0;

    getEnv(&pEnv);

    jclass classID = getClassID(className, pEnv);

    jmethodID methodID = pEnv->GetStaticMethodID(classID, methodName, paramCode);

    if (! methodID)
    {
        LOGD("JniHelper::getStaticMethodInfo: Failed to find static method id of %s", methodName);
        return false;
    }

    methodinfo.classID = classID;
    methodinfo.env = pEnv;
    methodinfo.methodID = methodID;

    return true;
}

bool JniHelper::getMethodInfo(JniMethodInfo &methodinfo, const char *className, const char *methodName, const char *paramCode)
{

    JNIEnv *pEnv = 0;

    getEnv(&pEnv);

    jclass classID = getClassID(className, pEnv);

    jmethodID methodID = pEnv->GetMethodID(classID, methodName, paramCode);

    if (!methodID)
    {
        LOGD("JniHelper::getMethodInfo: Failed to find method id of %s", methodName);
        return false;
    }

    methodinfo.classID = classID;
    methodinfo.env = pEnv;
    methodinfo.methodID = methodID;

    return true;
}

string JniHelper::jstring2string(jstring str)
{
    return jstring2string_(str);
}


std::pair<bool, int> JniHelper::jobject2int(JNIEnv* env, jobject obj)
{
    jmethodID methodId = NULL;
    jclass integer_cls = env->FindClass("java/lang/Integer");

    if (env->IsInstanceOf(obj, integer_cls))
    {
        methodId = env->GetMethodID(integer_cls, "intValue", "()I");
        int value = env->CallIntMethod(obj, methodId);
        return std::make_pair(true, value);
    }
    
    return std::make_pair(false, 0);
}


std::pair<bool, double> JniHelper::jobject2number(JNIEnv* env, jobject obj)
{
    jmethodID methodId = NULL;
    jclass float_cls = env->FindClass("java/lang/Float");
    jclass double_cls = env->FindClass("java/lang/Double");

    if (env->IsInstanceOf(obj, float_cls))
    {
        methodId = env->GetMethodID(float_cls, "doubleValue", "()D");
        double value = env->CallDoubleMethod(obj, methodId);
        return std::make_pair(true, value);
    }
    else if (env->IsInstanceOf(obj, double_cls))
    {
        methodId = env->GetMethodID(double_cls, "doubleValue", "()D");
        double value = env->CallDoubleMethod(obj, methodId);
        return std::make_pair(true, value);    
    }
    
    return std::make_pair(false, 0.0);
}


std::pair<bool, bool> JniHelper::jobject2bool(JNIEnv* env, jobject obj)
{
    jmethodID methodId = NULL;
    jclass cls = env->FindClass("java/lang/Boolean");

    if (env->IsInstanceOf(obj, cls))
    {
        methodId = env->GetMethodID(cls, "booleanValue", "()Z");
        bool value = env->CallBooleanMethod(obj, methodId);
        return std::make_pair(true, value);
    }
    
    return std::make_pair(false, false);
}


std::pair<bool, std::string> JniHelper::jobject2string(JNIEnv* env, jobject obj)
{
    jmethodID methodId = NULL;
    jclass cls = env->FindClass("java/lang/String");

    if (env->IsInstanceOf(obj, cls))
    {
        return std::make_pair(true, JniHelper::jstring2string((jstring)obj));
    }
    
    return std::make_pair(false, "");
}



jobject JniHelper::int2jobject(int arg)
{
    JniMethodInfo constructMI;
    if (JniHelper::getMethodInfo(constructMI, "java/lang/Integer", "<init>", "(I)V"))
    {
        jobject result = constructMI.env->NewObject(constructMI.classID, constructMI.methodID, arg);
        constructMI.env->DeleteLocalRef(constructMI.classID);
        return result;
    }
    return NULL;
}

jobject JniHelper::number2jobject(double arg)
{
    JniMethodInfo constructMI;
    if (JniHelper::getMethodInfo(constructMI, "java/lang/Double", "<init>", "(D)V"))
    {
        jobject result = constructMI.env->NewObject(constructMI.classID, constructMI.methodID, arg);
        constructMI.env->DeleteLocalRef(constructMI.classID);
        return result;
    }
    return NULL;
}

jobject JniHelper::bool2jobject(bool arg)
{
    JniMethodInfo constructMI;
    if (JniHelper::getMethodInfo(constructMI, "java/lang/Boolean", "<init>", "(Z)V"))
    {
        jobject result = constructMI.env->NewObject(constructMI.classID, constructMI.methodID, arg);
        constructMI.env->DeleteLocalRef(constructMI.classID);
        return result;
    }
    return NULL;
}

jobject JniHelper::string2jobject(const char* arg)
{
    JNIEnv* env = NULL;
    if (! getEnv(&env)) return NULL;
    return env->NewStringUTF(arg);
}

JNIEnv* JniHelper::getJNIEnv()
{
    JNIEnv* env = NULL;
    if (! getEnv(&env)) return NULL;
    return env;
}

void JniHelper::initCachedClassMap(JNIEnv *env)
{
    LOGD("JniHelper::initCachedClassMap begin");

    addClass("org/unify/helper/CELibHelper", env);
    addClass("org/unify/helper/JsHelper", env);
    addClass("org/unify/http/SimpleHttpClient", env);
    addClass("org/unify/http/CEHttpResponseHandler", env);
    addClass("org/unify/http/CEImageResponseHandler", env);
    addClass("org/unify/http/ImageHttpUtils", env);

    LOGD("JniHelper::initCachedClassMap end");
}

void JniHelper::addClass(const std::string& className, JNIEnv *env)
{
    LOGD("JniHelper::addClass: %s", className.c_str());

    jclass javaClass = getClassID(className.c_str(), env);

    javaClass = (jclass)env->NewGlobalRef(javaClass);

    LOGD("JniHelper::addClass result: %d", javaClass);

    cachedClassMap[className] = javaClass;
}

    //org.unify.http.SimpleHttpClient


NS_CE_END
