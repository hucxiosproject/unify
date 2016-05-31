//
//  js_extensions.c
//  testJS
//
//  Created by kaixuan on 15-6-12.
//  Copyright (c) 2015年 kaixuan. All rights reserved.
//

#include "js_extensions.h"
#include "ccMacrosUnify.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <functional>

#include "fs.h"
#include "http.h"
#include "timerUtils.h"
#include "image.h"
#include "tojs/ce_tojs_manual.h"

#include "storage.h"

static duk_ret_t js_print(duk_context* ctx)
{
    int nargs = duk_get_top(ctx);
    std::string t("[JS] ");
    for (int i = 0; i < nargs; i++)
    {
        if (duk_is_string(ctx, i))
            t += duk_get_string(ctx, i);
        else if (duk_is_null(ctx, i))
            t += "null";
        else if (duk_is_undefined(ctx, i))
            t += "undefined";
        else if (duk_is_boolean(ctx, i))
            t += (duk_get_boolean(ctx, i) ? "true" : "false");
        else if (duk_is_function(ctx, i))
            t += "function";
        else if (duk_is_array(ctx, i))
            t += duk_json_encode(ctx, i);
        else if (duk_is_thread(ctx, i))
            t += "thread";
        else if (duk_is_object(ctx, i))
            t += duk_json_encode(ctx, i);
        else if (duk_is_lightfunc(ctx, i))
            t += "lightfunc";
        else
        {
            const char* str = duk_to_string(ctx, i);
            t += (str ? str : "__unknown__");
        }

        if (i!=nargs-1)
            t += " ";
    }

    CCLOG(t.c_str());
    return 0;
}

static const char* setGlobal_js = "this.global = this; this.original_print = this.print;";

static const char* modSearch_js =
    "Duktape.modSearch = function(name) { "
    "    return io.read(name+'.js');"
    "};"
;

struct extra_funcs_pair
{
    duk_function_list_entry* extra_funcs;
    const char* classname;

    extra_funcs_pair(duk_function_list_entry* e, const char* c);
};

extra_funcs_pair::extra_funcs_pair(duk_function_list_entry* e, const char* c)
{
    extra_funcs = e;
    classname = c;
}

std::vector<extra_funcs_pair> extraRegisterFunctions;

void js_add_extra_register_function(duk_function_list_entry extra_funcs[], const char* classname)
{
    extraRegisterFunctions.push_back(extra_funcs_pair(extra_funcs, classname));
}

void jsopen_extensions(duk_context* ctx)
{
    duk_eval_string_noresult(ctx, setGlobal_js);

    duk_push_global_object(ctx);
    duk_push_c_function(ctx, js_print, DUK_VARARGS);
    duk_put_prop_string(ctx, -2 /*idx:global*/, "print");
    duk_pop(ctx);  /* pop global */

    js_register_fs(ctx);
    js_register_http(ctx);
    js_register_utils(ctx);
    js_register_image(ctx);
    js_register_storage(ctx);
    
    for (extra_funcs_pair& pair : extraRegisterFunctions)
    {
        duk_push_global_object(ctx);
        duk_push_object(ctx);
        duk_put_function_list(ctx, -1, pair.extra_funcs);
        duk_put_prop_string(ctx, -2, pair.classname);
        duk_pop(ctx);
    }

    register_all_ce_manual(ctx);
    duk_eval_string_noresult(ctx, modSearch_js);
}