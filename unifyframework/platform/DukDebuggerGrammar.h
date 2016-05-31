//
//  DukDebuggerGrammar.hpp
//  unifyframework
//
//  Created by Vlad on 4/14/16.
//  Copyright © 2016 vocinno. All rights reserved.
//

#ifndef DukDebuggerGrammar_h
#define DukDebuggerGrammar_h

#include <string>
#include <vector>
#include <functional>
#include <memory>
#include <mutex>

#include "boost/variant.hpp"


#ifdef DUK_OPT_DEBUGGER_SUPPORT


namespace dd {
    
    
    struct JsClass
    {
        int number;
        std::vector<unsigned char> data;
    };

    typedef boost::variant<int, std::string, double, JsClass> VarType;

    struct VersionIdentification
    {
        std::string line;
    };
    
    struct StatusNotification
    {
        int state;
        std::string fileName;
        std::string funcName;
        int lineNumber;
        int pc;
    };

    struct ReplyInt
    {
        int value;
    };
    
    struct Reply
    {
        std::vector<int> values;
    };
    
    struct ReplyNameValue
    {
        std::string name;
        VarType value;
    };

    struct ReplyNameValueArray
    {
        std::vector<ReplyNameValue> values;
    };

    struct ExceptionNotification
    {
        int fatal;
        std::string msg;
        std::string fileName;
        int line;
    };


    struct BaseRecognizer
    {
        virtual bool eat(std::vector<unsigned char>& data) = 0;

        virtual ~BaseRecognizer()
        {}

    };
    
    
    class GrammarReceiver
    {
    public:

        GrammarReceiver();

        std::vector<unsigned char> getLeftover();
        std::vector<unsigned char> getTotalLeftover();
        void clearLeftover();
        void put(const std::vector<unsigned char>& data);
        
        std::function<void(VersionIdentification)> versionIdentificationReceiver;
        std::function<void(StatusNotification)> statusNotificationReceiver;
        std::function<void(ReplyInt)> replyAddBreakpointReceiver;
        std::function<void(Reply)> replyRemoveBreakpointReceiver;
        
        std::function<void(ReplyNameValueArray)> replyNameValueArrayReceiver;
        std::function<void(ExceptionNotification)> exceptionNotificationReceiver;

        void InitDefaultRecognizers();
        
        std::vector<unsigned char> CreatePauseRequest();
        std::vector<unsigned char> CreateResumeRequest();
        std::vector<unsigned char> CreateAddBreakpointRequest(std::string fileName, int line);
        std::vector<unsigned char> CreateRemoveBreakpointRequest(int id);
        std::vector<unsigned char> CreateStepIntoRequest();
        std::vector<unsigned char> CreateStepOverRequest();
        std::vector<unsigned char> CreateStepOutRequest();
        std::vector<unsigned char> CreateGetLocalsRequest();

    protected:
        std::vector<unsigned char> totalSavedData;
        std::vector<unsigned char> savedData;

        //recognizerQueue
        std::mutex recognizerQueueMutex;

        std::vector<std::shared_ptr<BaseRecognizer>> recognizerQueue;

        std::vector<std::shared_ptr<BaseRecognizer>> defaultRecognizers;


    };
    
    
}

#endif

#endif /* DukDebuggerGrammar_h */
