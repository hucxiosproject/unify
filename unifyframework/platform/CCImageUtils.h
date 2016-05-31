//
//  CCImageUtils.h
//  unifyframework
//
//  Created by Jason on 15/8/21.
//  Copyright (c) 2015年 vocinno. All rights reserved.
//

#ifndef __ce__ImageUtils__
#define __ce__ImageUtils__

#include "common.h"
#include "duktapeExtra.h"

NS_CE_BEGIN

class CCImageUtils
{
public:
    static void uploadImages(const char *url, const char *jsonData, JS_Function listener);
    static void downloadImage(const char *url, const bool saveCached, JS_Function listener);
    static void getLocalImagePath(const char *url, JS_Function listener);
};

NS_CE_END

#endif /* defined(__ce__Utils__) */
