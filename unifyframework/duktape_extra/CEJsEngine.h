//
//  CEJsEngine.h
//  ce
//
//  Created by kaixuan on 15-6-15.
//  Copyright (c) 2015年 kaixuan. All rights reserved.
//

#ifndef __ce__CEJsEngine__
#define __ce__CEJsEngine__

#include <iostream>
#include "common.h"
#include "duktapeExtra.h"

#include "boost/asio.hpp"
#include "boost/thread.hpp"

#include "platform/DukDebugger.h"



NS_CE_BEGIN

class CEJsEngine
{
public:
    static CEJsEngine* getInstance();
    virtual ~CEJsEngine();
    
    //NO NO NO
    //inline duk_context* getJsState() const { return ctx; }
    
    
    void setScriptPath(const char* path);
    
    int executeString(const char* source);
    //int executeJsFile(const char* filename);
    

    void callJsFunction(JS_Function func);
    void callJsFunction(JS_Function func, const std::vector<CEJsValue>& args);
    
    //CEJsValue callGlobalFunction(const char* functionName);
    CEJsValue callGlobalFunction(const char* functionName, const std::vector<CEJsValue>& args);
    
    CEJsValue callProxy(const char* proxyName, const char* funcName);
    CEJsValue callProxy(const char* proxyName, const char* funcName, const std::vector<CEJsValue>& args);
    
    
    void callback(JS_Function func, int code, const char* response);


#ifdef DUK_OPT_DEBUGGER_SUPPORT
    void DetachDebugger();

    void PauseDebugger();
    void ResumeDebugger();

    void AddBreakpoint(std::string filename, int line);
    void RemoveBreakpoint(std::string filename, int line);

    void StepIntoDebugger();
    void StepOverDebugger();
    void StepOutDebugger();

    void GetLocalsDebugger();

    void Cooperate();

    void OnDebuggerStateChangedFunc(dd::StatusNotification statusNotification);
    void OnDebuggerGetLocalsFunc(dd::ReplyNameValueArray replyNameValueArray);
    void OnReceiveExceptionNotificationFunc(dd::ExceptionNotification exceptionNotification);


    void PrintLeftover();
    void ClearLeftover();

    #endif
private:
    CEJsEngine();
    
    void innerInitJs();
    
    void innerCallJsFunction(JS_Function func, const std::vector<CEJsValue> args);
    
    CEJsValue innerCallGlobalFunction(std::string functionName, const std::vector<CEJsValue> args);
    
    int innerExecuteString(std::string& source);
    
    void innerCallback(JS_Function func, int code, std::string response);
    
    void clean();
    
    void destroyHeap();

#ifdef UNIFY_ANDROID
    void attachThreadJni();
    void detachThreadJni();
#endif
    
    static CEJsEngine* _instance;
    
    duk_context* ctx;
    
    std::string _scriptPath;
    
    volatile bool javaScriptRunning;
    
    boost::asio::io_service _ioService;
    
    std::shared_ptr<boost::asio::io_service::work> _workPtr;
    
    boost::thread _javaScriptThread;
    
    
    
};

NS_CE_END

#endif /* defined(__ce__CEJsEngine__) */
