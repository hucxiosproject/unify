//
//  CEJsValue.cpp
//  unifyframework
//
//  Created by Vlad on 5/25/16.
//  Copyright © 2016 vocinno. All rights reserved.
//

#include <stdio.h>
#include "CEJsValue.h"


CEJsValue js_to_cejs_value(duk_context* ctx, int index)
{
    if (duk_is_number(ctx, index))
        return JS_NUMBER(duk_to_number(ctx, index));
    
    if (duk_is_boolean(ctx, index))
        return JS_BOOL(duk_to_boolean(ctx, index));
    
    if (duk_is_string(ctx, index))
        return JS_STRING(duk_to_string(ctx, index));
    
    if (duk_is_array(ctx, index))
        return JS_STRING(duk_json_encode(ctx, index));
    
    if (duk_is_object(ctx, index))
        return JS_STRING(duk_json_encode(ctx, index));
    
    if (duk_is_undefined(ctx, index)) {
        return JS_UNDEFINED;
    }
    
    return JS_NULL;
}
