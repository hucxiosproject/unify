//
//  CELocalKeyValueUtilsIOS.m
//  unifyframework
//
//  Created by Jason on 15/9/23.
//  Copyright (c) 2015年 vocinno. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "CELocalKeyValueUtils.h"

NS_CE_BEGIN

bool CELocalKeyValueUtils::saveInteger(const char* key, int value) {
    NSString* keyString = [NSString stringWithUTF8String:key];
    [[NSUserDefaults standardUserDefaults] setInteger:value forKey:keyString];
    [[NSUserDefaults standardUserDefaults] synchronize];
    return true;
}

bool CELocalKeyValueUtils::saveString(const char *key, const char *value) {
    NSString* keyString = [NSString stringWithUTF8String:key];
    NSString* valueString = [NSString stringWithUTF8String:value];
    [[NSUserDefaults standardUserDefaults] setValue:valueString forKey:keyString];
    [[NSUserDefaults standardUserDefaults] synchronize];
    return true;
}

int CELocalKeyValueUtils::getInteger(const char *key, int defaultValue) {
    NSString* keyString = [NSString stringWithUTF8String:key];
    NSInteger value = NAN;
    @try {
        if ([[[[NSUserDefaults standardUserDefaults] dictionaryRepresentation] allKeys] containsObject:keyString]) {
            value = [[[NSUserDefaults standardUserDefaults] valueForKey:keyString] integerValue];
        } else {
            value = defaultValue;
        }
    }
    @catch (NSException *exception) {
        NSLog(@"cannot get int for %@, reason is %@", keyString, [exception reason]);
        value = defaultValue;
    }
    @finally {

    }
    return (int)value;
}

std::string CELocalKeyValueUtils::getString(const char *key, const char* defaultValue) {
    NSString* keyString = [NSString stringWithUTF8String:key];
    NSString* value = NULL;
    @try {
        if ([[[[NSUserDefaults standardUserDefaults] dictionaryRepresentation] allKeys] containsObject:keyString]) {
            value = [[NSUserDefaults standardUserDefaults] stringForKey:keyString];
        } else {
            value = [NSString stringWithUTF8String:defaultValue];
        }
    }
    @catch (NSException *exception) {
        NSLog(@"cannot get string for %@, reason is %@", keyString, [exception reason]);
        value = [NSString stringWithUTF8String:defaultValue];
    }
    @finally {
        
    }
    std::string valueString = *new std::string([value UTF8String]);
    return valueString;
}

NS_CE_END
