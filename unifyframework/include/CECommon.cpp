//
//  CECommon.cpp
//  ce
//
//  Created by kaixuan on 15-7-3.
//  Copyright (c) 2015年 1901. All rights reserved.
//

#include "CECommon.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
#include <android/log.h>
#include <jni.h>
#endif


NS_CE_BEGIN

static char logbuff[sizeof(char) * (kMaxLogLen + 1)];

#if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID

void log(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    memset(logbuff, 0, sizeof(logbuff));
    vsnprintf(logbuff, kMaxLogLen, format, args);
    va_end(args);
    __android_log_print(ANDROID_LOG_DEBUG, "unify", "%s", logbuff);
}

#else

void log(const char* format, ...)
{
    printf("unify: ");
    va_list ap;
    va_start(ap, format);
    memset(logbuff, 0, sizeof(logbuff));
    vsnprintf(logbuff, kMaxLogLen, format, ap);
    va_end(ap);
    printf("%s", logbuff);
    printf("\n");
    fflush(stdout);
}

#endif



NS_CE_END