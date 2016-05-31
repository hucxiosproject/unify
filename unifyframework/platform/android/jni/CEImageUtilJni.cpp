#include <stdlib.h>
#include <android/log.h>
#include <string>
#include <map>
#include "common.h"
#include "JniHelperUnify.h"
#include "CEImageUtilJni.h"

static const char* HTTP_RESPONSE_HANDLER_CLASS_NAME = "org/unify/http/CEImageResponseHandler";
static const char* IMAGE_UTIL_CLASS_NAME = "org/unify/http/ImageHttpUtils";

static std::map<std::string, ce::CEImageUtilsAndroid*> s_images;

using namespace ce;

static std::string ptrString(void* p)
{
    char ptr[64] = { 0 };
    sprintf(ptr, "%p", p);
    return std::string(ptr);
}

extern "C" {

    JNIEXPORT void JNICALL Java_org_unify_http_CEImageResponseHandler_onJniSuccess(JNIEnv*  env, jclass thiz, jstring context, jint statusCode, jstring response) {
        std::string ctx = JniHelper::jstring2string(context);
        CEImageUtilsAndroid* image = s_images[ctx];
        if (image)
        {
            std::string s = JniHelper::jstring2string(response);
            image->callback(statusCode, s.c_str());
            s_images.erase(ctx);
        }
    }

    JNIEXPORT void JNICALL Java_org_unify_http_CEImageResponseHandler_onJniFailure(JNIEnv*  env, jclass thiz, jstring context, jint statusCode, jstring errorMessage) {
        std::string ctx = JniHelper::jstring2string(context);
        CEImageUtilsAndroid* image = s_images[ctx];
        if (image)
        {
            std::string s = JniHelper::jstring2string(errorMessage);
            image->callback(statusCode, s.c_str());
            s_images.erase(ctx);
        }
    }
}

static jobject createResponseHandler(const char* context)
{
    JniMethodInfo responseConstructMI;
    if (JniHelper::getMethodInfo(responseConstructMI, HTTP_RESPONSE_HANDLER_CLASS_NAME, "<init>", "()V")) 
    {
        JNIEnv* env = responseConstructMI.env;
        jobject responseObj = env->NewObject(responseConstructMI.classID, responseConstructMI.methodID);
        jfieldID contextFieldId = env->GetFieldID(responseConstructMI.classID, "context", "Ljava/lang/String;");
        jstring ctx = env->NewStringUTF(context);
        env->SetObjectField(responseObj, contextFieldId, ctx);
        env->DeleteLocalRef(ctx);
        //env->DeleteLocalRef(responseConstructMI.classID);//Class is global reference now
        return responseObj;
    }
    else
    {
        CCLOG("Can't find java method: %s (%s)", "<init>", HTTP_RESPONSE_HANDLER_CLASS_NAME);
    }
    return NULL;
}

void uploadImagesJni(ce::CEImageUtilsAndroid* imageUploader) {
    JniMethodInfo t;
    const char* methodName = "uploadFiles";
    if (JniHelper::getStaticMethodInfo(t, IMAGE_UTIL_CLASS_NAME, methodName, "(Ljava/lang/String;Ljava/lang/String;Lorg/unify/http/CEImageResponseHandler;)V")) {
  		std::string context = ptrString(imageUploader);
  		jobject responseHandler = createResponseHandler(context.c_str());
  		if (responseHandler) {
			s_images[context] = imageUploader;
			jstring url = t.env->NewStringUTF(imageUploader->getUrl().c_str());
			jstring data = t.env->NewStringUTF(imageUploader->getFiles().c_str());
			t.env->CallStaticVoidMethod(t.classID, t.methodID, url, data, responseHandler);
            //t.env->DeleteLocalRef(t.classID);//Class is global reference now
            t.env->DeleteLocalRef(url);
            t.env->DeleteLocalRef(data);
            t.env->DeleteLocalRef(responseHandler);
  		}
    } else {
  		CCLOG("Can't find java method: %s (%s)", methodName, IMAGE_UTIL_CLASS_NAME);
    }
}

void downloadImageJni(ce::CEImageUtilsAndroid* imageDownloader) {
    JniMethodInfo t;
    const char* methodName = "downloadImage";
    if (JniHelper::getStaticMethodInfo(t, IMAGE_UTIL_CLASS_NAME, methodName, "(Ljava/lang/String;Ljava/lang/Boolean;Lorg/unify/http/CEImageResponseHandler;)V")) {
        std::string context = ptrString(imageDownloader);
        jobject responseHandler = createResponseHandler(context.c_str());
        if (responseHandler) {
            s_images[context] = imageDownloader;
            jstring url = t.env->NewStringUTF(imageDownloader->getUrl().c_str());
            jobject cached = JniHelper::bool2jobject(imageDownloader->getCached());
            t.env->CallStaticVoidMethod(t.classID, t.methodID, url, cached, responseHandler);
            //t.env->DeleteLocalRef(t.classID);//Class is global reference now
            t.env->DeleteLocalRef(url);
            t.env->DeleteLocalRef(responseHandler);
        }
    } else {
        CCLOG("Can't find java method: %s (%s)", methodName, IMAGE_UTIL_CLASS_NAME);
    }
}
