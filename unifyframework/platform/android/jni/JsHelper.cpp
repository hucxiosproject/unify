#include <stdlib.h>
#include <jni.h>
#include <android/log.h>
#include <string>
#include <vector>
#include "JniHelperUnify.h"
#include "CELibHelper.h"
#include "include/NotificationCenter.h"

#define  LOG_TAG    "JsHelper"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)


#include "CEJsEngine.h"

using namespace ce;



static CEJsValue jobject2jsvalue(JNIEnv* env, jobject obj)
{
    std::pair<bool, std::string> ret_bs = JniHelper::jobject2string(env, obj);
    if (ret_bs.first) {
        return JS_STRING(ret_bs.second.c_str());
    }

    std::pair<bool, int> ret_is = JniHelper::jobject2int(env, obj);
    if (ret_is.first) {
        return JS_NUMBER(ret_is.second);
    }

    std::pair<bool, double> ret_ns = JniHelper::jobject2number(env, obj);
    if (ret_ns.first) {
        return JS_NUMBER(ret_ns.second);
    }

    std::pair<bool, bool> ret_bb = JniHelper::jobject2bool(env, obj);
    if (ret_bb.first) {
        return JS_BOOL(ret_bb.second);
    } 

    return JS_NULL;  
}


static jobject jsvalue2jobject(const CEJsValue& value)
{
    switch (value.getType())
    {
        case JsValueTypeNumber:
            return JniHelper::number2jobject(value.numberValue());

        case JsValueTypeBool:
            return JniHelper::bool2jobject(value.boolValue());

        case JsValueTypeString:
            return JniHelper::string2jobject(value.cstringValue());

        case JsValueTypeArray:
        case JsValueTypeMap:
            return JniHelper::string2jobject(value.toString().c_str());

        default:
            break;
    }

    return NULL;
}



extern "C" {

    JNIEXPORT int JNICALL Java_org_unify_helper_JsHelper_executeJsString(JNIEnv*  env, jclass clazz, jstring source) {
        std::string s = JniHelper::jstring2string(source);
        LOGD("Java_org_unify_helper_JsHelper_executeJsString begin");
        int result = CEJsEngine::getInstance()->executeString(s.c_str());
        LOGD("Java_org_unify_helper_JsHelper_executeJsString end");
        return result;
    }


    JNIEXPORT int JNICALL Java_org_unify_helper_JsHelper_executeJsFile(JNIEnv*  env, jclass clazz, jstring jsFile) {
        //Xperimental
        //std::string s = JniHelper::jstring2string(jsFile);
        //return CEJsEngine::getInstance()->executeJsFile(s.c_str());
        return 0;
    }

    
    JNIEXPORT void JNICALL Java_org_unify_helper_JsHelper_addNotificationObserver(JNIEnv*  env, jclass clazz, jstring name, jstring className) {
        std::string noticeName = JniHelper::jstring2string(name);
        std::string observerClassName = JniHelper::jstring2string(className);
        NotificationCenter::getInstance()->addObserverForJni(noticeName.c_str(), observerClassName.c_str());
    }

    JNIEXPORT void JNICALL Java_org_unify_helper_JsHelper_addNotificationObservers(JNIEnv*  env, jclass clazz, jobjectArray names, jstring className) {
        std::string observerClassName = JniHelper::jstring2string(className);

        std::vector<std::string> v;
        jsize count = env->GetArrayLength(names);
        for (int i = 0; i < count; ++i)
        {
            jstring str = (jstring)(env->GetObjectArrayElement(names, i));
            v.push_back(JniHelper::jstring2string(str));
        }

        NotificationCenter::getInstance()->addObserversForJni(v, observerClassName.c_str());
    }
    

    JNIEXPORT void JNICALL Java_org_unify_helper_JsHelper_removeNotificationObserver(JNIEnv*  env, jclass clazz, jstring name, jstring className) {
        std::string arg1 = JniHelper::jstring2string(name);
        std::string arg2 = JniHelper::jstring2string(className);
        NotificationCenter::getInstance()->removeObserverForJni(arg1.c_str(), arg2.c_str());
    }


    JNIEXPORT void JNICALL Java_org_unify_helper_JsHelper_removeAllNotifications(JNIEnv*  env, jclass clazz, jstring className) {
        std::string arg = JniHelper::jstring2string(className);
        NotificationCenter::getInstance()->removeAllObserversForClassJni(arg.c_str());
    }


    JNIEXPORT jobject JNICALL Java_org_unify_helper_JsHelper_callGlobalFunction(JNIEnv*  env, jclass clazz, jstring functionName, jobjectArray args) {
        std::string fn = JniHelper::jstring2string(functionName);

        std::vector<CEJsValue> v;
        jsize count = env->GetArrayLength(args);
        for (int i = 0; i < count; ++i)
        {
            jobject element = env->GetObjectArrayElement(args, i);
            v.push_back(jobject2jsvalue(env, element));
        }

        CEJsValue result = CEJsEngine::getInstance()->callGlobalFunction(fn.c_str(), v);
        return jsvalue2jobject(result);
    }


    JNIEXPORT jobject JNICALL Java_org_unify_helper_JsHelper_callProxy(JNIEnv* env, jclass clazz, jstring proxyName, jstring functionName, jobjectArray args) {
        std::string pn = JniHelper::jstring2string(proxyName);
        std::string fn = JniHelper::jstring2string(functionName);

        std::vector<CEJsValue> v;
        jsize count = env->GetArrayLength(args);
        for (int i = 0; i < count; ++i)
        {
            jobject element = env->GetObjectArrayElement(args, i);
            v.push_back(jobject2jsvalue(env, element));
        }

        CEJsValue result = CEJsEngine::getInstance()->callProxy(pn.c_str(), fn.c_str(), v);   
        return jsvalue2jobject(result);
    }


JNIEXPORT void JNICALL Java_org_unify_helper_JsHelper_DebuggerDetach(JNIEnv* env, jclass clazz) {
#ifdef DUK_OPT_DEBUGGER_SUPPORT
    CEJsEngine::getInstance()->DetachDebugger();
    #endif
}

JNIEXPORT void JNICALL Java_org_unify_helper_JsHelper_DebuggerPause(JNIEnv* env, jclass clazz) {
#ifdef DUK_OPT_DEBUGGER_SUPPORT
    CEJsEngine::getInstance()->PauseDebugger();
    #endif
}

JNIEXPORT void JNICALL Java_org_unify_helper_JsHelper_DebuggerResume(JNIEnv* env, jclass clazz) {
#ifdef DUK_OPT_DEBUGGER_SUPPORT
    CEJsEngine::getInstance()->ResumeDebugger();
    #endif
}

JNIEXPORT void JNICALL Java_org_unify_helper_JsHelper_DebuggerAddBreakpoint(JNIEnv* env, jclass clazz, jstring fileName, jint line) {
#ifdef DUK_OPT_DEBUGGER_SUPPORT
    std::string fileNameStr = JniHelper::jstring2string(fileName);

    CEJsEngine::getInstance()->AddBreakpoint(fileNameStr, line);
    #endif
}

JNIEXPORT void JNICALL Java_org_unify_helper_JsHelper_DebuggerRemoveBreakpoint(JNIEnv* env, jclass clazz, jstring fileName, jint line) {
#ifdef DUK_OPT_DEBUGGER_SUPPORT
    std::string fileNameStr = JniHelper::jstring2string(fileName);

    CEJsEngine::getInstance()->RemoveBreakpoint(fileNameStr, line);
    #endif
}

JNIEXPORT void JNICALL Java_org_unify_helper_JsHelper_DebuggerCooperate(JNIEnv* env, jclass clazz) {
#ifdef DUK_OPT_DEBUGGER_SUPPORT
    CEJsEngine::getInstance()->Cooperate();
    #endif
}

JNIEXPORT void JNICALL Java_org_unify_helper_JsHelper_DebuggerStepInto(JNIEnv* env, jclass clazz) {
#ifdef DUK_OPT_DEBUGGER_SUPPORT
    CEJsEngine::getInstance()->StepIntoDebugger();
    #endif
}

JNIEXPORT void JNICALL Java_org_unify_helper_JsHelper_DebuggerStepOver(JNIEnv* env, jclass clazz) {
#ifdef DUK_OPT_DEBUGGER_SUPPORT
    CEJsEngine::getInstance()->StepOverDebugger();
    #endif
}

JNIEXPORT void JNICALL Java_org_unify_helper_JsHelper_DebuggerStepOut(JNIEnv* env, jclass clazz) {
#ifdef DUK_OPT_DEBUGGER_SUPPORT
    CEJsEngine::getInstance()->StepOutDebugger();
#endif
}

JNIEXPORT void JNICALL Java_org_unify_helper_JsHelper_DebuggerGetLocals(JNIEnv* env, jclass clazz) {
#ifdef DUK_OPT_DEBUGGER_SUPPORT
    CEJsEngine::getInstance()->GetLocalsDebugger();
#endif
}

JNIEXPORT void JNICALL Java_org_unify_helper_JsHelper_DebuggerPrintLeftover(JNIEnv* env, jclass clazz) {
#ifdef DUK_OPT_DEBUGGER_SUPPORT
    CEJsEngine::getInstance()->PrintLeftover();
#endif
}


JNIEXPORT void JNICALL Java_org_unify_helper_JsHelper_DebuggerClearLeftover(JNIEnv* env, jclass clazz) {
#ifdef DUK_OPT_DEBUGGER_SUPPORT
    CEJsEngine::getInstance()->ClearLeftover();
#endif
}


} //extern C

#ifdef DUK_OPT_DEBUGGER_SUPPORT

void OnDebuggerStateChanged(int state, const char* fileName, const char* funcName, int line, int pc)
{

    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, "org/unify/helper/JsHelper", "JniOnDebuggerStateChanged",
                                       "(ILjava/lang/String;Ljava/lang/String;II)V")) {


        jstring jFileName = t.env->NewStringUTF(fileName);
        jstring jFuncName = t.env->NewStringUTF(funcName);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, state, jFileName, jFuncName, line, pc);

        //t.env->DeleteLocalRef(t.classID); //Class is global reference now
        t.env->DeleteLocalRef(jFileName);
        t.env->DeleteLocalRef(jFuncName);
    }
}

void OnDebuggerGetLocals(const char* namesArr, const char* valuesArr)
{

    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, "org/unify/helper/JsHelper", "JniOnDebuggerGetLocals",
                                       "(Ljava/lang/String;Ljava/lang/String;)V")) {


        jstring jNamesArr = t.env->NewStringUTF(namesArr);
        jstring jValuesArr = t.env->NewStringUTF(valuesArr);
        t.env->CallStaticVoidMethod(t.classID, t.methodID, jNamesArr, jValuesArr);

        //t.env->DeleteLocalRef(t.classID); //Class is global reference now
        t.env->DeleteLocalRef(jNamesArr);
        t.env->DeleteLocalRef(jValuesArr);
    }
}

//void OnReceiveExceptionNotification(int fatal, const char* msg, const char* fileName, int line);

void OnReceiveExceptionNotification(int fatal, const char* msg, const char* fileName, int line)
{

        LOGD("OnReceiveExceptionNotification\n");

    JniMethodInfo t;

    if (JniHelper::getStaticMethodInfo(t, "org/unify/helper/JsHelper", "JniOnReceiveExceptionNotification",
                                       "(ILjava/lang/String;Ljava/lang/String;I)V")) {


        jstring jMsg = t.env->NewStringUTF(msg);
        jstring jFileName = t.env->NewStringUTF(fileName);

        t.env->CallStaticVoidMethod(t.classID, t.methodID, fatal, jMsg, jFileName, line);

        //t.env->DeleteLocalRef(t.classID); //Class is global reference now
        t.env->DeleteLocalRef(jMsg);
        t.env->DeleteLocalRef(jFileName);

    }
}
#endif
