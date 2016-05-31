//
//  js_extensions.h
//  testJS
//
//  Created by kaixuan on 15-6-12.
//  Copyright (c) 2015年 kaixuan. All rights reserved.
//

#ifndef __mujs_extensions__
#define __mujs_extensions__

/*
#if __cplusplus
extern "C" {
#endif*/
    
#include "duktape1.4.0/duktape.h"

void js_add_extra_register_function(duk_function_list_entry extra_funcs[], const char* classname);
void jsopen_extensions(duk_context* ctx);

/*
#if __cplusplus
}
#endif
 */

#endif /* defined(__mujs_extensions__) */
