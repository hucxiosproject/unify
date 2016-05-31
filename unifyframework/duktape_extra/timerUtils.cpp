//
//  utils.cpp
//  ce
//
//  Created by kaixuan on 15-7-27.
//  Copyright (c) 2015年 1901. All rights reserved.
//

#include "timerUtils.h"
#include "CETimerUtils.h"
/*
static int jsb_setTimeout(duk_context* ctx)
{
    CCLOG("jsb_setTimeout, enter");
    if (!duk_is_function(ctx, 0))
    {
        CCLOG("arg(0) is't function.");
        return 0;
    }
    
    JS_Function callback = duk_get_heapptr(ctx, 0);
    double millisecondInterval = duk_to_number(ctx, 1);
    void* timer = ce::CETimerUtils::createTimer(millisecondInterval, false, callback);
    duk_push_pointer(ctx, timer);
    return 1;
}

static int jsb_clearTimeout(duk_context* ctx)
{
    if (duk_is_pointer(ctx, 0))
    {
        void* timer = duk_get_pointer(ctx, 0);
        ce::CETimerUtils::cancelTimer(timer);
    }
    return 0;
}

static int jsb_setInterval(duk_context* ctx)
{
    if (!duk_is_function(ctx, 0))
    {
        CCLOG("arg(0) is't function.");
        return 0;
    }
    
    JS_Function callback = duk_get_heapptr(ctx, 0);
    double millisecondInterval = duk_to_number(ctx, 1);
    void* timer = ce::CETimerUtils::createTimer(millisecondInterval, true, callback);
    duk_push_pointer(ctx, timer);
    return 1;
}

static int jsb_clearInterval(duk_context* ctx)
{
    if (duk_is_pointer(ctx, 0))
    {
        void* timer = duk_get_pointer(ctx, 0);
        ce::CETimerUtils::cancelTimer(timer);
    }
    return 0;
}
*/


static int jsb_setTimeout(duk_context* ctx)
{
    if (!duk_is_function(ctx, 0))
    {
        CCLOG("arg(0) is't function.");
        return 0;
    }
    
    JS_Function callback = duk_get_heapptr(ctx, 0);
    double millisecondInterval = duk_to_number(ctx, 1);
    int timerId = ce::CETimerUtils::getInstance().createTimer(millisecondInterval, false, callback);
    duk_push_int(ctx, timerId);
    return 1;
}

static int jsb_setInterval(duk_context* ctx)
{
    if (!duk_is_function(ctx, 0))
    {
        CCLOG("arg(0) is't function.");
        return 0;
    }
    
    JS_Function callback = duk_get_heapptr(ctx, 0);
    double millisecondInterval = duk_to_number(ctx, 1);
    int timerId = ce::CETimerUtils::getInstance().createTimer(millisecondInterval, true, callback);
    duk_push_int(ctx, timerId);
    return 1;
}

static int jsb_clearInterval(duk_context* ctx)
{

    int timerId = duk_get_int(ctx, 0);
    ce::CETimerUtils::getInstance().cancelTimer(timerId);
    return 0;
}



static duk_function_list_entry utils_funcs[] = {
    { "setTimeout", jsb_setTimeout, 2 },
    { "clearTimeout", jsb_clearInterval, 1 },
    { "setInterval", jsb_setInterval, 2 },
    { "clearInterval", jsb_clearInterval, 1 },
    { NULL, NULL, 0 }
};

int js_register_utils(duk_context* ctx)
{
    duk_push_global_object(ctx);
    duk_put_function_list(ctx, -1, utils_funcs);
    duk_pop(ctx);  /* pop global */
    return 1;
}