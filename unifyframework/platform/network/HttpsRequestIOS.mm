//
//  HttpRequest.cpp
//  ce
//
//  Created by kaixuan on 15-7-9.
//  Copyright (c) 2015年 1901. All rights reserved.
//

#include "HttpsRequest.h"
#include "CEJsEngine.h"
#include "CEFuncUtilsIOS.h"
#include "AFHTTPSessionManager.h"
#include "HttpsResult.h"


NS_CE_BEGIN


HttpsRequest::HttpsRequest() : _jsListener(NULL)
{}


HttpsRequest::~HttpsRequest()
{
    _jsListener = NULL;
}

HttpsRequest* HttpsRequest::createRequest(const char *aUrl, const char* aHeaderData, const char* aData, const char* method, JS_Function aJsListener) {
    HttpsRequest* request = new HttpsRequest();
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

void HttpsRequest::init(const char *aUrl, const char *aHeaderData, const char *aPostData, int aMethod, JS_Function aJsListener)
{
    _data = aPostData;
    _url = aUrl;
    _headerData = aHeaderData ? std::string(aHeaderData) : "";
    _postData = aPostData ? std::string(aPostData) : "";
    _method = aMethod;
    _jsListener = aJsListener;
    
}


void HttpsRequest::start()
{
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

 
    
    
    if (_method == kCEHTTPRequestMethodGET) {
        this->httpsGet();
     } else if (_method == kCEHTTPRequestMethodDELETE) {
         this->httpsDelete();
     } else if (_method == kCEHTTPRequestMethodPOST) {
         this->httpsPost();
    } else if (_method == kCEHTTPRequestMethodPUT) {
        this->httpsPut();
    } else {
        CCLOG("Error: Unsupported method(%d)!", _method);
        return;
    }

}


void HttpsRequest::callbackAndCleanup(int statusCode, const char *result, size_t length)
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



void HttpsRequest::httpsPost(){
    
    
    NSString *baseURL = [NSString stringWithUTF8String:_url.c_str()];

    AFHTTPSessionManager *manager = [AFHTTPSessionManager manager];
    NSString *postString = [NSString stringWithUTF8String:_postData.c_str()];
    NSDictionary *parameters = [CEFuncUtilsIOS jsonToDict:postString];
    
    manager.requestSerializer = [AFJSONRequestSerializer serializer];
    [manager.requestSerializer setValue:@"application/json;charset=UTF-8" forHTTPHeaderField:@"Content-Type"];
    [manager.requestSerializer setTimeoutInterval:30.0];

    manager.responseSerializer = [AFJSONResponseSerializer serializer];
    [manager POST:baseURL parameters:parameters success:^(NSURLSessionDataTask *task, id responseObject) {
        
        [HttpsResult httpSuccessCallback:task responseObject:responseObject resultBlock:^(int code, unsigned char *bytes, long len) {
            this->callbackAndCleanup(code, (const char*)bytes, len);

        }];
        
        
    } failure:^(NSURLSessionDataTask *task, NSError *error) {
        this->callbackAndCleanup(kNetworkErrorCode, [[error description] UTF8String]);
    }];
}

void HttpsRequest::httpsGet(){
     
    
    NSString *baseURL = [NSString stringWithUTF8String:_url.c_str()];
    
    AFHTTPSessionManager *manager = [AFHTTPSessionManager manager];
    NSString *postString = [NSString stringWithUTF8String:_postData.c_str()];
    NSDictionary *parameters = [CEFuncUtilsIOS jsonToDict:postString];
    
    manager.requestSerializer = [AFJSONRequestSerializer serializer];
    [manager.requestSerializer setValue:@"application/json;charset=UTF-8" forHTTPHeaderField:@"Content-Type"];
    [manager.requestSerializer setTimeoutInterval:30.0];
     
    manager.responseSerializer = [AFJSONResponseSerializer serializer];
    [manager GET:baseURL parameters:parameters success:^(NSURLSessionDataTask *task, id responseObject) {
        
        [HttpsResult httpSuccessCallback:task responseObject:responseObject resultBlock:^(int code, unsigned char *bytes, long len) {
            this->callbackAndCleanup(code, (const char*)bytes, len);
            
        }];
    } failure:^(NSURLSessionDataTask *task, NSError *error) {
        this->callbackAndCleanup(kNetworkErrorCode, [[error description] UTF8String]);
    }];
}
void HttpsRequest::httpsPut(){
    NSString *baseURL = [NSString stringWithUTF8String:_url.c_str()];
    
    AFHTTPSessionManager *manager = [AFHTTPSessionManager manager];
    NSString *postString = [NSString stringWithUTF8String:_postData.c_str()];
    NSDictionary *parameters = [CEFuncUtilsIOS jsonToDict:postString];
    
    manager.requestSerializer = [AFJSONRequestSerializer serializer];
    [manager.requestSerializer setValue:@"application/json;charset=UTF-8" forHTTPHeaderField:@"Content-Type"];
    [manager.requestSerializer setTimeoutInterval:30.0];
    
    manager.responseSerializer = [AFJSONResponseSerializer serializer];
    [manager PUT:baseURL parameters:parameters success:^(NSURLSessionDataTask *task, id responseObject) {
        
        [HttpsResult httpSuccessCallback:task responseObject:responseObject resultBlock:^(int code, unsigned char *bytes, long len) {
            this->callbackAndCleanup(code, (const char*)bytes, len);
            
        }];
    } failure:^(NSURLSessionDataTask *task, NSError *error) {
        this->callbackAndCleanup(kNetworkErrorCode, [[error description] UTF8String]);
     }];
}

void HttpsRequest::httpsDelete(){
    NSString *baseURL = [NSString stringWithUTF8String:_url.c_str()];
    
    AFHTTPSessionManager *manager = [AFHTTPSessionManager manager];
    NSString *postString = [NSString stringWithUTF8String:_postData.c_str()];
    NSDictionary *parameters = [CEFuncUtilsIOS jsonToDict:postString];
    
    manager.requestSerializer = [AFJSONRequestSerializer serializer];
    [manager.requestSerializer setValue:@"application/json;charset=UTF-8" forHTTPHeaderField:@"Content-Type"];
    [manager.requestSerializer setTimeoutInterval:30.0];
    
    manager.responseSerializer = [AFJSONResponseSerializer serializer];
    [manager DELETE:baseURL parameters:parameters success:^(NSURLSessionDataTask *task, id responseObject) {
        
        [HttpsResult httpSuccessCallback:task responseObject:responseObject resultBlock:^(int code, unsigned char *bytes, long len) {
            this->callbackAndCleanup(code, (const char*)bytes, len);
            
        }];
    } failure:^(NSURLSessionDataTask *task, NSError *error) {
        this->callbackAndCleanup(kNetworkErrorCode, [[error description] UTF8String]);
     }];
}
 
NS_CE_END