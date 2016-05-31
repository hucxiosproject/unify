//
//  CELocalKeyValueUtils.h
//  unifyframework
//
//  Created by Jason on 15/9/23.
//  Copyright (c) 2015年 vocinno. All rights reserved.
//

#ifndef __ce__local_key_value_utils__
#define __ce__local_key_value_utils__

#include <string.h>
#include "common.h"
#include "duktapeExtra.h"

NS_CE_BEGIN

class CELocalKeyValueUtils
{
public:
    static bool saveInteger(const char* key, int value);
    static bool saveString(const char* key, const char* value);
    static int getInteger(const char* key, int defaultValue);
    static std::string getString(const char* key, const char* defaultValue);
};

NS_CE_END

#endif
