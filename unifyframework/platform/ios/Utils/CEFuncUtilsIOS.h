//
//  CEFuncUtilsIOS.h
//  ce
//
//  Created by Jason on 15/9/28.
//  Copyright (c) 2015年 1901. All rights reserved.
//

#import <UIKit/UIKit.h>
#import <Foundation/Foundation.h>

@interface CEFuncUtilsIOS : NSObject

+(NSString*)convertArrayToJson:(id)data;
+(NSString *)methodsObjToJson:(id)obj;
+(NSDictionary *)jsonToDict:(NSString *)json;

+(void)getImage:(NSString*)url imgView:(UIImageView *)imgView;
+(NSString *)getParamsWithDictionary:(NSDictionary *)dic;
+(NSString *)getCachesPath;


//+ (void) AttachDebugger;
+ (void) PauseDebugger;
+ (void) ResumeDebugger;
@end
