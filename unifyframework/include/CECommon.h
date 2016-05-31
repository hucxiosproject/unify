//
//  CECommon.h
//  ce
//
//  Created by kaixuan on 15-7-3.
//  Copyright (c) 2015年 1901. All rights reserved.
//

#ifndef __ce__CECommon__
#define __ce__CECommon__

#include "CCPlatformMacrosUnify.h"
#include <sstream>
#include <string>

NS_CE_BEGIN

/// The max length of CCLog message.
static const int kMaxLogLen = 16*1024;


void CC_DLL log(const char* format, ...);


template <typename T>
std::string CC_DLL to_string(T value)
{
    std::ostringstream os ;
    os << value ;
    return os.str() ;
}

NS_CE_END


#endif /* defined(__ce__CECommon__) */
