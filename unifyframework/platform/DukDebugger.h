//
//  DukDebugger.hpp
//  unifyframework
//
//  Created by Vlad on 4/13/16.
//  Copyright © 2016 vocinno. All rights reserved.
//

#ifndef DukDebugger_h
#define DukDebugger_h

#include <functional>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <map>

#include "include/common.h"
#include "duktape1.4.0/duktape.h"

#include "DukDebuggerGrammar.h"



#ifdef DUK_OPT_DEBUGGER_SUPPORT


NS_CE_BEGIN

class DukDebugger
{
public:
    DukDebugger(duk_context* ctx);
    
    void Attach();
    void Detach();
    
    void Pause();
    void Resume();
    void StepInto();
    void StepOver();
    void StepOut();
    
    void GetLocals();

    void AddBreakpoint(std::string filename, int line);
    void RemoveBreakpoint(std::string filename, int line);


    void onReceiveVersionIdentification(dd::VersionIdentification versionIdentification);
    void onReceiveStatusNotification(dd::StatusNotification statusNotification);
    void onAddBreakpoint(dd::ReplyInt replyInt);
    void onRemoveBreakpoint(dd::Reply reply);
    void onReceiveGetLocals(dd::ReplyNameValueArray replyNameValueArray);
    void onReceiveExceptionNotification(dd::ExceptionNotification exceptionNotification);

    std::function<void()> OnDetached;
    std::function<void(dd::StatusNotification)> OnDebuggerStateChangedFunc;
    std::function<void(dd::ReplyNameValueArray)> OnDebuggerGetLocalsFunc;
    std::function<void(dd::ExceptionNotification)> OnReceiveExceptionNotificationFunc;

    //Callbacks from duktape
    duk_size_t duk_trans_dvalue_read(void *udata, char *buffer, duk_size_t length);
    duk_size_t duk_trans_dvalue_write(void *udata, const char *buffer, duk_size_t length);
    duk_size_t duk_trans_dvalue_peek(void *udata);
    void duk_trans_dvalue_detached(void *udata);

    duk_size_t innerSendData(char *buffer, duk_size_t length);
    
    void RefillDataToSend(const std::vector<unsigned char>& newData);

    void PrintLeftover();
    void ClearLeftover();
    
protected:

    duk_context* context;
    bool firstPause;
    bool isAttached;
    
    dd::GrammarReceiver grammarReceiver;
    
    std::vector<unsigned char> outputBuffer;
    
    std::mutex outputBufferMutex;
    
    std::condition_variable outputBufferConditionVariable;

    std::mutex breakpointPendingMutex;

    std::vector<std::pair<std::string, int>> breakpointPendingList;

    std::map<int, std::pair<std::string, int>> breakpointMap;


};


extern std::shared_ptr<DukDebugger> DukDebuggerPtr;

NS_CE_END


duk_size_t duk_trans_dvalue_read_cb(void *udata, char *buffer, duk_size_t length);
duk_size_t duk_trans_dvalue_write_cb(void *udata, const char *buffer, duk_size_t length);
duk_size_t duk_trans_dvalue_peek_cb(void *udata);
void duk_trans_dvalue_detached_cb(void *udata);

#endif

#endif /* DukDebugger_h */
