//
//  ce_tojs_manual.cpp
//  ce
//
//  Created by kaixuan on 15-6-23.
//  Copyright (c) 2015年 kaixuan. All rights reserved.
//

#include "ce_tojs_manual.h"
#include "PlatformHelper.h"
#include "NotificationCenter.h"

void js_class_push_propertyobject(duk_context* ctx, const char* package, const char* classname)
{
//    duk_push_global_object(ctx);
//    duk_push_object(ctx);
//    duk_dup(ctx, -1);
//    duk_put_prop_string(ctx, -3, pname.CString());
//    duk_remove(ctx, -2); // remove Atomic object
}

void js_class_get_prototype(duk_context* ctx, const char* package, const char *classname)
{
//    duk_get_global_string(ctx, package);
    duk_push_global_object(ctx);
    printf("1: %d\n", duk_get_top(ctx));
    duk_get_prop_string(ctx, -1, classname);
    printf("1: %d -> %d\n", duk_get_top(ctx), duk_get_type(ctx, -1));
    duk_get_prop_string(ctx, -1, "prototype");
    printf("1: %d -> %d\n", duk_get_top(ctx), duk_get_type(ctx, -1));
    duk_remove(ctx, -2); // remove class object
    duk_remove(ctx, -2); // remove Atomic object
}





using namespace ce;







static duk_ret_t js_PlatformHelper_getCurrentLanguage(duk_context* ctx)
{
    duk_push_int(ctx, PlatformHelper::getCurrentLanguage());
    return 1;
}

static duk_ret_t js_PlatformHelper_getTargetPlatform(duk_context* ctx)
{
    duk_push_int(ctx, PlatformHelper::getTargetPlatform());
    return 1;
}

static void tojs_PlatformHelper(duk_context* ctx)
{
    static duk_function_list_entry io_funcs[] = {
        { "getCurrentLanguage", js_PlatformHelper_getCurrentLanguage, 0 },
        { "getTargetPlatform", js_PlatformHelper_getTargetPlatform, 0 },
        { NULL, NULL, 0 }
    };
    
    duk_push_global_object(ctx);
    duk_push_object(ctx);
    duk_put_function_list(ctx, -1, io_funcs);
    duk_put_prop_string(ctx, -2, "PlatformHelper");
    duk_pop(ctx);  /* pop global */
}



//static duk_ret_t js_NotificationCenter_getInstance(duk_context* ctx)
//{
//    void* instance = NotificationCenter::getInstance();
//    duk_push_pointer(ctx, instance);
//    return 1;
//}
//
//
//static duk_ret_t js_NotificationCenter_notify_static(duk_context* ctx)
//{
//    NotificationCenter* self = static_cast<NotificationCenter*>(duk_get_pointer(ctx, 0));
//    if (self)
//    {
//        const char* a = duk_get_string(ctx, 1);
//        CEJsValue b = js_to_cejs_value(ctx, 2);
//        self->notify(a, b.toString().c_str());
//    }
//    return 0;
//}


static duk_ret_t js_NotificationCenter_addObserver(duk_context* J)
{
//    NotificationCenter::getInstance()->addObserver(, );
    return 0;
}


static duk_ret_t js_NotificationCenter_notify(duk_context* ctx)
{
    const char* a = duk_to_string(ctx, 0);
    const char* b = NULL;
    
    if (duk_is_string(ctx, 1))
    {
        b = duk_to_string(ctx, 1);
    }
    else if (duk_is_object(ctx, 1))
    {
        b = duk_json_encode(ctx, 1);
    }
    
    CCLOG("unify b1");
    CCLOG(b);
    CCLOG("unify b2");
    NotificationCenter::getInstance()->notify(a, b);
    return 0;
}



static void tojs_NotificationCenter(duk_context* ctx)
{
//    duk_push_global_object(ctx);
//    duk_push_object(ctx);
//    duk_push_c_function(ctx, js_NotificationCenter_getInstance, 0);
//    duk_put_prop_string(ctx, -2, "getInstance");
//    duk_push_c_function(ctx, js_NotificationCenter_notify_static, 3);
//    duk_put_prop_string(ctx, -2, "notify");
//    duk_put_global_string(ctx, "NotificationCenter");
//    duk_pop(ctx);  /* pop global */
    
    
    static duk_function_list_entry _NotificationCenter_funcs[] = {
        { "addObserver", js_NotificationCenter_addObserver, 2 },
        { "notify", js_NotificationCenter_notify, 2 },
        { NULL, NULL, 0 }
    };
    
    duk_push_global_object(ctx);
    duk_push_object(ctx);
    duk_put_function_list(ctx, -1, _NotificationCenter_funcs);
    duk_put_prop_string(ctx, -2, "notify");
    duk_pop(ctx);  /* pop global */
    
    return;
}


int register_all_ce_manual(duk_context* ctx)
{
    tojs_PlatformHelper(ctx);
    tojs_NotificationCenter(ctx);
    return 0;
}