//
//  NotificationCenter.h
//  ce
//
//  Created by kaixuan on 15-6-29.
//  Copyright (c) 2015年 1901. All rights reserved.
//

#ifndef __ce__NotificationCenter__
#define __ce__NotificationCenter__

#include "common.h"
#include <map>
#include <vector>
#include <functional>
#include "duktapeExtra.h"

#if UNIFY_IOS
#include <objc/objc.h>
#endif
 
typedef std::function<void(const char*)> callback_string;
typedef std::function<void(const char*, const char*)> callback_string_string;
typedef std::function<void(const char*, void*)> callback_string_void;
//typedef void (^IOSCallBackBlock)(NSString *name,id object);
//typedef void (^IOSCallBackBlock)(const char *name,const char*body);

NS_CE_BEGIN

class NotificationCenter
{
private:
    class observer
    {
    public:
        callback_string_string func = NULL;
        
#if UNIFY_IOS
        // for objc
        
        id objcTarget = NULL;
        id objcLastTarget = NULL;
        SEL objcCallback = NULL;
        
        //IOSCallBackBlock objBlock = NULL;
#elif UNIFY_ANDROID
        // for jni
        std::string className;
#endif
        
        // for js
        std::string tag;
        JS_Function jsCallback = NULL;
    };
    

public:
    NotificationCenter();
    virtual ~NotificationCenter();
    static NotificationCenter* getInstance();
    static void destroyInstance();
    
//    void testCallObjc(id target, void* selector);
//    void addObserver(const char* name, void* observer, callback_string_string callback);
//    void addObservers(const std::vector<std::string>& names, void* observer, callback_string_string callback);
    
private:
    std::map<std::string, std::vector<observer>> _observers;
    
    typedef std::function<bool(observer&)> removecheckFunc;
    void removeObserverByCondition(const char* name, removecheckFunc checker);
    void removeAllObserversByCondition(removecheckFunc checker);
    
    //void list_____list();
    
public:
#if UNIFY_IOS
    // for objc
//    void addObserverWithBlock(const char* name, id observer,IOSCallBackBlock block);
//    void addObserversWithBlock(const std::vector<std::string>& names, id observer,IOSCallBackBlock block);
//    void removeObserverWithBlock(const char *name,id observer);
    //for oc
    void addObserver(const char* name, id target, id objsLastTarget,SEL selector);
    void removeObservers(const char *name,id target,id objsLastTarget);
    
    
    void addObserver(const char* name, id target, SEL selector);
    void addObservers(const std::vector<std::string>& names, id target, SEL selector);
    void removeObserver(const char* name, id target, SEL selector);
    void removeAllObserversForTarget(const char* name, id target);
 
#elif UNIFY_ANDROID
    // for jni
    void addObserverForJni(const char* name, const char* className);
    void addObserversForJni(const std::vector<std::string>& names, const char* className);
    void removeObserverForJni(const char* name, const char* className);
    void removeAllObserversForClassJni(const char* className);
#endif
    
    
    // for js
    void addObserverForJs(const char* name, const char* tag, JS_Function callback);
    void addObserversForJs(const std::vector<std::string>& names, const char* tag, JS_Function callback);
    void removeObserverForJs(const char* name, const char* tag);
    
    // notify
    void notify(const char* name, const char* body = NULL);
};

NS_CE_END

#endif /* defined(__ce__NotificationCenter__) */
