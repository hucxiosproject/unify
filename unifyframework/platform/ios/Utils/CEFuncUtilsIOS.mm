//
//  CEFuncUtilsIOS.m
//  ce
//
//  Created by Jason on 15/9/28.
//  Copyright (c) 2015年 1901. All rights reserved.
//

#import "CEFuncUtilsIOS.h"
#import "AFHTTPRequestOperationManager.h"
#import "CCImageUtils.h"
#import "CCFileUtilsIOS.h"

#include "CEJsEngine.h"

#include "platform/CCJsDebuggerUtils.h"


void OnDebuggerStateChanged(int state, const char* fileName, const char* funcName, int line, int pc)
{
    //Implementation is not ready
}


void OnDebuggerGetLocals(const char* namesArr, const char* valuesArr)
{
    //Implementation is not ready
}


void OnReceiveExceptionNotification(int fatal, const char* msg, const char* fileName, int line)
{
    //Implementation is not ready
}


@implementation CEFuncUtilsIOS

static NSOperationQueue *operationQueue;

+(NSString*)convertArrayToJson:(id)data {
    NSError *error = nil;
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:data
                                                       options:NSJSONWritingPrettyPrinted
                                                         error:&error];
    
    if ([jsonData length] > 0 && error == nil){
        NSString *jsonString = [[NSString alloc] initWithData:jsonData
                                                     encoding:NSUTF8StringEncoding];
        return jsonString;
    }else{
        return nil;
    }
}

+(NSString *)methodsObjToJson:(id)obj{
    NSError *error;
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:obj options:NSJSONWritingPrettyPrinted error:&error];
    NSString *jsonString = [[NSString alloc] initWithData:jsonData
                                                 encoding:NSUTF8StringEncoding];
    if (error) {
        NSLog(@"error = %ld %@",[error code],[error description]);
        
        return @"";
    }
    
    return jsonString;
}

+(NSDictionary *)jsonToDict:(NSString *)json {
    NSDictionary *dic = nil;
    NSError *error = nil;
    NSData *data = [json dataUsingEncoding:NSUTF8StringEncoding];
    id jsonObject = [NSJSONSerialization JSONObjectWithData:data
                                                    options:NSJSONReadingAllowFragments
                                                      error:&error];
    if (jsonObject) {
        dic = (NSDictionary *)jsonObject;
    }
    return dic;
}

+(void)getImage:(NSString*)url imgView:(UIImageView *)imgView {
    NSString *savePath = [self.class getCachesPath];
    NSURL *nsurl = [NSURL URLWithString:url];
    NSString *name = [NSString stringWithFormat:@"img-%lu", (unsigned long)[[nsurl description] hash]];
    NSString *localFileName = [savePath stringByAppendingString:name];
    
    
    
    dispatch_group_t dispatchGroup = dispatch_group_create();
    dispatch_group_async(dispatchGroup, [self getGlobalQueue], ^{
        bool localFileExist = ce::CCFileUtilsIOS::getInstance()->isFileExist([localFileName UTF8String]);
        
        if(localFileExist){
            
            NSData *imageData = nil;
            imageData = [NSData dataWithContentsOfFile:localFileName];
            UIImage *image = [UIImage imageWithData:imageData];
            dispatch_async(dispatch_get_main_queue(), ^{
                [imgView setImage:image];
            });
           
        }else{
            
            AFHTTPRequestOperation *operation = [self.class getOperation:url endBlock:^(bool success, NSString *url, NSData *data) {
                if (success) {
                    //NSString *encodedImageStr = [data base64EncodedStringWithOptions:NSDataBase64Encoding64CharacterLineLength];
                    
                    NSFileManager *fileManage = [NSFileManager defaultManager];
                    [fileManage createFileAtPath:localFileName contents:data attributes:nil];
                    NSData *imageData = nil;
                    imageData = [NSData dataWithContentsOfFile:localFileName];
                    UIImage *image = [UIImage imageWithData:imageData];
                    [imgView setImage:image];
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [imgView setImage:image];
                    });
                } else {
//                    dispatch_async(dispatch_get_main_queue(), ^{
//                        [self callback:@"" msg:@"url error"];
//                    });
                }
            }];
            NSOperationQueue *operationQueue = [self.class getOperationQueue];
            [operationQueue addOperation:operation];
        }

    });
}

+(NSOperationQueue*)getOperationQueue {
    if (!operationQueue) {
        operationQueue = [[NSOperationQueue alloc] init];
    }
    return operationQueue;
}

+(AFHTTPRequestOperation *)getOperation:(NSString*)stringUrl endBlock:(void(^)(bool success, NSString* url, NSData* data))block{
    NSURL *url = [NSURL URLWithString:stringUrl];
    NSURLRequest *request = [NSURLRequest requestWithURL:url];
    AFHTTPRequestOperation *afOperation = [[AFHTTPRequestOperation alloc] initWithRequest:request];
    afOperation.userInfo = [NSDictionary dictionaryWithObjectsAndKeys:stringUrl,@"url", nil];
    
    [afOperation setCompletionBlockWithSuccess:^(AFHTTPRequestOperation *operation, id responseObject) {
        block(true, stringUrl, operation.responseData);
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        block(false, stringUrl, operation.responseData);
    }];
    return afOperation;
}


+(NSString *)getParamsWithDictionary:(NSDictionary *)dic {
    NSError *error;
    NSData *jsonData = [NSJSONSerialization dataWithJSONObject:dic options:NSJSONWritingPrettyPrinted error:&error];
    NSString *json = [[NSString alloc] initWithData:jsonData
                                           encoding:NSUTF8StringEncoding];
    return json;
}

+(NSString *)getCachesPath{
    // 获取Caches目录路径
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *cachesDir = [paths objectAtIndex:0];
    
    return cachesDir;
}

static dispatch_queue_t _globalQueue;
+(dispatch_queue_t) getGlobalQueue {
    if (!_globalQueue) {
        _globalQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    }
    return _globalQueue;
}

/*
+ (void) AttachDebugger
{
    //ce::CEJsEngine::getInstance()->AttachDebugger();
}*/

+ (void) PauseDebugger
{
    #ifdef DUK_OPT_DEBUGGER_SUPPORT
    ce::CEJsEngine::getInstance()->PauseDebugger();
    #endif
}

+ (void) ResumeDebugger
{
    #ifdef DUK_OPT_DEBUGGER_SUPPORT
    ce::CEJsEngine::getInstance()->ResumeDebugger();
     #endif
}


@end