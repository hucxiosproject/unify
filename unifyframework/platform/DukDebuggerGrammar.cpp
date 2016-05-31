//
//  DukDebuggerGrammar.cpp
//  unifyframework
//
//  Created by Vlad on 4/14/16.
//  Copyright © 2016 vocinno. All rights reserved.
//

#include "DukDebuggerGrammar.h"



#ifdef DUK_OPT_DEBUGGER_SUPPORT


#include "boost/spirit/include/classic.hpp"
#include "boost/spirit/include/qi.hpp"
#include "boost/spirit/include/phoenix_core.hpp"
#include "boost/spirit/include/phoenix_operator.hpp"
#include "boost/spirit/include/phoenix_fusion.hpp"
#include "boost/spirit/include/phoenix_stl.hpp"
#include <boost/spirit/include/qi_binary.hpp>


#include "boost/fusion/include/adapt_struct.hpp"
#include "boost/phoenix/object/construct.hpp"

#include <boost/fusion/container/vector.hpp>
#include <boost/fusion/include/vector.hpp>
#include <boost/fusion/container/vector/vector_fwd.hpp>
#include <boost/fusion/include/vector_fwd.hpp>

#include <boost/phoenix/object/static_cast.hpp>

#include <boost/phoenix/bind/bind_function.hpp>

//#include "DukDebugger.h"




//Welcome to the magic world of Boost C++ templates, Spirit and Phoenix Library!

//More information about debug protocol in general: https://github.com/svaarala/duktape/blob/master/doc/debugger.rst

//More information about Spirit: http://www.boost.org/doc/libs/1_60_0/libs/spirit/doc/html/index.html
//More information about Phoenix: http://www.boost.org/doc/libs/1_60_0/libs/phoenix/doc/html/index.html



BOOST_FUSION_ADAPT_STRUCT(
                          dd::JsClass,
                          (int, number)
                          (std::vector<unsigned char>, data)
                          )

BOOST_FUSION_ADAPT_STRUCT(
                          dd::VersionIdentification,
                          (std::string, line)
                          )

BOOST_FUSION_ADAPT_STRUCT(
                          dd::Reply,
                          (std::vector<int>, values)
                          )


BOOST_FUSION_ADAPT_STRUCT(
        dd::ReplyInt,
        (int, value)
)

BOOST_FUSION_ADAPT_STRUCT(
                          dd::StatusNotification,
                          (int, state)
                          (std::string, fileName)
                          (std::string, funcName)
                          (int, lineNumber)
                          (int, pc)
                          )

BOOST_FUSION_ADAPT_STRUCT(
                          dd::ReplyNameValue,
                          (std::string, name)
                          (dd::VarType, value)
                          )

BOOST_FUSION_ADAPT_STRUCT(
                          dd::ReplyNameValueArray,
                          (std::vector<dd::ReplyNameValue>, values)
                          )

BOOST_FUSION_ADAPT_STRUCT(
        dd::ExceptionNotification,
        (int, fatal)
                (std::string, msg)
                (std::string, fileName)
                (int, line)
)

namespace dd
{
    
    const unsigned char REQ = 0x01;
    const unsigned char REP = 0x02;
    const unsigned char NFY = 0x04;
    const unsigned char EOM = 0x00;
    
    
    const unsigned char JS_CLASS_SHIFT = 0x1b;
    const unsigned char INT_SHIFT = 0x80;
    const unsigned char LONG_INT_SHIFT = 0xC0;
    
    const unsigned char NOTIFICATION_STATUS = 0x01;
    const unsigned char NOTIFICATION_EXCEPTION = 0x05;
    
    const unsigned char NOTIFICATION_STATUS_INT = INT_SHIFT + NOTIFICATION_STATUS;

    const unsigned char NOTIFICATION_EXCEPTION_INT = INT_SHIFT + NOTIFICATION_EXCEPTION;


    using namespace boost::spirit;
    
    
    using boost::phoenix::at_c;
    using boost::phoenix::push_back;

    typedef std::vector<unsigned char>::const_iterator Iterator;


    template<typename TGrammar, typename TResult>
    struct Recognizer : public BaseRecognizer
    {

        TGrammar& grammar;
        std::function<void(TResult)> callback;

        TResult result;

        Recognizer(TGrammar& inGrammar, std::function<void(TResult)> inCallback)
            : grammar(inGrammar)
            , callback(inCallback)
        {

        }


        virtual bool eat(std::vector<unsigned char>& data)
        {
            result = TResult(); //clear

            Iterator iter = data.begin();
            Iterator end = data.end();

            if (qi::parse(iter, end, grammar, result))
            {
                data.erase(data.begin(), iter);

                if (callback)
                {
                    callback(result);
                }

                return true;
            }

            return false;
        }

    };





    struct VersionIdentificationGrammar : qi::grammar<Iterator, VersionIdentification()>
    {
        VersionIdentificationGrammar()
            : VersionIdentificationGrammar::base_type(start)
        {
            //Working, and simple:
            start = qi::char_(0x31, 0x39) >> qi::char_(0x20) >> +(qi::char_ - (char)0x0a) >> qi::char_(0x0a);
            
        }
        
        qi::rule<Iterator, VersionIdentification()> start;
    };

    struct BigEndianWord : qi::grammar<Iterator, int()>
    {
        qi::rule<Iterator, int()> start;
        BigEndianWord()
                : BigEndianWord::base_type(start)
        {
            start = qi::big_word;
        }
    };

    struct DukDebugStr2 : qi::grammar<Iterator, std::string()>
    {
        qi::rule<Iterator, std::string()> start;

        qi::rule<Iterator, locals<int>, std::string()> lenStrType2;

        DukDebugStr2()
        : DukDebugStr2::base_type(start)
        {

            lenStrType2 %= omit[qi::char_(0x12)] >>
            omit[ qi::big_word [ qi::_a = _1]] >>
            repeat(boost::phoenix::ref(qi::_a))[qi::char_];


            start =  lenStrType2;
        }
    };

    struct DukDebugStr : qi::grammar<Iterator, std::string()>
    {
        qi::rule<Iterator, std::string()> start;

        qi::rule<Iterator, locals<int>, std::string()> lenStrType1;
        qi::rule<Iterator, locals<int>, std::string()> lenStrType2;

        qi::rule<Iterator, std::string()> undefinedStr;

        qi::rule<Iterator, std::string()> nullStr;

        qi::rule<Iterator, std::string()> trueStr;

        qi::rule<Iterator, std::string()> falseStr;

        qi::rule<Iterator, std::string()> unusedStr;

        DukDebugStr()
                : DukDebugStr::base_type(start)
        {
            lenStrType1 %= omit[qi::char_(0x60, 0x7f) [ qi::_a = (_1 - 0x60)]] >>
                           repeat(boost::phoenix::ref(qi::_a))[qi::char_];

            lenStrType2 %= omit[qi::char_(0x12)] >>
                           omit[ qi::big_word [ qi::_a = _1]] >>
                           repeat(boost::phoenix::ref(qi::_a))[qi::char_];

            undefinedStr = qi::char_(0x16) [ _val = "undefined" ];

            nullStr = qi::char_(0x17) [ _val = "null" ];

            trueStr = qi::char_(0x18) [ _val = "true" ];

            falseStr = qi::char_(0x19) [ _val = "false" ];

            unusedStr = qi::char_(0x15) [ _val = "unused/none" ];


            start = lenStrType1 | lenStrType2 | undefinedStr | nullStr | trueStr | falseStr | unusedStr;
        }
    };


    struct DukDebugInteger : qi::grammar<Iterator, int()>
    {
        qi::rule<Iterator, int()> start;
        
        qi::rule<Iterator, int()> shortInteger;
        
        qi::rule<Iterator, locals<int>, int()> longInteger;
        
        DukDebugInteger()
        : DukDebugInteger::base_type(start)
        {
           
            shortInteger = qi::char_(0x80, 0xbf) [ _val = boost::phoenix::static_cast_<unsigned char>(_1) - INT_SHIFT ];
            
            longInteger = qi::char_(0xc0, 0xff)[qi::_a = boost::phoenix::static_cast_<unsigned char>(_1) - LONG_INT_SHIFT ] >> qi::char_ [_val = qi::_a*0x100 + boost::phoenix::static_cast_<unsigned char>(_1)];
            
            start = shortInteger | longInteger;
        }
    };

    struct DukDebugDouble : qi::grammar<Iterator, double()>
    {
        qi::rule<Iterator, double()> start;

        DukDebugDouble()
                : DukDebugDouble::base_type(start)
        {

            static_assert(sizeof(double) == 8, "double must be 64 bits");

            start = omit[qi::char_(0x1a)] >>
                    boost::spirit::big_bin_double;
                    //boost::spirit::big_qword[_val = *boost::phoenix::static_cast_<long double*>(&_1)];
        }
    };
    

    struct DukDebugJsClass : qi::grammar<Iterator, JsClass()>
    {
        qi::rule<Iterator, JsClass()> start;
        
        qi::rule<Iterator, locals<int>, std::vector<unsigned char>()> lenStrType;
        
        DukDebugJsClass()
        : DukDebugJsClass::base_type(start)
        {
            lenStrType %= omit[qi::char_ [ qi::_a = _1]] >>
            repeat(boost::phoenix::ref(qi::_a))[qi::char_];
            
            
            start = qi::char_(JS_CLASS_SHIFT) >>
            qi::char_ [at_c<0>(_val) = _1] >>
            lenStrType [at_c<1>(_val) = _1];
        }
    };

    struct StatusNotificationGrammar : qi::grammar<Iterator, StatusNotification()>
    {
        
        StatusNotificationGrammar()
        : StatusNotificationGrammar::base_type(start)
        {

            //More simple version:
            start =
            omit[qi::char_(NFY)] >>
            omit[qi::char_(NOTIFICATION_STATUS_INT)]>>
            debugInt >>
            debugString >>
            debugString >>
            debugInt >>
            debugInt >>
            omit[qi::char_(EOM)]
            ;
        
        }
        
        qi::rule<Iterator, StatusNotification()> start;

        
        DukDebugStr debugString;
        DukDebugInteger debugInt;
    };


    struct ReplyGrammar : qi::grammar<Iterator, Reply()>
    {
        ReplyGrammar()
        : ReplyGrammar::base_type(start)
        {

            //More simple version:
            start =
            omit[qi::char_(REP)] >>
            omit[qi::char_(EOM)];
            
        }
        
        qi::rule<Iterator, Reply()> start;
        DukDebugInteger debugInt;
        DukDebugStr debugString;
    };

    struct ReplyIntGrammar : qi::grammar<Iterator, ReplyInt()>
    {
        ReplyIntGrammar()
                : ReplyIntGrammar::base_type(start)
        {

            //More simple version:
            start =
                    omit[qi::char_(REP)] >>
                    debugInt >>
                    omit[qi::char_(EOM)];

        }

        qi::rule<Iterator, ReplyInt()> start;
        DukDebugInteger debugInt;
    };

    struct ReplyNameValueGrammar : qi::grammar<Iterator, ReplyNameValue()>
    {
        ReplyNameValueGrammar()
        : ReplyNameValueGrammar::base_type(start)
        {
            
            start = debugString >>
            (debugInt | debugString | debugDouble | debugJsClass);
        }
        
        qi::rule<Iterator, ReplyNameValue()> start;
        
        DukDebugInteger debugInt;
        
        DukDebugStr debugString;

        DukDebugDouble debugDouble;


        DukDebugJsClass debugJsClass;
    };
    

    struct ReplyNameValueArrayGrammar : qi::grammar<Iterator, ReplyNameValueArray()>
    {
        ReplyNameValueArrayGrammar()
        : ReplyNameValueArrayGrammar::base_type(start)
        {
            start =
            omit[qi::char_(REP)] >>
            *(replyNameValue) >>
            omit[qi::char_(EOM)];
            
        }
        
        qi::rule<Iterator, ReplyNameValueArray()> start;
        
        ReplyNameValueGrammar replyNameValue;
 
    };


    struct ExceptionNotificationGrammar : qi::grammar<Iterator, ExceptionNotification()>
    {

        ExceptionNotificationGrammar()
                : ExceptionNotificationGrammar::base_type(start)
        {

            start =
                    omit[qi::char_(NFY)] >>
                    omit[qi::char_(NOTIFICATION_EXCEPTION_INT)]>>
                    debugInt >>
                    debugString >>
                    debugString >>
                    debugInt >>
                    omit[qi::char_(EOM)]
                    ;

        }

        qi::rule<Iterator, ExceptionNotification()> start;


        DukDebugStr debugString;
        DukDebugInteger debugInt;
    };
    
    
    VersionIdentificationGrammar versionIdentificationGrammar;
    
    StatusNotificationGrammar statusNotificationGrammar;
    
    ReplyGrammar replyGrammar;

    ReplyIntGrammar replyIntGrammar;

    ReplyNameValueArrayGrammar replyNameValueArrayGrammar;

    ExceptionNotificationGrammar exceptionNotificationGrammar;


    GrammarReceiver::GrammarReceiver()
    {
    }

    void GrammarReceiver::InitDefaultRecognizers()
    {
        defaultRecognizers.push_back(std::shared_ptr<BaseRecognizer>(new Recognizer<VersionIdentificationGrammar, VersionIdentification>(versionIdentificationGrammar, versionIdentificationReceiver)));
        defaultRecognizers.push_back(std::shared_ptr<BaseRecognizer>(new Recognizer<StatusNotificationGrammar, StatusNotification>(statusNotificationGrammar, statusNotificationReceiver)));
        defaultRecognizers.push_back(std::shared_ptr<BaseRecognizer>(new Recognizer<ExceptionNotificationGrammar, ExceptionNotification>(exceptionNotificationGrammar, exceptionNotificationReceiver)));
    }



    std::vector<unsigned char> GrammarReceiver::getLeftover()
    {
        return savedData;
    }

    std::vector<unsigned char> GrammarReceiver::getTotalLeftover()
    {
        return totalSavedData;
    }

    void GrammarReceiver::clearLeftover()
    {
        savedData.clear();
    }



    
    void GrammarReceiver::put(const std::vector<unsigned char>& data)
    {

        


        totalSavedData.insert(totalSavedData.end(), data.begin(), data.end());
        savedData.insert(savedData.end(), data.begin(), data.end());
        
        bool recognizerRepeat;

        do {
            recognizerRepeat = false;
            for (auto& defaultRecognizer : defaultRecognizers) {
                if (defaultRecognizer->eat(savedData))
                {
                    recognizerRepeat = true;
                    break;
                }
            }

            {
                std::unique_lock<std::mutex> uniqueLock(recognizerQueueMutex); //Careful with deadlocks!

                if (!recognizerRepeat && recognizerQueue.size() > 0 && recognizerQueue[0]->eat(savedData))
                {
                    recognizerQueue.erase(recognizerQueue.begin());
                    recognizerRepeat = true;
                }
            }

        } while (recognizerRepeat);


    }
    
    
    std::vector<unsigned char> GrammarReceiver::CreatePauseRequest()
    {
        {
            std::unique_lock<std::mutex> uniqueLock(recognizerQueueMutex);
            recognizerQueue.push_back(std::shared_ptr<BaseRecognizer>(
                    new Recognizer<ReplyGrammar, Reply>(replyGrammar, 0)));
        }
        return {REQ, INT_SHIFT + 0x12, EOM};
    }
    
    std::vector<unsigned char> GrammarReceiver::CreateResumeRequest()
    {
        {
            std::unique_lock<std::mutex> uniqueLock(recognizerQueueMutex);
            recognizerQueue.push_back(std::shared_ptr<BaseRecognizer>(
                    new Recognizer<ReplyGrammar, Reply>(replyGrammar, 0)));
        }
        return {REQ, INT_SHIFT + 0x13, EOM};
    }

    std::vector<unsigned char> GrammarReceiver::CreateStepIntoRequest()
    {
        {
            std::unique_lock<std::mutex> uniqueLock(recognizerQueueMutex);
            recognizerQueue.push_back(std::shared_ptr<BaseRecognizer>(
                    new Recognizer<ReplyGrammar, Reply>(replyGrammar, 0)));
        }
        return {REQ, INT_SHIFT + 0x14, EOM};
    }

    std::vector<unsigned char> GrammarReceiver::CreateStepOverRequest()
    {
        {
            std::unique_lock<std::mutex> uniqueLock(recognizerQueueMutex);
            recognizerQueue.push_back(std::shared_ptr<BaseRecognizer>(
                    new Recognizer<ReplyGrammar, Reply>(replyGrammar, 0)));
        }
        return {REQ, INT_SHIFT + 0x15, EOM};
    }

    std::vector<unsigned char> GrammarReceiver::CreateStepOutRequest()
    {
        {
            std::unique_lock<std::mutex> uniqueLock(recognizerQueueMutex);
            recognizerQueue.push_back(std::shared_ptr<BaseRecognizer>(
                    new Recognizer<ReplyGrammar, Reply>(replyGrammar, 0)));
        }
        return {REQ, INT_SHIFT + 0x16, EOM};
    }

    
    std::vector<unsigned char> GrammarReceiver::CreateGetLocalsRequest()
    {
        {
            std::unique_lock<std::mutex> uniqueLock(recognizerQueueMutex);

            recognizerQueue.push_back(std::shared_ptr<BaseRecognizer>(
                    new Recognizer<ReplyNameValueArrayGrammar, ReplyNameValueArray>(
                            replyNameValueArrayGrammar, replyNameValueArrayReceiver)));
        }
        return {REQ, INT_SHIFT + 0x1d, EOM};
    }

    std::vector<unsigned char> GrammarReceiver::CreateAddBreakpointRequest(std::string fileName, int line)
    {
        {
            std::unique_lock<std::mutex> uniqueLock(recognizerQueueMutex);
            recognizerQueue.push_back(std::shared_ptr<BaseRecognizer>(
                    new Recognizer<ReplyIntGrammar, ReplyInt>(replyIntGrammar, replyAddBreakpointReceiver)));
        }

        std::vector<unsigned char> result =  {REQ, INT_SHIFT + 0x18};

        if (fileName.size() <= 31)
        {
            result.push_back(0x60 + static_cast<unsigned char>(fileName.size()));
        }
        else
        {
            unsigned char hi = static_cast<unsigned char>(fileName.size() % 256);
            unsigned char lo = static_cast<unsigned char>(fileName.size() / 256);
            result.push_back(0x12);
            result.push_back(hi);
            result.push_back(lo);
        }

        for (int i = 0; i < fileName.size(); i++)
        {
            result.push_back(static_cast<unsigned char>(fileName[i]));
        }

        if (line <= 63)
        {
            result.push_back(0x80 + static_cast<unsigned char>(line));
        }
        else if (line >= 64 && line <= 16383)
        {
            unsigned char hi = static_cast<unsigned char>(line / 256);
            unsigned char lo = static_cast<unsigned char>(line % 256);
            result.push_back(0xc0 + hi);
            result.push_back(lo);
        }
        

        result.push_back(EOM);
        return result;
    }

    std::vector<unsigned char> GrammarReceiver::CreateRemoveBreakpointRequest(int id)
    {
        {
            std::unique_lock<std::mutex> uniqueLock(recognizerQueueMutex);
            recognizerQueue.push_back(std::shared_ptr<BaseRecognizer>(new Recognizer<ReplyGrammar, Reply>(replyGrammar, replyRemoveBreakpointReceiver)));
        }
        std::vector<unsigned char> result =  {REQ, INT_SHIFT + 0x19};


        if (id <= 63)
        {
            result.push_back(0x80 + static_cast<unsigned char>(id));
        }
        else if (id >= 64 && id <= 16383)
        {
            unsigned char hi = static_cast<unsigned char>(id / 256);
            unsigned char lo = static_cast<unsigned char>(id % 256);
            result.push_back(0xc0 + hi);
            result.push_back(lo);
        }


        result.push_back(EOM);
        return result;
    }
}

#endif