//
//  HttpRequest.cpp
//  ce
//
//  Created by kaixuan on 15-7-9.
//  Copyright (c) 2015年 1901. All rights reserved.
//

#include "HttpRequest.h"
#include "CEJsEngine.h"
#import "ASIHTTPRequest.h"
#import "ASIFormDataRequest.h"



NS_CE_BEGIN


HttpRequest::HttpRequest() : _jsListener(NULL)
{}


HttpRequest::~HttpRequest()
{
    _jsListener = NULL;
}

HttpRequest* HttpRequest::createRequest(const char *aUrl, const char* aHeaderData, const char* aData, const char* method, JS_Function aJsListener) {
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
        NSLog(@"cannot handle method for %s", method);
    }
    return request;
}

void HttpRequest::init(const char *aUrl, const char *aHeaderData, const char *aPostData, int aMethod, JS_Function aJsListener)
{
    _data = aPostData;
    _url = aUrl;
    _headerData = aHeaderData ? std::string(aHeaderData) : "";
    _postData = aPostData ? std::string(aPostData) : "";
    _method = aMethod;
    _jsListener = aJsListener;
    
}


void HttpRequest::start()
{
    NSURL* url = [NSURL URLWithString:[NSString stringWithUTF8String:_url.c_str()]];
    
    id headers = nil;
    id postData = nil;
    
    if (!_headerData.empty())
    {
        NSData* data = [[NSString stringWithUTF8String:_headerData.c_str()] dataUsingEncoding:NSUTF8StringEncoding];
        NSError* error = nil;
        headers = [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:&error];
        if (error)
        {
            NSLog(@"invalid http header, Error: %@", [error description]);
        }
    }
    
    if (!_postData.empty())
    {
        NSData* data = [[NSString stringWithUTF8String:_postData.c_str()] dataUsingEncoding:NSUTF8StringEncoding];
        NSError* error = nil;
        postData = [NSJSONSerialization JSONObjectWithData:data options:kNilOptions error:&error];
        if (error)
        {
            NSLog(@"invalid post data, Error: %@", [error description]);
        }
    }

    __block ASIHTTPRequest* request = nil;
    request = [ASIHTTPRequest requestWithURL:url];
    
    [request addRequestHeader:@"Content-Type" value:@"application/json;charset=UTF-8"];
    
    if (_method == kCEHTTPRequestMethodGET) {
        [request setRequestMethod:@"GET"];
    } else if (_method == kCEHTTPRequestMethodDELETE) {
        [request setRequestMethod:@"DELETE"];
    } else if (_method == kCEHTTPRequestMethodPOST) {
        [request setRequestMethod:@"POST"];
        NSString* httpData = [[NSString alloc ] initWithUTF8String:_data];
        NSData* jsonData = [httpData dataUsingEncoding:NSUTF8StringEncoding];
        [request appendPostData:jsonData];
    } else if (_method == kCEHTTPRequestMethodPUT) {
        [request setRequestMethod:@"PUT"];
        NSString* httpData = [[NSString alloc ] initWithUTF8String:_data];
        NSData* jsonData = [httpData dataUsingEncoding:NSUTF8StringEncoding];
        [request appendPostData:jsonData];
    } else {
        CCLOG("Error: Unsupported method(%d)!", _method);
        return;
    }
    
    for (id key in headers) {
        [request addRequestHeader:key value:[headers objectForKey:key]];
    }
    [request setTimeOutSeconds:30];
    
    [request setCompletionBlock:^{
        int statusCode = [request responseStatusCode];
        NSData* responseData = [request responseData];

        /*
         Need parse JSON to dictionary and then convert back to string
         JSON specification says, that 4-bytes UTF8 sequences such as EMOJI symbol U+1F601 (0xF0 0x9F 0x98 0x80)
         Would be represented in escape format, such as \uD83D \uDC4C. That secuence would lead to crush in JavaScript
         To convert escaped UTF8 string to normal string, we parse JSON, and turn result dictionary back to UTF8 string
        */


        NSData* convertedJsonData = nil;

        NSDictionary* dict = [NSJSONSerialization JSONObjectWithData:responseData options:0 error:NULL];

        if (dict != nil)
        {
            convertedJsonData = [NSJSONSerialization dataWithJSONObject:dict options:0 error:NULL];
        }

        //Failed to recognize JSON - maybe responseData is empty?
        if (convertedJsonData == nil)
        {
            convertedJsonData = responseData;
        }

        unsigned char* bytes = (unsigned char*)[convertedJsonData bytes];
        this->callbackAndCleanup(statusCode, (const char*)bytes, [convertedJsonData length]);

    }];
    
    [request setFailedBlock:^{
        NSError *error = [request error];
        this->callbackAndCleanup(kNetworkErrorCode, [[error description] UTF8String]);
    }];
    
    [request startAsynchronous];
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
    
    //Xperimental
    /*
    duk_context* ctx = CEJsEngine::getInstance()->getJsState();
    duk_push_heapptr(ctx, _jsListener);
    duk_push_int(ctx, statusCode);
    duk_push_lstring(ctx, result, (length == -1) ? strlen(result) : length);
    if (duk_pcall(ctx, 2) != DUK_EXEC_SUCCESS)
    {
        CCLOG("Call js function error: %s", duk_to_string(ctx, -1));
    }
    
    CEJsEngine::getInstance()->clean();
    delete this;*/
}

NS_CE_END