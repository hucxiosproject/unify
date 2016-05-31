//
//  image.m
//  unifyframework
//
//  Created by Jason on 15/8/21.
//  Copyright (c) 2015年 vocinno. All rights reserved.
//

#import "image.h"
#include "duktapeExtra.h"
#include "CCImageUtils.h"
#include "CCFileUtils.h"

static int jsb_image_download(duk_context* ctx) {
    const char* url = duk_to_string(ctx, -3);
    const bool saveCached = duk_to_boolean(ctx, -2);
    if (!duk_is_function(ctx, -1)) {
        CCLOG("arg(3) is't function.");
        return 0;
    }

    JS_Function listener = duk_get_heapptr(ctx, -1);
    ce::CCImageUtils::downloadImage(url, saveCached, listener);
    return 0;
}

static int jsb_image_upload(duk_context* ctx) {
    const char* url = duk_to_string(ctx, -3);
    const char* jsonData = NULL;
    if (duk_is_object(ctx, -2))
    {
        jsonData = duk_json_encode(ctx, -2);
    }
    else if (duk_is_string(ctx, -2))
    {
        jsonData = duk_to_string(ctx, -2);
    }
    
    if (!duk_is_function(ctx, -1)) {
        CCLOG("arg(3) is't function.");
        return 0;
    }
    JS_Function listener = duk_get_heapptr(ctx, -1);
    ce::CCFileUtils::getInstance()->uploadFiles(url, jsonData, listener);
//    ce::CCImageUtils::uploadImages(url, jsonData, listener);
    return 0;
}

static duk_function_list_entry image_funcs[] = {
    { "download", jsb_image_download, 3 },
    { "upload", jsb_image_upload, 3 },
    { NULL, NULL, 0 }
};

int js_register_image(duk_context* ctx) {
    duk_push_global_object(ctx);
    duk_push_object(ctx);
    duk_put_function_list(ctx, -1, image_funcs);
    duk_put_prop_string(ctx, -2, "image");
    duk_pop(ctx);  /* pop global */
    return 1;
}
