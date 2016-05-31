//
//  HttpRequestAndroid.cpp
//  ce
//
//  Created by kaixuan on 15-7-9.
//  Copyright (c) 2015年 1901. All rights reserved.
//

#include "HttpRequest.h"
#include "CEJsEngine.h"
#include "jni/CEHttpRequestJni.h"



NS_CE_BEGIN


HttpRequest::HttpRequest() : _jsListener(NULL)
{}


HttpRequest::~HttpRequest()
{
    _jsListener = NULL;
}

HttpRequest* HttpRequest::createRequest(const char *aUrl, const char* aHeaderData, const char* aData, const char* method, JS_Function aJsListener)
{
    HttpRequest* request = new HttpRequest();
    std::string methodName = method;
    if (methodName == "get") {
        request->init(aUrl, aHeaderData, aData, kCEHTTPRequestMethodGET, aJsListener);
    } else if (methodName == "delete") {
        request->init(aUrl, aHeaderData, aData, kCEHTTPRequestMethodDELETE, aJsListener);
    } else if (methodName == "post") {
        request->init(aUrl, aHeaderData, aData, kCEHTTPRequestMethodPOST, aJsListener);
    } else if (methodName == "put") {
        request->init(aUrl, aHeaderData, aData, kCEHTTPRequestMethodPUT, aJsListener);
    } else {
        CCLOG("cannot handle method for %s", method);
    }
    return request;
}

void HttpRequest::init(const char *aUrl, const char *aHeaderData, const char *aPostData, int aMethod, JS_Function aJsListener)
{
    _url = aUrl;
    _headerData = aHeaderData ? std::string(aHeaderData) : "";
    _postData = aPostData ? std::string(aPostData) : "";
    _method = aMethod;
    _jsListener = aJsListener;
}

void HttpRequest::start()
{
    if (_method == kCEHTTPRequestMethodGET)
    {
        httpGetJni(this);
    }
    else if (_method == kCEHTTPRequestMethodPOST)
    {
        httpPostJni(this);
    }
    else if (_method == kCEHTTPRequestMethodDELETE)
    {
        httpDeleteJni(this);
    }
    else if (_method == kCEHTTPRequestMethodPUT)
    {
        httpPutJni(this);
    }
    else
    {
        CCLOG("Error: Unsupported method(%d)!", _method);
        return;
    }
}


void HttpRequest::callbackAndCleanup(int statusCode, const char *result, size_t length)
{
    std::string s;

    if (length == -1)
    {
        s = std::string(result);
    }
    else
    {
        s = std::string(&result[0], &result[0] + length);
    }

    CEJsEngine::getInstance()->callback(_jsListener, statusCode, s.c_str());
    
    delete this;
}


NS_CE_END