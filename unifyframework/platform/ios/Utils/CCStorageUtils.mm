//
//  CCStorageUtils.m
//  unifyframework
//
//  Created by Vlad on 4/13/16.
//  Copyright © 2016 vocinno. All rights reserved.
//

#import "CCStorageUtils.h"

#include "platform/StorageUtils.h"



@implementation CCStorageUtils


+(NSString*)getValueForKey:(NSString*)key withDefaultValue:(NSString*)defaultValue
{
    
    std::string result = ce::StorageUtils::getStorageUtils()->GetValueForKey([key UTF8String], [defaultValue UTF8String]);
    
    NSString* resultStr = [[NSString alloc] initWithUTF8String:result.c_str()];
    
    return resultStr;
    
}

@end
