//
//  CETimerUtils.cpp
//  unifyframework
//
//  Created by Vlad on 5/16/16.
//  Copyright © 2016 vocinno. All rights reserved.
//

#include "CETimerUtils.h"

#include "boost/bind.hpp"

#include "CEJsEngine.h"

boost::asio::io_service timer_io;

NS_CE_BEGIN


TimerStruct::TimerStruct()
{
    
}

TimerStruct::TimerStruct(boost::asio::io_service& io_service, long milliseconds, bool inRepeat, JS_Function inCallback)
: timer(new boost::asio::deadline_timer(io_service, boost::posix_time::milliseconds(milliseconds)))
, repeat(inRepeat)
, callback(inCallback)
{
        
}



std::shared_ptr<CETimerUtils> timerUtils;


CETimerUtils::CETimerUtils()
: work(io_service)
, timerCounter(0)
{
    auto f = boost::bind(&boost::asio::io_service::run, &io_service);
    
    timerThread = std::thread(f);
}


CETimerUtils& CETimerUtils::getInstance()
{
    if (timerUtils == nullptr)
    {
        timerUtils = std::shared_ptr<CETimerUtils>(new CETimerUtils());
    }
    
    return *timerUtils;
}



int CETimerUtils::createTimer(long interval, bool repeat, JS_Function callback)
{
    int timerId = timerCounter;
    
    TimerStruct timerStruct(io_service, interval, repeat, callback);
    
    timerStruct.timer->async_wait(boost::bind(&CETimerUtils::onTimerFire, this, timerId, interval, _1));
    
    timerMap.emplace(timerId, timerStruct);
    
    timerCounter = timerCounter + 1;
    
    
    return timerId;
}

void CETimerUtils::cancelTimer(int timerId)
{
    timerMap.erase(timerId);
}

void CETimerUtils::onTimerFire(int timerId, long interval, const boost::system::error_code& e)
{
    if (!e)
    {
        if (timerMap.count(timerId) != 0)
        {
            TimerStruct& timerStruct = timerMap[timerId];
            
            ce::CEJsEngine::getInstance()->callJsFunction(timerStruct.callback);
            
            if (timerStruct.repeat)
            {
                timerStruct.timer->expires_at(timerStruct.timer->expires_at() + boost::posix_time::milliseconds(interval));
                timerStruct.timer->async_wait(boost::bind(&CETimerUtils::onTimerFire, this, timerId, interval, _1));
            }
            else
            {
                cancelTimer(timerId);
            }
        }
    }
}

NS_CE_END

