//
//  Utils.h
//  ce
//
//  Created by kaixuan on 15-7-27.
//  Copyright (c) 2015年 1901. All rights reserved.
//

#ifndef __ce__Utils__
#define __ce__Utils__

#include <iostream>
#include "common.h"
#include "duktapeExtra.h"

#include <thread>
#include <memory>

#include "boost/asio.hpp"
#include "boost/date_time/posix_time/posix_time.hpp"


NS_CE_BEGIN

struct TimerStruct
{
    std::shared_ptr<boost::asio::deadline_timer> timer;
    bool repeat;
    JS_Function callback;

    
    TimerStruct();
    
    TimerStruct(boost::asio::io_service& io_service, long milliseconds, bool inRepeat, JS_Function inCallback);
};

class CETimerUtils
{
public:
    CETimerUtils();
    static CETimerUtils& getInstance();
    
    int createTimer(long interval, bool repeat, JS_Function callback);
    void cancelTimer(int timerId);
    
protected:
    
    std::thread timerThread;
    
    boost::asio::io_service io_service;
    boost::asio::io_service::work work;
    
    std::map<int, TimerStruct> timerMap;
    
    int timerCounter;
    
    void onTimerFire(int timerId, long interval, const boost::system::error_code& e);
    
};

NS_CE_END

#endif /* defined(__ce__Utils__) */
