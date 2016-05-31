//
//  CCStorageUtils.h
//  unifyframework
//
//  Created by Vlad on 4/13/16.
//  Copyright © 2016 vocinno. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface CCStorageUtils : NSObject


+(NSString*)getValueForKey:(NSString*)key withDefaultValue:(NSString*)defaultValue;


@end
