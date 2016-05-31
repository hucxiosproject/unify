#ifndef __CCMACROSUNIFY_H__
#define __CCMACROSUNIFY_H__


#include "CECommon.h"


// @temp code
#define CCAssert(cond, msg) ((void)(cond))



#ifndef CCAssert
#if CE_DEBUG > 0
extern bool CC_DLL cc_assert_script_compatible(const char *msg);
#define CCAssert(cond, msg) do {                              \
      if (!(cond)) {                                          \
        if (!cc_assert_script_compatible(msg) && strlen(msg)) \
          printf("Assert failed: %s\n", msg);                 \
        CC_ASSERT(cond);                                      \
      } \
    } while (0)
#else
#define CCAssert(cond, msg) ((void)(cond))
#endif
#endif  // CCAssert


/// when define returns true it means that our architecture uses big endian
#define CC_HOST_IS_BIG_ENDIAN (bool)(*(unsigned short *)"\0\xff" < 0x100)
#define CC_SWAP32(i)  ((i & 0x000000ff) << 24 | (i & 0x0000ff00) << 8 | (i & 0x00ff0000) >> 8 | (i & 0xff000000) >> 24)
#define CC_SWAP16(i)  ((i & 0x00ff) << 8 | (i &0xff00) >> 8)
#define CC_SWAP_INT32_LITTLE_TO_HOST(i) ((CC_HOST_IS_BIG_ENDIAN == true)? CC_SWAP32(i) : (i) )
#define CC_SWAP_INT16_LITTLE_TO_HOST(i) ((CC_HOST_IS_BIG_ENDIAN == true)? CC_SWAP16(i) : (i) )
#define CC_SWAP_INT32_BIG_TO_HOST(i)    ((CC_HOST_IS_BIG_ENDIAN == true)? (i) : CC_SWAP32(i) )
#define CC_SWAP_INT16_BIG_TO_HOST(i)    ((CC_HOST_IS_BIG_ENDIAN == true)? (i):  CC_SWAP16(i) )


// 1901 added
//#define CCLOG(...)



#endif // __CCMACROS_H__
