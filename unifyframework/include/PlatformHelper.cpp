//
//  PlatformHelper.cpp
//  ce
//
//  Created by kaixuan on 15-6-23.
//  Copyright (c) 2015年 kaixuan. All rights reserved.
//

#include "PlatformHelper.h"
#include "common.h"

NS_CE_BEGIN;

int PlatformHelper::getCurrentLanguage()
{
    return 1;
}

int PlatformHelper::getTargetPlatform()
{
    return CC_TARGET_PLATFORM;
}

NS_CE_END