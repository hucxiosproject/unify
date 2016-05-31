//
//  HttpsResult.h
//  unifyframework
//
//  Created by huchangxing on 16/5/16.
//  Copyright © 2016年 vocinno. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface HttpsResult : NSObject{
}


+(void)httpSuccessCallback:(NSURLSessionDataTask *)task responseObject:(id)responseObject resultBlock:(void(^)(int code,unsigned char* bytes,long len))resultBlock;
+(void)httpFailureCallback:(NSURLSessionDataTask *)task error:(NSError*)error;


@end
 