//
//  HttpRequest.h
//  ce
//
//  Created by kaixuan on 15-7-9.
//  Copyright (c) 2015年 1901. All rights reserved.
//

#ifndef __ce__HttpRequest__
#define __ce__HttpRequest__

#include <stdio.h>
#include "common.h"
#include "duktapeExtra.h"

#define kCEHTTPRequestMethodGET                 0
#define kCEHTTPRequestMethodPOST                1
#define kCEHTTPRequestMethodPUT                 2
#define kCEHTTPRequestMethodDELETE              3

// defined network error code
#define kNetworkErrorCode   -1


NS_CE_BEGIN

class HttpRequest
{
private:
    std::string _url;
    std::string _headerData;
    std::string _postData;
    int _method;
    JS_Function _jsListener;
    
    const char* _data; // for chinese
    
public:
    HttpRequest();
    virtual ~HttpRequest();
    
    static HttpRequest* createRequest(const char* aUrl,
                                      const char* aHeader = NULL, // json format
                                      const char* aData = NULL,     // json format
                                      const char* method = NULL,
                                      JS_Function aJsListener = NULL);
    
    void start();
    
    const std::string& getUrl() const { return _url; }
    const std::string& getHeader() const { return _headerData; }
    const std::string& getPostData() const { return _postData; }
    const JS_Function getJSListener() const { return _jsListener; }
    
    
    void callbackAndCleanup(int statusCode, const char* result, size_t length = -1);
    
private:
    void init(const char* aUrl, const char* aHeaderData, const char* aPostData, int aMethod, JS_Function aJsListener);
};

NS_CE_END

#endif /* defined(__ce__HttpRequest__) */
