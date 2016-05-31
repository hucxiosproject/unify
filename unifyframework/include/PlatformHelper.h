//
//  PlatformHelper.h
//  ce
//
//  Created by kaixuan on 15-6-23.
//  Copyright (c) 2015年 kaixuan. All rights reserved.
//

#ifndef __ce__PlatformHelper__
#define __ce__PlatformHelper__

#include <iostream>
#include "common.h"

NS_CE_BEGIN

class PlatformHelper
{
public:
    
    static int getCurrentLanguage();
    
    static int getTargetPlatform();
};

NS_CE_END

#endif /* defined(__ce__PlatformHelper__) */
