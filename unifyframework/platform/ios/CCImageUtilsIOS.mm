//
//  CCImageUtilsIOS.m
//  unifyframework
//
//  Created by Jason on 15/8/21.
//  Copyright (c) 2015年 vocinno. All rights reserved.
//

#include "CEJsEngine.h"
#import "CCImageUtils.h"
#import "CCFileUtilsIOS.h"
#import <Foundation/Foundation.h>
#import "AFHTTPRequestOperationManager.h"
#include "CEFuncUtilsIOS.h"

#include <cassert>

/////////////////////////////////////
//
// Image downloader
//
/////////////////////////////////////

@interface ImageDownloader : NSObject

@property(assign)JS_Function jsCallback;
@property(strong)NSString* url;
@property(strong)NSString* fileName;

- (void)callback:(NSString*)imageDataString msg:(NSString*)msg;

@end

@implementation ImageDownloader

static NSMutableDictionary *imageDict;
static dispatch_queue_t _globalQueue;
static NSOperationQueue *operationQueue;

- (void)downloadImage:(NSString*)url saveCached:(BOOL)saveCached jsCallback:(JS_Function)jsCallback {
    CCLOG("start download image for ios");
    
    self.url = url;
    self.jsCallback = jsCallback;
    
    NSURL *nsurl = [NSURL URLWithString:url];
    self.fileName = [self getFileName:nsurl];
    NSString *savePath = [self getCachesPath];
    NSString *localFileName = [savePath stringByAppendingString:self.fileName];
    
    dispatch_group_t dispatchGroup = dispatch_group_create();
    dispatch_group_async(dispatchGroup, [self getGlobalQueue], ^{
        NSData *imageData = nil;
        CCLOG("start download image for ios, loca file name is %s", [localFileName UTF8String]);
        bool localFileExist = ce::CCFileUtilsIOS::getInstance()->isFileExist([localFileName UTF8String]);
        if (localFileExist) {
            CCLOG("start download image for ios, local file exist");
            imageData = [NSData dataWithContentsOfFile:localFileName];
//            NSString *encodedImageStr = [imageData base64EncodedStringWithOptions:NSDataBase64Encoding64CharacterLineLength];
//            dispatch_async(dispatch_get_main_queue(), ^{
//                [self callback:encodedImageStr msg:@""];
            dispatch_async(dispatch_get_main_queue(), ^{
                [self callback:@"" msg:@""];

            });
        } else {
            CCLOG("start download image for ios, local file not exist");
            AFHTTPRequestOperation *operation = [self getOperation:url endBlock:^(bool success, NSString *url, NSData *data) {
                if (success) {
                    //NSString *encodedImageStr = [data base64EncodedStringWithOptions:NSDataBase64Encoding64CharacterLineLength];
                    if (saveCached) {
                        NSFileManager *fileManage = [NSFileManager defaultManager];
                        [fileManage createFileAtPath:localFileName contents:data attributes:nil];
                    }
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [self callback:@"" msg:@""];
                    });
                } else {
                    dispatch_async(dispatch_get_main_queue(), ^{
                        [self callback:@"" msg:@"url error"];
                    });
                }
            }];
            NSOperationQueue *operationQueue = [self getOperationQueue];
            [operationQueue addOperation:operation];
        }
        
//            if (imageData) {
//                //if savePath, save imageData to local
//                if (saveCached && !localFileExist) {
//                    NSFileManager *fileManage = [NSFileManager defaultManager];
//                    [fileManage createFileAtPath:localFileName contents:imageData attributes:nil];
//                }
//                
//                NSString *encodedImageStr = [imageData base64EncodedStringWithOptions:NSDataBase64Encoding64CharacterLineLength];
//                dispatch_async(dispatch_get_main_queue(), ^{
//                    [self callback:encodedImageStr msg:@""];
//                });
//                
//            } else {
//                dispatch_async(dispatch_get_main_queue(), ^{
//                    [self callback:nil msg:@"url error"];
//                });
//            }
//            
//            // set value nil in imageDict
//            [self performSelectorOnMainThread:@selector(removeValueForKey) withObject:nil waitUntilDone:false];
    });

}

- (void)removeValueForKey {
//    [[self getImageDict] removeObjectForKey:self.fileName];
}

- (void)callback:(NSString*)imageDataString msg:(NSString*)msg {
    
    //Xperimental
    assert(false);
    
    
    
    /*
    duk_context* ctx = ce::CEJsEngine::getInstance()->getJsState();
    duk_push_heapptr(ctx, [self jsCallback]);
    if ([msg isEqualToString:@""]) {
        duk_push_true(ctx);
    } else {
        duk_push_false(ctx);
    }
    duk_push_string(ctx, [self.url UTF8String]);
    duk_push_string(ctx, [imageDataString UTF8String]);
    duk_push_string(ctx, [msg UTF8String]);
    if (duk_pcall(ctx, 4) != DUK_EXEC_SUCCESS)
    {
        CCLOG("Call js function error: %s", duk_to_string(ctx, -1));
    }
    
    ce::CEJsEngine::getInstance()->clean();*/
}

- (NSString*)getFileName:(NSURL*)url {
    NSString *name = [NSString stringWithFormat:@"img-%lu", (unsigned long)[[url description] hash]];
    return name;
}

- (NSMutableDictionary *) getImageDict {
    if (!imageDict) {
        imageDict = [NSMutableDictionary dictionary];
    }
    return imageDict;
}
- (dispatch_queue_t) getGlobalQueue {
    if (!_globalQueue) {
        _globalQueue = dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT, 0);
    }
    return _globalQueue;
}

-(NSString *)getCachesPath{
    // 获取Caches目录路径
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *cachesDir = [paths objectAtIndex:0];
    
    return cachesDir;
}

-(NSOperationQueue*)getOperationQueue {
    if (!operationQueue) {
        operationQueue = [[NSOperationQueue alloc] init];
    }
    return operationQueue;
}

-(AFHTTPRequestOperation *)getOperation:(NSString*)stringUrl endBlock:(void(^)(bool success, NSString* url, NSData* data))block{
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

@end

/////////////////////////////////////
//
// Image uploader
//
/////////////////////////////////////
@interface ImageUploader : NSObject

@property(assign)JS_Function jsCallback;
@property(strong)NSString* url;
@property(strong)NSMutableArray* images;

-(void)uploadImages:(NSString*)url images:(NSMutableArray*)images params:(NSDictionary*)params callback:(JS_Function)callback;

@end

@implementation ImageUploader

-(void)uploadImages:(NSString*)url images:(NSMutableArray*)images params:(NSDictionary*)params callback:(JS_Function)callback {
    self.jsCallback = callback;
    self.url = url;
    self.images = images;
    
    AFHTTPRequestOperationManager *manager = [AFHTTPRequestOperationManager manager];
    NSMutableDictionary *dicForRequest = [[NSMutableDictionary alloc] init];
    NSEnumerator *enumerator = [params keyEnumerator];
    NSString *key;
    while (key = [enumerator nextObject]) {
        NSString *value = [params objectForKey:key];
        [dicForRequest setValue:value forKey:key];
    }
    NSMutableArray *imageArray = [[NSMutableArray alloc]init];
    NSMutableDictionary *imageNameDic = [[NSMutableDictionary alloc]init];
    [manager POST:url parameters:dicForRequest constructingBodyWithBlock:^(id<AFMultipartFormData> formData) {
        for (NSDictionary *image in images) {
            NSString *filePath = [image objectForKey:@"path"];
            bool localFileExist = ce::CCFileUtilsIOS::getInstance()->isFileExist([filePath UTF8String]);
            if (!localFileExist) {
                NSLog(@"image not found when upload, image path is %@", filePath);
                return;
            }
            UIImage *uiimage = [UIImage imageNamed:filePath];
//            NSData *data = UIImageJPEGRepresentation(uiimage, 1.0);
            NSData *data = UIImagePNGRepresentation(uiimage);
            
            NSString *fileName = [NSString stringWithFormat:@"img-%lu", (unsigned long)[filePath hash]];
            NSString *mimeType = [self getMimeTypeForImage:data];
            [formData appendPartWithFileData:data name:filePath fileName:fileName mimeType:mimeType];
            [imageNameDic setValue: filePath forKey:@"filePath"];
            [imageNameDic setValue: fileName forKey:@"fileName"];
            [imageArray addObject:imageNameDic ];
        }
    } success:^(AFHTTPRequestOperation *operation, id responseObject) {
//        NSMutableArray *arr = [[NSMutableArray alloc] initWithObjects:responseObject];
//        NSDictionary *data = [[NSDictionary alloc] initWithObjectsAndKeys:responseObject];
//        NSMutableDictionary *dic;
        
//        for (int i = 0; i < arr.count; i ++) {
//            dic =[[NSMutableDictionary alloc]initWithDictionary:arr[i]];
//            for (int j = 0; j < imageArray.count; j++) {
//                if([[dic objectForKey:@"name"] isEqualToString:[imageArray[j] objectForKey:@"fileName"]]){
//                    NSString *str = [imageArray[j] objectForKey:@"filePath"];
//                    [dic setValue:str forKey:@"name"] ;
//                    [arr replaceObjectAtIndex:i withObject:dic];
//                }
//            }
//        }
//        
//        NSString *response = [CEFuncUtilsIOS convertArrayToJson:arr];
        [self callback:200 response:responseObject];
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        NSLog(@"image upload error1 %ld", (long)[error code]);
        NSLog(@"image upload error2 %@", [error userInfo]);
        [self callback:-1 response:[error localizedDescription]];
    }];
}

- (void)callback:(int)statusCode response:(NSString*)response {
    
    assert(false);
    
    //Xperimental
    
    /*
    duk_context* ctx = ce::CEJsEngine::getInstance()->getJsState();
    duk_push_heapptr(ctx, [self jsCallback]);
    duk_push_int(ctx, statusCode);
    duk_push_string(ctx, [response UTF8String]);
    if (duk_pcall(ctx, 2) != DUK_EXEC_SUCCESS)
    {
        CCLOG("Call js function error: %s", duk_to_string(ctx, -1));
    }
    
    ce::CEJsEngine::getInstance()->clean();*/
}

- (NSString *) getMimeTypeForImage:(NSData*)data {
    uint8_t c;
    [data getBytes:&c length:1];
    
    switch (c) {
        case 0xFF:
            return @"image/jpeg";
        case 0x89:
            return @"image/png";
        case 0x47:
            return @"image/gif";
        case 0x49:
        case 0x4D:
            return @"image/tiff";
    }
    return nil;
}

@end

/////////////////////////////////////
//
// C++
//
/////////////////////////////////////

NS_CE_BEGIN

void CCImageUtils::downloadImage(const char *url, const bool saveCached, JS_Function listener) {
    NSString *urlString = [NSString stringWithUTF8String:url];
    ImageDownloader *downloader = [[ImageDownloader alloc] init];
    [downloader downloadImage:urlString saveCached:saveCached jsCallback:listener];
    return;
}

void CCImageUtils::uploadImages(const char *url, const char *jsonData, JS_Function listener) {
    NSData *uploadImages = [[NSString stringWithUTF8String:jsonData] dataUsingEncoding:NSASCIIStringEncoding];
    NSError *error = nil;
    id jsonObject = [NSJSONSerialization JSONObjectWithData:uploadImages
                                                    options:NSJSONReadingAllowFragments
                                                      error:&error];
    ImageUploader *uploader = [[ImageUploader alloc] init];
    if (jsonObject) {
        NSDictionary *dic = (NSDictionary *)jsonObject;
        NSMutableArray *images = [dic objectForKey:@"images"];
        NSDictionary *paramsDic = [dic objectForKey:@"params"];
        [uploader uploadImages:[NSString stringWithUTF8String:url] images:images params:paramsDic callback:listener];
    } else {
        CCLOG("convert %s to nsdict error", [NSString stringWithUTF8String:jsonData]);
        [uploader callback:false response:[NSString stringWithUTF8String:jsonData]];
    }
}

NS_CE_END
