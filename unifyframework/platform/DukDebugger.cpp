//
//  DukDebugger.cpp
//  unifyframework
//
//  Created by Vlad on 4/13/16.
//  Copyright © 2016 vocinno. All rights reserved.
//

#include "DukDebugger.h"

#include <thread>
#include <mutex>

#include "boost/variant.hpp"

#if UNIFY_ANDROID

#include <android/log.h>


#define  LOG_TAG    "CEJsEngine"
#define  LOGD(...)  __android_log_print(ANDROID_LOG_DEBUG,LOG_TAG,__VA_ARGS__)

#else

#define  LOGD(...)

#endif


#ifdef DUK_OPT_DEBUGGER_SUPPORT


NS_CE_BEGIN


std::shared_ptr<DukDebugger> DukDebuggerPtr;


DukDebugger::DukDebugger(duk_context* ctx)
    : context(ctx)
    , firstPause(false)
    , isAttached(false)
{

    LOGD("DukDebugger::DukDebugger, this = %p, DukDebugger::onReceiveGetLocals = %p", this, &DukDebugger::onReceiveGetLocals);

    grammarReceiver.versionIdentificationReceiver = std::bind(&DukDebugger::onReceiveVersionIdentification, this, std::placeholders::_1);
    grammarReceiver.statusNotificationReceiver = std::bind(&DukDebugger::onReceiveStatusNotification, this, std::placeholders::_1);
    grammarReceiver.replyAddBreakpointReceiver = std::bind(&DukDebugger::onAddBreakpoint, this, std::placeholders::_1);
    grammarReceiver.replyRemoveBreakpointReceiver = std::bind(&DukDebugger::onRemoveBreakpoint, this, std::placeholders::_1);
    grammarReceiver.replyNameValueArrayReceiver = std::bind(&DukDebugger::onReceiveGetLocals, this, std::placeholders::_1);
    grammarReceiver.exceptionNotificationReceiver = std::bind(&DukDebugger::onReceiveExceptionNotification, this, std::placeholders::_1);

    grammarReceiver.InitDefaultRecognizers();
}

void DukDebugger::Attach()
{
    //This method should be called in JS thread
    
    duk_debugger_attach(context, duk_trans_dvalue_read_cb, duk_trans_dvalue_write_cb, duk_trans_dvalue_peek_cb, nullptr, nullptr, duk_trans_dvalue_detached_cb, nullptr);

}


void DukDebugger::Detach()
{
    //This method should be called in JS thread
}

void DukDebugger::Pause() {
    RefillDataToSend(grammarReceiver.CreatePauseRequest());
}


void DukDebugger::Resume()
{
    RefillDataToSend(grammarReceiver.CreateResumeRequest());
}

void DukDebugger::StepInto()
{
    RefillDataToSend(grammarReceiver.CreateStepIntoRequest());
}

void DukDebugger::StepOver()
{
    RefillDataToSend(grammarReceiver.CreateStepOverRequest());
}

void DukDebugger::StepOut()
{
    RefillDataToSend(grammarReceiver.CreateStepOutRequest());
}

void DukDebugger::GetLocals()
{
    RefillDataToSend(grammarReceiver.CreateGetLocalsRequest());
}

void DukDebugger::AddBreakpoint(std::string filename, int line) {

    {
        std::unique_lock<std::mutex> lock(breakpointPendingMutex);

        for (auto itr = breakpointMap.begin(); itr != breakpointMap.end(); itr++) {
            if (itr->second.first == filename && itr->second.second == line) {
                return;
            }
        }

        breakpointPendingList.push_back(std::pair<std::string, int>(filename, line));
    }

    RefillDataToSend(grammarReceiver.CreateAddBreakpointRequest(filename, line));
}

void DukDebugger::RemoveBreakpoint(std::string filename, int line)
{
    int id = -1;
    {
        std::unique_lock<std::mutex> lock(breakpointPendingMutex);

        for (auto itr = breakpointMap.begin(); itr != breakpointMap.end(); itr++) {
            if (itr->second.first == filename && itr->second.second == line) {
                id = itr->first;
                breakpointMap.erase(itr);
                break;
            }
        }
    }

    if (id != -1)
    {
        RefillDataToSend(grammarReceiver.CreateRemoveBreakpointRequest(id));
    }

}

void DukDebugger::onReceiveVersionIdentification(dd::VersionIdentification versionIdentification)
{
    //LOGD("DukDebugger::onReceiveVersionIdentification: %s\n", versionIdentification.line.c_str());
}

void DukDebugger::onReceiveStatusNotification(dd::StatusNotification statusNotification)
{

    LOGD("DukDebugger::onReceiveStatusNotification\n");

    LOGD("statusNotification.fileName: %s\n", statusNotification.fileName.c_str());
    LOGD("statusNotification.funcName: %s\n", statusNotification.funcName.c_str());
    LOGD("statusNotification.lineNumber: %d\n", statusNotification.lineNumber);
    LOGD("statusNotification.pc: %d\n", statusNotification.pc);
    LOGD("statusNotification.state: %d\n", statusNotification.state);

    if (!firstPause) {
        firstPause = true;
    }
    else
    {
        if (OnDebuggerStateChangedFunc)
        {
            OnDebuggerStateChangedFunc(statusNotification);

        }
        
        //GetLocals();
    }
    
    
}

void DukDebugger::onRemoveBreakpoint(dd::Reply reply)
{


}

void DukDebugger::onAddBreakpoint(dd::ReplyInt replyInt)
{
    //LOGD("DukDebugger::onReceiveReplyInt\n");

    std::unique_lock<std::mutex> lock(breakpointPendingMutex);

    if (breakpointPendingList.size() > 0) {

            breakpointMap[replyInt.value] = breakpointPendingList[0];

            breakpointPendingList.erase(breakpointPendingList.begin());

    }

}

void DukDebugger::onReceiveGetLocals(dd::ReplyNameValueArray replyNameValueArray)
{
    LOGD("DukDebugger::onReceiveGetLocals\n");
    if (OnDebuggerGetLocalsFunc)
    {
        OnDebuggerGetLocalsFunc(replyNameValueArray);
    }

}

void DukDebugger::onReceiveExceptionNotification(dd::ExceptionNotification exceptionNotification)
{

    LOGD("DukDebugger::onReceiveExceptionNotification\n");
    if (OnReceiveExceptionNotificationFunc)
    {
        LOGD("DukDebugger::onReceiveExceptionNotification inner\n");
        OnReceiveExceptionNotificationFunc(exceptionNotification);
    }
}


duk_size_t DukDebugger::duk_trans_dvalue_read(void *udata, char *buffer, duk_size_t length)
{
    
    std::unique_lock<std::mutex> uniqueLock(outputBufferMutex);
    
    if (outputBuffer.size() > 0)
    {
        return innerSendData(buffer, length);
    }
    
    //Oops, we don't have anything, we need to wait
    
    outputBufferConditionVariable.wait(uniqueLock, [this]{ return this->outputBuffer.size() > 0; }); //Inner content of predicate function is protected by uniqueLock
    
    return innerSendData(buffer, length);
}

duk_size_t DukDebugger::innerSendData(char *buffer, duk_size_t length)
{
    if (outputBuffer.size() > length)
    {
        //We can send what duktape requested, and still some data left
        
        memcpy(buffer, &outputBuffer[0], length*sizeof(unsigned char));
        
        outputBuffer.erase(outputBuffer.begin(), outputBuffer.begin() + length);
        
        return length;
    }
    
    //We send everything, and it is still not enough
    
    size_t outputBufferSize = outputBuffer.size();
    
    memcpy(buffer, &outputBuffer[0], outputBufferSize*sizeof(unsigned char));
    
    outputBuffer.clear();
    
    return outputBufferSize;
}


duk_size_t DukDebugger::duk_trans_dvalue_write(void *udata, const char *buffer, duk_size_t length)
{
    /*
    LOGD("Write start, len = %ld\n", length);
    
    for (int i = 0; i < length; i++)
    {
        LOGD(" %02x", (reinterpret_cast<unsigned const char*>(buffer))[i]);
    }*/
    
    std::vector<unsigned char> data;
    data.insert(data.begin(), &buffer[0], &buffer[length]);
    
    grammarReceiver.put(data);

    //LOGD("write end");
    return length;
}


duk_size_t DukDebugger::duk_trans_dvalue_peek(void *udata)
{
    std::unique_lock<std::mutex> uniqueLock(outputBufferMutex);

    return outputBuffer.size();
}


void DukDebugger::duk_trans_dvalue_detached(void *udata)
{
    //LOGD("Debugger detached\n");

}


void DukDebugger::RefillDataToSend(const std::vector<unsigned char>& newData)
{
    
    if (newData.size() == 0)
    {
        //Nothing to do here
        return;
    }
    
    
    {
        //Create lock guard to protect outputBuffer
        
        std::lock_guard<std::mutex> lock(outputBufferMutex);
        
        outputBuffer.insert(outputBuffer.end(), newData.begin(), newData.end());
        
    }
    //After lock is released, we can notify
    
    
    outputBufferConditionVariable.notify_all();
}


    void DukDebugger::PrintLeftover()
    {
        std::vector<unsigned char> data = grammarReceiver.getLeftover();
        LOGD("Leftover len = %ld\n", data.size());

        for (size_t i = 0; i < data.size(); i++)
        {
            LOGD(" %02x", data[i]);
        }

        LOGD("Leftover end");

        std::vector<unsigned char> totalLeftover = grammarReceiver.getTotalLeftover();
        LOGD("totalLeftover len = %ld\n", totalLeftover.size());

        for (size_t i = 0; i < totalLeftover.size(); i++)
        {
            LOGD(" %02x", totalLeftover[i]);
        }

        LOGD("totalLeftover end");
    }

    void DukDebugger::ClearLeftover()
    {
        grammarReceiver.clearLeftover();
    }

NS_CE_END


duk_size_t duk_trans_dvalue_read_cb(void *udata, char *buffer, duk_size_t length)
{
    return ce::DukDebuggerPtr->duk_trans_dvalue_read(udata, buffer, length);
}

duk_size_t duk_trans_dvalue_write_cb(void *udata, const char *buffer, duk_size_t length)
{
    return ce::DukDebuggerPtr->duk_trans_dvalue_write(udata, buffer, length);
}

duk_size_t duk_trans_dvalue_peek_cb(void *udata)
{
    return ce::DukDebuggerPtr->duk_trans_dvalue_peek(udata);
}


void duk_trans_dvalue_detached_cb(void *udata)
{
    return ce::DukDebuggerPtr->duk_trans_dvalue_detached(udata);
}



#endif
