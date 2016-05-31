LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := unify_static

LOCAL_MODULE_FILENAME := libunify

LOCAL_SRC_FILES := \
    duktape_extra/CEJsEngine.cpp \
    duktape_extra/duktapeExtra.cpp \
    duktape_extra/fs.cpp \
    duktape_extra/http.cpp \
    duktape_extra/js_extensions.cpp \
    duktape_extra/timerUtils.cpp \
    duktape_extra/image.cpp \
    duktape_extra/tojs/ce_tojs_manual.cpp \
    include/CECommon.cpp \
    include/NotificationCenter.cpp \
    include/PlatformHelper.cpp \
    platform/CCFileUtils.cpp \
    platform/android/CCFileUtilsAndroid.cpp \
    platform/android/CEImageUtilsAndroid.cpp \
    platform/android/CELocalKeyValueUtilsAndroid.cpp \
    platform/android/CETimerUtilsAndroid.cpp \
    platform/android/jni/CEHttpRequestJni.cpp \
    platform/android/jni/CELibHelper.cpp \
    platform/android/jni/JsHelper.cpp \
    platform/android/jni/JniHelper.cpp \
    platform/android/jni/CETimerUtilJni.cpp \
    platform/android/jni/CEImageUtilJni.cpp \
    platform/network/HttpRequestAndroid.cpp \
    support/zip_support/ZipUtils.cpp \
    support/zip_support/ioapi.cpp \
    support/zip_support/ioapi_mem.cpp \
    support/zip_support/unzip.cpp


LOCAL_EXPORT_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/duktape_single \
                    $(LOCAL_PATH)/duktape_extra \
                    $(LOCAL_PATH)/include \
                    $(LOCAL_PATH)/platform \
                    $(LOCAL_PATH)/platform/android \
                    $(LOCAL_PATH)/platform/network \
                    $(LOCAL_PATH)/support/zip_support

LOCAL_C_INCLUDES := $(LOCAL_PATH) \
                    $(LOCAL_PATH)/duktape_single \
                    $(LOCAL_PATH)/duktape_extra \
                    $(LOCAL_PATH)/include \
                    $(LOCAL_PATH)/platform \
                    $(LOCAL_PATH)/platform/android \
                    $(LOCAL_PATH)/platform/network \
                    $(LOCAL_PATH)/support/zip_support

LOCAL_EXPORT_LDLIBS := -llog -lz

LOCAL_WHOLE_STATIC_LIBRARIES := duktape_static


# define the macro to compile through support/zip_support/ioapi.c
LOCAL_CFLAGS := -DUSE_FILE32API $(ANDROID_COCOS2D_BUILD_FLAGS)

LOCAL_EXPORT_CFLAGS := -DUSE_FILE32API

include $(BUILD_STATIC_LIBRARY)

$(call import-module,duktape_single)
