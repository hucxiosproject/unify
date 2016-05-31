//
//  CEJsEngine.cpp
//  ce
//
//  Created by kaixuan on 15-6-15.
//  Copyright (c) 2015年 kaixuan. All rights reserved.
//

#include "CEJsEngine.h"
#include <algorithm>

#include "CEJsValue.h"
#include "js_extensions.h"
#include "CCFileUtils.h"
#include <cassert>
#include "boost/lexical_cast.hpp"
#include "boost/algorithm/string/replace.hpp"

#include "DukDebugger.h"
//#include "DukDebuggerGrammar.h"

#if UNIFY_ANDROID

#include "jni/JniHelperUnify.h"

#include <android/jni/JsHelper.h>

#include <android/log.h>


#define  LOG_TAG    "CEJsEngine"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

#else
//iOS

#include "platform/CCJsDebuggerUtils.h"




#define  LOGD(...)

#endif

NS_CE_BEGIN



#ifdef DUK_OPT_DEBUGGER_SUPPORT


    class debugger_value_visitor
    : public boost::static_visitor<std::string>
{
public:

    std::string operator()(int & i) const
    {
        return boost::lexical_cast<std::string>(i) + " (int)";
    }

    std::string operator()(std::string & str) const
    {
        return str + " (str)";
    }


    std::string operator()(double& d) const
    {
        return boost::lexical_cast<std::string>(d) + " (double)";
    }

    std::string operator()(dd::JsClass & obj) const
    {
        std::string result = "class " + boost::lexical_cast<std::string>(obj.number) + ", size " + boost::lexical_cast<std::string>(obj.data.size()) + " (class)";

        return result;
    }
        //JsClass

};

#endif


static int js_pushCEJsValue(duk_context* J, const CEJsValue& value)
{
    int result = 1;
    
    CEJsValueType type = value.getType();
    switch (type) {
        case JsValueNone:
            result = 0;
            break;
        case JsValueTypeUndefined:
            duk_push_undefined(J);
            break;
        case JsValueTypeNull:
            duk_push_null(J);
            break;
        case JsValueTypeBool:
            duk_push_boolean(J, value.boolValue());
            break;
        case JsValueTypeNumber:
            duk_push_number(J, value.numberValue());
            break;
        case JsValueTypeString:
            duk_push_string(J, value.stringValue().c_str());
            break;
        default:
            break;
    }
    
    return result;
}




CEJsEngine* CEJsEngine::_instance = nullptr;


CEJsEngine* CEJsEngine::getInstance()
{
    LOGD("CEJsEngine::getInstance begin");
    if (!_instance)
    {
        LOGD("CEJsEngine::getInstance created");
        _instance = new CEJsEngine();
    }
    LOGD("CEJsEngine::getInstance end");
    return _instance;
}


CEJsEngine::CEJsEngine()
: javaScriptRunning(true)
, _workPtr(new boost::asio::io_service::work(_ioService))
, _javaScriptThread(boost::bind(&boost::asio::io_service::run, &_ioService))
{
    LOGD("CEJsEngine::CEJsEngine() begin");
    _ioService.post(boost::bind(&CEJsEngine::innerInitJs, this));
    LOGD("CEJsEngine::CEJsEngine() end");
}


CEJsEngine::~CEJsEngine()
{
    _ioService.post(boost::bind(&CEJsEngine::destroyHeap, this));
    
    _workPtr = nullptr;
    
    _javaScriptThread.join();
}


void CEJsEngine::setScriptPath(const char *path)
{
    //Should be called only in javascript thread
    assert(boost::this_thread::get_id() == _javaScriptThread.get_id());
    
    if (!_scriptPath.empty())
    {
        // remove old script path from search paths
        std::vector<std::string> newPaths = CCFileUtils::getInstance()->getSearchPaths();
        auto foundIter = newPaths.erase(std::find(newPaths.begin(), newPaths.end(), _scriptPath));
        if (foundIter != newPaths.end()) { newPaths.erase(foundIter); }
        CCFileUtils::getInstance()->setSearchPaths(newPaths);
    }
    
    _scriptPath = std::string(path);
    CCFileUtils::getInstance()->addSearchPath(path);
}




int CEJsEngine::executeString(const char *source)
{
    _ioService.post(boost::bind(&CEJsEngine::innerExecuteString, this, std::string(source)));

    return DUK_EXEC_SUCCESS;
}


int CEJsEngine::innerExecuteString(std::string& source)
{
    //Should be called only in javascript thread
    assert(boost::this_thread::get_id() == _javaScriptThread.get_id());
    
    int result = duk_peval_string(ctx, source.c_str());
    if (result == DUK_EXEC_ERROR) { printf("executeString Error: %s\n", duk_to_string(ctx, -1)); }
    duk_pop(ctx);
    return result;
}

/*
int CEJsEngine::executeJsFile(const char *filename)
{
    //Should be called only in javascript thread
    assert(boost::this_thread::get_id() == _javaScriptThread.get_id());
    
    std::string filePath = ce::CCFileUtils::getInstance()->fullPathForFilename(filename);
    int result = duk_peval_file(ctx, filePath.c_str());
    if (result == DUK_EXEC_ERROR) { printf("executeJsFile Error: %s\n", duk_to_string(ctx, -1)); }
    duk_pop(ctx);
    return result;
}
*/


void CEJsEngine::callJsFunction(JS_Function func)
{
    callJsFunction(func, { JS_NONE });
}

void CEJsEngine::callJsFunction(JS_Function func, const std::vector<CEJsValue> &args)
{
    _ioService.post(boost::bind(&CEJsEngine::innerCallJsFunction, this, func, args));
    
}

void CEJsEngine::innerCallJsFunction(JS_Function func, const std::vector<CEJsValue> args)
{
    //Should be called only in javascript thread
    assert(boost::this_thread::get_id() == _javaScriptThread.get_id());
    
    duk_push_heapptr(ctx, func);
    
    int argNum = 0;
    for (auto arg : args)
    {
        argNum += js_pushCEJsValue(ctx, arg);
    }
    
    if (duk_pcall(ctx, argNum) != DUK_EXEC_SUCCESS)
    {
        CCLOG("callJsFunction Error: %s", duk_to_string(ctx, -1));
    }
    
    clean();
}

/*
CEJsValue CEJsEngine::callGlobalFunction(const char* functionName)
{
    std::vector<CEJsValue> args;
    return callGlobalFunction(functionName, args);
}

*/

CEJsValue CEJsEngine::callGlobalFunction(const char *functionName, const std::vector<CEJsValue>& args)
{
    _ioService.post(boost::bind(&CEJsEngine::innerCallGlobalFunction, this, std::string(functionName), args));
    
    return JS_NULL;

}

CEJsValue CEJsEngine::innerCallGlobalFunction(std::string functionName, const std::vector<CEJsValue> args)
{
    //Should be called only in javascript thread
    assert(boost::this_thread::get_id() == _javaScriptThread.get_id());
    
    
    bool ret = duk_get_global_string(ctx, functionName.c_str());
    if (!ret || !duk_is_function(ctx, -1))
    {
        CCLOG("Can't find funciton: %s", functionName.c_str());
        duk_pop(ctx);
        return JS_NULL;
    }
    
    
    int argNum = 0;
    for (auto arg : args)
    {
        argNum += js_pushCEJsValue(ctx, arg);
    }
    
    CEJsValue result;
    if (duk_pcall(ctx, argNum) == DUK_EXEC_ERROR)
    {
        CCLOG("Error (%s): %s\n", functionName.c_str(), duk_to_string(ctx, -1));
    }
    else
    {
        result = js_to_cejs_value(ctx, -1);
    }
    duk_pop(ctx);
    clean();
    
    return result;
}



CEJsValue CEJsEngine::callProxy(const char *proxyName, const char *funcName)
{
    std::vector<CEJsValue> args;
    return callProxy(proxyName, funcName, args);
}

CEJsValue CEJsEngine::callProxy(const char *proxyName, const char *funcName, const std::vector<CEJsValue> &args)
{
    std::vector<CEJsValue> clonedArgs(args);
    clonedArgs.insert(clonedArgs.begin(), JS_STRING(funcName));
    clonedArgs.insert(clonedArgs.begin(), JS_STRING(proxyName));
    return this->callGlobalFunction("callProxy", clonedArgs);
}

void CEJsEngine::callback(JS_Function func, int code, const char* response)
{
    _ioService.post(boost::bind(&CEJsEngine::innerCallback, this, func, code, std::string(response)));
    
}

#ifdef DUK_OPT_DEBUGGER_SUPPORT

void CEJsEngine::DetachDebugger()
{
    _ioService.post(std::bind(&DukDebugger::Detach, DukDebuggerPtr));
}

void CEJsEngine::PauseDebugger()
{
    DukDebuggerPtr->Pause();
    _ioService.post(std::bind(&duk_debugger_cooperate, ctx));
}

void CEJsEngine::StepIntoDebugger()
{
    DukDebuggerPtr->StepInto();
    // DukDebuggerPtr->GetLocals();
}


void CEJsEngine::StepOverDebugger()
{
    DukDebuggerPtr->StepOver();
    //DukDebuggerPtr->GetLocals();
}

void CEJsEngine::StepOutDebugger()
{
    DukDebuggerPtr->StepOut();
    //DukDebuggerPtr->GetLocals();
}

void CEJsEngine::GetLocalsDebugger()
{
    //CCLOG("CEJsEngine::GetLocalsDebugger begin");
    DukDebuggerPtr->GetLocals();
    //_ioService.post(std::bind(&duk_debugger_cooperate, ctx));
    //CCLOG("CEJsEngine::GetLocalsDebugger end");
}


void CEJsEngine::AddBreakpoint(std::string filename, int line)
{
    //_ioService.post(std::bind(&DukDebugger::AddBreakpoint, DukDebuggerPtr, filename, line));
    DukDebuggerPtr->AddBreakpoint(filename, line);
    _ioService.post(std::bind(&duk_debugger_cooperate, ctx));
}

void CEJsEngine::RemoveBreakpoint(std::string filename, int line)
{
    DukDebuggerPtr->RemoveBreakpoint(filename, line);
    _ioService.post(std::bind(&duk_debugger_cooperate, ctx));
}

void CEJsEngine::Cooperate()
{
    _ioService.post(std::bind(&duk_debugger_cooperate, ctx));
}

    //void OnDebuggerStateChanged(int state, std::string fileName, std::string funcName, int line, int pc);
void CEJsEngine::OnDebuggerStateChangedFunc(dd::StatusNotification statusNotification)
{
    //Declared in Jshelper.h in Android or in
    OnDebuggerStateChanged(statusNotification.state, statusNotification.fileName.c_str(), statusNotification.funcName.c_str(), statusNotification.lineNumber, statusNotification.pc);
}

void CEJsEngine::OnReceiveExceptionNotificationFunc(dd::ExceptionNotification exceptionNotification)
{
    LOGD("CEJsEngine::OnReceiveExceptionNotificationFunc\n");
    //Declared in Jshelper.h in Android or in
    OnReceiveExceptionNotification(exceptionNotification.fatal, exceptionNotification.msg.c_str(), exceptionNotification.fileName.c_str(), exceptionNotification.line);
}

void CEJsEngine::OnDebuggerGetLocalsFunc(dd::ReplyNameValueArray replyNameValueArray)
{

    //std::vector<std::pair<std::string, std::string>> result;

    std::string resultNames;
    std::string resultValues;
/*
    dd::ReplyNameValueArray empty;

    std::vector<std::string> ttvalues;

    for (auto& ss : ttvalues)
    {
        resultNames+="kkk";
    }

    LOGD("empty.values.size = %d", empty.values.size());
    LOGD("replyNameValueArray.values.size = %d", replyNameValueArray.values.size());
*/
    LOGD("replyNameValueArray.values.size = %d", replyNameValueArray.values.size());
    for (auto& replyNameValue : replyNameValueArray.values)
    {
        resultNames += replyNameValue.name + "\n";
        LOGD("replyNameValue.name = %s", replyNameValue.name.c_str());
        std::string resultValue = boost::apply_visitor(debugger_value_visitor(), replyNameValue.value);

        boost::replace_all(resultValue, "\n", "\\n");
        LOGD("resultValue = %s", resultValue.c_str());


        resultValues += resultValue + "\n";

    }

    if (resultNames.size() > 0) {
        resultNames.erase(resultNames.size() - 1, 1);
    }
    if (resultNames.size() > 0) {
        resultValues.erase(resultValues.size() - 1, 1);
    }

    //Declared in Jshelper.h in Android or in
    OnDebuggerGetLocals(resultNames.c_str(), resultValues.c_str());

}

void CEJsEngine::PrintLeftover()
{
    _ioService.post(std::bind(&DukDebugger::PrintLeftover, DukDebuggerPtr));
}

void CEJsEngine::ClearLeftover()
{
    _ioService.post(std::bind(&DukDebugger::ClearLeftover, DukDebuggerPtr));
}

void CEJsEngine::ResumeDebugger()
{
    DukDebuggerPtr->Resume();
    _ioService.post(std::bind(&duk_debugger_cooperate, ctx));
}

#endif

void CEJsEngine::innerCallback(JS_Function func, int code, std::string response)
{
    duk_push_heapptr(ctx, func);
    
    duk_push_int(ctx, code);

    duk_push_string(ctx, response.c_str());
    
    if (duk_pcall(ctx, 2) != DUK_EXEC_SUCCESS)
    {
        CCLOG("Call js function error: %s", duk_to_string(ctx, -1));
    }
    
    clean();

}


void CEJsEngine::clean()
{
    //Should be called only in javascript thread
    assert(boost::this_thread::get_id() == _javaScriptThread.get_id());
    
    duk_set_top(ctx, 0);
}

void CEJsEngine::destroyHeap()
{
    //Should be called only in javascript thread
    assert(boost::this_thread::get_id() == _javaScriptThread.get_id());
    
    duk_destroy_heap(ctx);
    ctx = nullptr;

#if UNIFY_ANDROID
    detachThreadJni();
#endif
}


void CEJsEngine::innerInitJs()
{
    //Should be called only in javascript thread
    assert(boost::this_thread::get_id() == _javaScriptThread.get_id());


    LOGD("CEJsEngine::innerInitJs()");
    printf("%s", "CEJsEngine::innerInitJs()");
    
    setScriptPath("scripts/");
    setScriptPath("js/scripts/");
    ctx = duk_create_heap_default();
    jsopen_extensions(ctx);

#ifdef DUK_OPT_DEBUGGER_SUPPORT
    
    //We are in JS thread, so this might be a race condition here:
    DukDebuggerPtr = std::shared_ptr<DukDebugger>(new DukDebugger(ctx));

    DukDebuggerPtr->Attach();

    DukDebuggerPtr->Resume();

    //duk_peval_string(ctx, "var test = 0;\ntest = undefined;");

    DukDebuggerPtr->OnDebuggerStateChangedFunc = std::bind(&CEJsEngine::OnDebuggerStateChangedFunc, this, std::placeholders::_1);

    DukDebuggerPtr->OnDebuggerGetLocalsFunc = std::bind(&CEJsEngine::OnDebuggerGetLocalsFunc, this, std::placeholders::_1);

    DukDebuggerPtr->OnReceiveExceptionNotificationFunc = std::bind(&CEJsEngine::OnReceiveExceptionNotificationFunc, this, std::placeholders::_1);


    //OnReceiveExceptionNotificationFunc

#endif

    LOGD("CEJsEngine::innerInitJs() end");


#if UNIFY_ANDROID
    attachThreadJni();
#endif
}


#if UNIFY_ANDROID

void CEJsEngine::attachThreadJni()
{

    JNIEnv* env;

    JniHelper::getJavaVM()->AttachCurrentThread(&env, NULL);



}

void CEJsEngine::detachThreadJni()
{
    JniHelper::getJavaVM()->DetachCurrentThread(); //App would crash if thread exit without calling DetachCurrentThread()
}

#endif

NS_CE_END
