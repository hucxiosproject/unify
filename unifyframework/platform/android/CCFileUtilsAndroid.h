#ifndef __CC_FILEUTILS_ANDROID_H__
#define __CC_FILEUTILS_ANDROID_H__

#include "platform/CCFileUtils.h"
#include "include/CCPlatformMacrosUnify.h"
#include "duktapeExtra.h"
#include <string>
#include <vector>

NS_CE_BEGIN

//! @brief  Helper class to handle file operations
class CC_DLL CCFileUtilsAndroid : public CCFileUtils
{
    friend class CCFileUtils;
    CCFileUtilsAndroid();
public:
    virtual ~CCFileUtilsAndroid();

    /* override funtions */
    bool init();
    virtual unsigned char* getFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize);
    virtual std::string getWritablePath();
    virtual bool isFileExist(const std::string& strFilePath);
    virtual bool isDirectoryExist(const std::string& strDirPath);
    virtual bool isAbsolutePath(const std::string& strPath);

    virtual void uploadFiles(const char *url, const char *jsonData, JS_Function listener);
    
private:
    unsigned char* doGetFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize, bool forAsync);
};

NS_CE_END

#endif    // __CC_FILEUTILS_ANDROID_H__

