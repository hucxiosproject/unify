//
//  NotificationCenter.h
//  ce
//
//  Created by kaixuan on 15-6-29.
//  Copyright (c) 2015年 1901. All rights reserved.
//

#ifndef __CC_PLATFORM_MACROS_UNIFY_H__
#define __CC_PLATFORM_MACROS_UNIFY_H__

/**
 * define some platform specific macros
 */
#include "CCPlatformConfigUnify.h"
#include "CCPlatformDefineUnify.h"

// namespace ce {}
#ifdef __cplusplus
    #define NS_CE_BEGIN                     namespace ce {
    #define NS_CE_END                       }
    #define USING_NS_CE                     using namespace ce
#else
    #define NS_CE_BEGIN
    #define NS_CE_END
    #define USING_NS_CE
#endif



// CE_DEBUG
#if !defined(CE_DEBUG) || CE_DEBUG == 0
#define CCLOG(...)       do {} while (0)
#else
#define CCLOG(format, ...)      ce::log(format, ##__VA_ARGS__)
#endif // CE_DEBUG



#define CC_SAFE_DELETE(p)            do { if(p) { delete (p); (p) = 0; } } while(0)
#define CC_SAFE_DELETE_ARRAY(p)     do { if(p) { delete[] (p); (p) = 0; } } while(0)
#define CC_SAFE_FREE(p)                do { if(p) { free(p); (p) = 0; } } while(0)
#define CC_SAFE_RELEASE(p)            do { if(p) { (p)->release(); } } while(0)
#define CC_SAFE_RELEASE_NULL(p)        do { if(p) { (p)->release(); (p) = 0; } } while(0)
#define CC_SAFE_RETAIN(p)            do { if(p) { (p)->retain(); } } while(0)
#define CC_BREAK_IF(cond)            if(cond) break


#ifdef __GNUC__
#define CC_UNUSED __attribute__ ((unused))
#else
#define CC_UNUSED
#endif


#endif // __CC_PLATFORM_MACROS_UNIFY_H__
