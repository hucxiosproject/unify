#include "CCFileUtilsAndroid.h"
#include "support/zip_support/ZipUtils.h"
#include "include/CECommon.h"
#include "jni/CELibHelper.h"
#include "duktapeExtra.h"
#include "platform/CCImageUtils.h"

using namespace std;

NS_CE_BEGIN

// record the zip on the resource path
static ZipFile *s_pZipFile = NULL;

CCFileUtils* CCFileUtils::getInstance()
{
    if (_instance == NULL)
    {
        _instance = new CCFileUtilsAndroid();
        _instance->init();
        std::string resourcePath = getApkPath();
        s_pZipFile = new ZipFile(resourcePath, "assets/");
    }
    return _instance;
}

CCFileUtilsAndroid::CCFileUtilsAndroid()
{
}

CCFileUtilsAndroid::~CCFileUtilsAndroid()
{
    CC_SAFE_DELETE(s_pZipFile);
}

bool CCFileUtilsAndroid::init()
{
    m_strDefaultResRootPath = "assets/";
    return CCFileUtils::init();
}

bool CCFileUtilsAndroid::isFileExist(const std::string& strFilePath)
{
    if (0 == strFilePath.length())
    {
        return false;
    }

    bool bFound = false;
    
    // Check whether file exists in apk.
    if (strFilePath[0] != '/')
    {
        std::string strPath = strFilePath;
        if (strPath.find(m_strDefaultResRootPath) != 0)
        {// Didn't find "assets/" at the beginning of the path, adding it.
            strPath.insert(0, m_strDefaultResRootPath);
        }

        if (s_pZipFile->fileExists(strPath))
        {
            bFound = true;
        } 
    }
    else
    {
        FILE *fp = fopen(strFilePath.c_str(), "r");
        if(fp)
        {
            bFound = true;
            fclose(fp);
        }
    }
    return bFound;
}

bool CCFileUtilsAndroid::isDirectoryExist(const std::string& strDirPath)
{
    return inDirectoryExistsJNI(strDirPath.c_str());
}

bool CCFileUtilsAndroid::isAbsolutePath(const std::string& strPath)
{
    // On Android, there are two situations for full path.
    // 1) Files in APK, e.g. assets/path/path/file.png
    // 2) Files not in APK, e.g. /data/data/org.cocos2dx.hellocpp/cache/path/path/file.png, or /sdcard/path/path/file.png.
    // So these two situations need to be checked on Android.
    if (strPath[0] == '/' || strPath.find(m_strDefaultResRootPath) == 0)
    {
        return true;
    }
    return false;
}


unsigned char* CCFileUtilsAndroid::getFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize)
{
    return doGetFileData(pszFileName, pszMode, pSize, false);
}

    
unsigned char* CCFileUtilsAndroid::doGetFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize, bool forAsync)
{
    unsigned char * pData = 0;
    
    if ((! pszFileName) || (! pszMode) || 0 == strlen(pszFileName))
    {
        return 0;
    }
    
    string fullPath = fullPathForFilename(pszFileName);
    
    if (fullPath[0] != '/')
    {
       if (forAsync)
       {
           // pData = s_pZipFile->getFileData(fullPath.c_str(), pSize, s_pZipFile->_dataThread);
       }
       else
       {
           pData = s_pZipFile->getFileData(fullPath.c_str(), pSize);
       }
    }
    else
    {
        do
        {
            // read rrom other path than user set it
	        //CCLOG("GETTING FILE ABSOLUTE DATA: %s", pszFileName);
            FILE *fp = fopen(fullPath.c_str(), pszMode);
            CC_BREAK_IF(!fp);
            
            unsigned long size;
            fseek(fp,0,SEEK_END);
            size = ftell(fp);
            fseek(fp,0,SEEK_SET);
            pData = new unsigned char[size];
            size = fread(pData,sizeof(unsigned char), size,fp);
            fclose(fp);
            
            if (pSize)
            {
                *pSize = size;
            }
        } while (0);
    }
    
    if (! pData)
    {
        std::string msg = "Get data from file(";
        msg.append(pszFileName).append(") failed!");
        CCLOG("%s", msg.c_str());
    }
    
    return pData;
}

void CCFileUtilsAndroid::uploadFiles(const char *url, const char *jsonData, JS_Function listener) {
    ce::CCImageUtils::uploadImages(url, jsonData, listener);
}

string CCFileUtilsAndroid::getWritablePath()
{
    // Fix for Nexus 10 (Android 4.2 multi-user environment)
    // the path is retrieved through Java Context.getCacheDir() method
    string dir("");
    string tmp = getFileDirectoryJNI();

    if (tmp.length() > 0)
    {
        dir.append(tmp).append("/");

        return dir;
    }
    else
    {
        return "";
    }
}

NS_CE_END
