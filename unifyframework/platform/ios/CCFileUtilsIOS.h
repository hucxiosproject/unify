#ifndef __CC_FILEUTILS_IOS_H__
#define __CC_FILEUTILS_IOS_H__

#include "CCFileUtils.h"
#include <string>
#include <vector>
#include "CCPlatformMacrosUnify.h"
#include "common.h"
#include "duktapeExtra.h"

NS_CE_BEGIN

//! @brief  Helper class to handle file operations
class CC_DLL CCFileUtilsIOS : public CCFileUtils
{
public:
    /* override funtions */
    virtual std::string getWritablePath();
    virtual std::string getCachePath();
    virtual bool isFileExist(const std::string& strFilePath);
    virtual bool isDirectoryExist(const std::string& strDirPath);
    virtual bool isAbsolutePath(const std::string& strPath);
    virtual std::string getFullPathForDirectoryAndFilename(const std::string& strDirectory, const std::string& strFilename);
    
    virtual void uploadFiles(const char *url, const char *jsonData, JS_Function listener);
};

NS_CE_END

#endif    // __CC_FILEUTILS_IOS_H__

