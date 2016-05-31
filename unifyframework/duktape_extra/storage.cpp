//
//  storage.cpp
//  unifyframework
//
//  Created by Vlad on 4/13/16.
//  Copyright © 2016 vocinno. All rights reserved.
//

#include "storage.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "StorageUtils.h"
#include "duktapeExtra.h"

using namespace ce;

static int jsb_saveString(duk_context* ctx) {
    const char* key = duk_to_string(ctx, 0);
    const char* value = duk_to_string(ctx, 1);
    StorageUtils::getStorageUtils()->SetValueForKey(key, value);
    duk_push_boolean(ctx, true);
    return 1;
}

static int jsb_getString(duk_context* ctx) {
    const char* key = duk_to_string(ctx, 0);
    const char* defaultValue = duk_to_string(ctx, 1);
    std::string result = StorageUtils::getStorageUtils()->GetValueForKey(key, defaultValue);
    duk_push_string(ctx, result.c_str());
    return 1;
}

static duk_function_list_entry storage_funcs[] = {
    { "saveString", jsb_saveString, 2},
    { "getString", jsb_getString, 2},
    { NULL, NULL, 0 }
};


int js_register_storage(duk_context* J)
{
    duk_push_global_object(J);
    duk_push_object(J);
    duk_put_function_list(J, -1, storage_funcs);
    duk_put_prop_string(J, -2, "storage");
    duk_pop(J);  /* pop global */
    return 1;
}


