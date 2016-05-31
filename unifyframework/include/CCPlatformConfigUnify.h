#ifndef __CC_PLATFORM_CONFIG_UNIFY_H__
#define __CC_PLATFORM_CONFIG_UNIFY_H__


//////////////////////////////////////////////////////////////////////////
// configure
//////////////////////////////////////////////////////////////////////////

// define supported target platform macro which CC uses.
#define CC_PLATFORM_UNKNOWN            0
#define CC_PLATFORM_IOS                1
#define CC_PLATFORM_ANDROID            2
#define CC_PLATFORM_MAC                3

#define UNIFY_IOS                   false
#define UNIFY_ANDROID               false

// Determine target platform by compile environment macro.
#define CC_TARGET_PLATFORM             CC_PLATFORM_UNKNOWN

// mac
#if defined(CC_TARGET_OS_MAC)
#undef  CC_TARGET_PLATFORM
#define CC_TARGET_PLATFORM         CC_PLATFORM_MAC
#endif

// ios
#if defined(CC_TARGET_OS_IPHONE)
    #undef  CC_TARGET_PLATFORM
    #define CC_TARGET_PLATFORM         CC_PLATFORM_IOS
	#undef UNIFY_IOS
	#define UNIFY_IOS true
#endif

// android
#if defined(ANDROID)
    #undef  CC_TARGET_PLATFORM
    #define CC_TARGET_PLATFORM         CC_PLATFORM_ANDROID
	#undef UNIFY_ANDROID
	#define UNIFY_ANDROID true
#endif


//#if TARGET_OS_IPHONE || TARGET_IPHONE_SIMULATOR
//    #undef UNIFY_IOS
//    #define UNIFY_IOS true
//#else
//    //check user set platform
//    #if ! CC_TARGET_PLATFORM
//        #error  "Cannot recognize the target platform; are you targeting an unsupported platform?"
//    #else
//        #if CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID
//            #undef UNIFY_ANDROID
//            #define UNIFY_ANDROID true
//        #endif
//    #endif
//#endif





#endif  // __CC_PLATFORM_CONFIG_UNIFY_H__

