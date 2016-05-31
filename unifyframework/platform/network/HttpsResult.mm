//
//  HttpsResult.m
//  unifyframework
//
//  Created by huchangxing on 16/5/16.
//  Copyright © 2016年 vocinno. All rights reserved.
//

#import "HttpsResult.h"

@implementation HttpsResult{
}

+(void)httpSuccessCallback:(NSURLSessionDataTask *)task responseObject:(id)responseObject resultBlock:(void(^)(int code,unsigned char* bytes,long len))resultBlock{
    
    NSHTTPURLResponse* r = (NSHTTPURLResponse*)task.response;
    int  code = (int)r.statusCode;
    
    NSData* convertedJsonData = nil;
    //     NSDictionary* dict = [NSJSONSerialization JSONObjectWithData:responseData options:0 error:NULL];
    //
    if (responseObject != nil)
    {
        convertedJsonData = [NSJSONSerialization dataWithJSONObject:responseObject options:0 error:NULL];
    }
    
    //Failed to recognize JSON - maybe responseData is empty?
    if (convertedJsonData == nil)
    {
        convertedJsonData = [NSJSONSerialization dataWithJSONObject:[NSDictionary dictionary] options:0 error:NULL];;
    }
    
    unsigned char* bytes = (unsigned char*)[convertedJsonData bytes];
    resultBlock(code,bytes,[convertedJsonData length]);
    
 
    
}

+(void)httpSuccessCallback:(NSURLSessionDataTask *)task responseObject:(id)responseObject{

    
    
    //this->callbackAndCleanup(statusCode, (const char*)bytes, [convertedJsonData length]);

}
+(void)httpFailureCallback:(NSURLSessionDataTask *)task error:(NSError*)error{
    
}

@end
