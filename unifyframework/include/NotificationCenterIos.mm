//
//  NotificationCenterIos.m
//  unifyframework
//
//  Created by Vlad on 4/11/16.
//  Copyright © 2016 vocinno. All rights reserved.
//

#import <Foundation/Foundation.h>

#import "NotificationCenterIos.h"

void dispatchInMainThread(notifyFunc f, id object, SEL selector, const char* name, const char* body)
{
    
    NSString* savedName = [[NSString alloc] initWithUTF8String:name];
    NSString* savedBody = [[NSString alloc] initWithUTF8String:body];

    dispatch_async(dispatch_get_main_queue(), ^{
        f(object, selector, [savedName UTF8String], [savedBody UTF8String]);
    });
}