#include "CCFileUtils.h"
#include "ccMacrosUnify.h"
#include "unzip.h"


NS_CE_BEGIN

CCFileUtils* CCFileUtils::_instance = NULL;

void CCFileUtils::purgeFileUtils()
{
    if (_instance)
    {
        delete _instance;
        _instance = NULL;
    }
}

CCFileUtils::CCFileUtils()
//: m_pFilenameLookupDict(NULL)
{
}

CCFileUtils::~CCFileUtils()
{
//    CC_SAFE_RELEASE(m_pFilenameLookupDict);
}

bool CCFileUtils::init()
{
    m_searchPathArray.push_back(m_strDefaultResRootPath);
    m_searchResolutionsOrderArray.push_back("");
    updateSearchPathArrayCheck();
    return true;
}

void CCFileUtils::purgeCachedEntries()
{
    m_fullPathCache.clear();
}

unsigned char* CCFileUtils::getFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize)
{
    unsigned char * pBuffer = NULL;
    CCAssert(pszFileName != NULL && pSize != NULL && pszMode != NULL, "Invalid parameters.");
    *pSize = 0;
    do
    {
        // read the file from hardware
        std::string fullPath = fullPathForFilename(pszFileName);
        FILE *fp = fopen(fullPath.c_str(), pszMode);
        CC_BREAK_IF(!fp);
        
        fseek(fp,0,SEEK_END);
        *pSize = ftell(fp);
        fseek(fp,0,SEEK_SET);
        pBuffer = new unsigned char[*pSize];
        *pSize = fread(pBuffer,sizeof(unsigned char), *pSize,fp);
        fclose(fp);
    } while (0);
    
    if (! pBuffer)
    {
        std::string msg = "Get data from file(";
        msg.append(pszFileName).append(") failed!");
        
        printf("%s\n", msg.c_str());
    }
    return pBuffer;
}

unsigned char* CCFileUtils::getFileDataFromZip(const char* pszZipFilePath, const char* pszFileName, unsigned long * pSize)
{
    unsigned char * pBuffer = NULL;
    unzFile pFile = NULL;
    *pSize = 0;

    do 
    {
        CC_BREAK_IF(!pszZipFilePath || !pszFileName);
        CC_BREAK_IF(strlen(pszZipFilePath) == 0);

        pFile = unzOpen(pszZipFilePath);
        CC_BREAK_IF(!pFile);

        int nRet = unzLocateFile(pFile, pszFileName, 1);
        CC_BREAK_IF(UNZ_OK != nRet);

        char szFilePathA[260];
        unz_file_info FileInfo;
        nRet = unzGetCurrentFileInfo(pFile, &FileInfo, szFilePathA, sizeof(szFilePathA), NULL, 0, NULL, 0);
        CC_BREAK_IF(UNZ_OK != nRet);

        nRet = unzOpenCurrentFile(pFile);
        CC_BREAK_IF(UNZ_OK != nRet);

        pBuffer = new unsigned char[FileInfo.uncompressed_size];
        int CC_UNUSED nSize = unzReadCurrentFile(pFile, pBuffer, FileInfo.uncompressed_size);
        CCAssert(nSize == 0 || nSize == (int)FileInfo.uncompressed_size, "the file size is wrong");

        *pSize = FileInfo.uncompressed_size;
        unzCloseCurrentFile(pFile);
    } while (0);

    if (pFile)
    {
        unzClose(pFile);
    }

    return pBuffer;
}


bool CCFileUtils::writeToFile(const unsigned char *data, size_t dataLen, const char *fullPath)
{
    FILE* fp = 0;
    
    fp = fopen(fullPath, "w");
    if (!fp)
    {
        return false;
    }
    
    fwrite(data, dataLen, 1, fp);
    fclose(fp);
    return true;
}


std::string CCFileUtils::getNewFilename(const char* pszFileName)
{
//    const char* pszNewFileName = NULL;
//    // in Lookup Filename dictionary ?
//    CCString* fileNameFound = m_pFilenameLookupDict ? (CCString*)m_pFilenameLookupDict->objectForKey(pszFileName) : NULL;
//    if( NULL == fileNameFound || fileNameFound->length() == 0) {
//        pszNewFileName = pszFileName;
//    }
//    else {
//        pszNewFileName = fileNameFound->getCString();
//        //CCLOG("FOUND NEW FILE NAME: %s.", pszNewFileName);
//    }
//    return pszNewFileName;
    
    return pszFileName;
}

std::string CCFileUtils::getPathForFilename(const std::string& filename, const std::string& resolutionDirectory, const std::string& searchPath)
{
    std::string file = filename;
    std::string file_path = "";
    size_t pos = filename.find_last_of("/");
    if (pos != std::string::npos)
    {
        file_path = filename.substr(0, pos+1);
        file = filename.substr(pos+1);
    }
    
    // searchPath + file_path + resourceDirectory
    std::string path = searchPath;
    path += file_path;
    path += resolutionDirectory;
    
    path = getFullPathForDirectoryAndFilename(path, file);
    
    // CCLOG("getPathForFilename, fullPath = %s ", path.c_str());
    return path;
}


std::string CCFileUtils::fullPathForFilename(const char* pszFileName)
{
    CCAssert(pszFileName != NULL, "CCFileUtils: Invalid path");
    
    std::string strFileName = pszFileName;
    if (isAbsolutePath(pszFileName))
    {
        //CCLOG("Return absolute path( %s ) directly.", pszFileName);
        return pszFileName;
    }
    
    // Already Cached ?
    std::map<std::string, std::string>::iterator cacheIter = m_fullPathCache.find(pszFileName);
    if (cacheIter != m_fullPathCache.end())
    {
        //CCLOG("Return full path from cache: %s", cacheIter->second.c_str());
        return cacheIter->second;
    }
    
    // Get the new file name.
//    std::string newFilename = getNewFilename(pszFileName);
    std::string newFilename = pszFileName;
    
    std::string fullpath = "";
    
    for (std::vector<std::string>::iterator searchPathsIter = m_searchPathArrayCheck.begin();
         searchPathsIter != m_searchPathArrayCheck.end(); ++searchPathsIter) {
        for (std::vector<std::string>::iterator resOrderIter = m_searchResolutionsOrderArray.begin();
             resOrderIter != m_searchResolutionsOrderArray.end(); ++resOrderIter) {
            
            // CCLOG("\n\nSEARCHING: %s, %s, %s\n", newFilename.c_str(), resOrderIter->c_str(), searchPathsIter->c_str());
            
            fullpath = this->getPathForFilename(newFilename, *resOrderIter, *searchPathsIter);
            
            if (fullpath.length() > 0)
            {
                // Using the filename passed in as key.
                m_fullPathCache.insert(std::pair<std::string, std::string>(pszFileName, fullpath));
                //CCLOG("Returning path: %s", fullpath.c_str());
                return fullpath;
            }
        }
    }
    
    //CCLOG("fullPathForFilename: No file found at %s. Possible missing file.", pszFileName);
    
    // The file wasn't found, return the file name passed in.
    return pszFileName;
}

    
const char* CCFileUtils::fullPathFromRelativeFile(const char *pszFilename, const char *pszRelativeFile)
{
//    std::string relativeFile = pszRelativeFile;
//    CCString *pRet = CCString::create("");
//    pRet->m_sString = relativeFile.substr(0, relativeFile.rfind('/')+1);
//    pRet->m_sString += getNewFilename(pszFilename);
//    return pRet->getCString();
    return "";
}

//void CCFileUtils::setSearchResolutionsOrder(const std::vector<std::string>& searchResolutionsOrder)
//{
//    bool bExistDefault = false;
//    m_fullPathCache.clear();
//    m_searchResolutionsOrderArray.clear();
//    for (std::vector<std::string>::const_iterator iter = searchResolutionsOrder.begin(); iter != searchResolutionsOrder.end(); ++iter)
//    {
//        std::string resolutionDirectory = *iter;
//        if (!bExistDefault && resolutionDirectory == "")
//        {
//            bExistDefault = true;
//        }
//        
//        if (resolutionDirectory.length() > 0 && resolutionDirectory[resolutionDirectory.length()-1] != '/')
//        {
//            resolutionDirectory.append("/");
//        }
//        
//        m_searchResolutionsOrderArray.push_back(resolutionDirectory);
//    }
//    if (!bExistDefault)
//    {
//        m_searchResolutionsOrderArray.push_back("");
//    }
//}
//
//void CCFileUtils::addSearchResolutionsOrder(const char* order)
//{
//    m_searchResolutionsOrderArray.push_back(order);
//}
//
//const std::vector<std::string>& CCFileUtils::getSearchResolutionsOrder()
//{
//    return m_searchResolutionsOrderArray;
//}

const std::vector<std::string>& CCFileUtils::getSearchPaths()
{
    return m_searchPathArray;
}

void CCFileUtils::setSearchPaths(const std::vector<std::string>& searchPaths)
{
    m_fullPathCache.clear();
    m_searchPathArray.clear();
    m_searchPathArray.assign(searchPaths.begin(), searchPaths.end());
    updateSearchPathArrayCheck();
}

//void CCFileUtils::setSearchRootPath(const char* path)
//    {
//    m_fullPathCache.clear();
//    m_strDefaultResRootPath = path ? path : "";
//    updateSearchPathArrayCheck();
//    }
//
void CCFileUtils::addSearchPath(const char* path)
{
    if (path && strlen(path) > 0)
    {
        m_searchPathArray.push_back(path);
        updateSearchPathArrayCheck();
    }
}

//void CCFileUtils::setFilenameLookupDictionary(CCDictionary* pFilenameLookupDict)
//{
//    m_fullPathCache.clear();
//    CC_SAFE_RELEASE(m_pFilenameLookupDict);
//    m_pFilenameLookupDict = pFilenameLookupDict;
//    CC_SAFE_RETAIN(m_pFilenameLookupDict);
//}
//
//void CCFileUtils::loadFilenameLookupDictionaryFromFile(const char* filename)
//{
//    std::string fullPath = this->fullPathForFilename(filename);
//    if (fullPath.length() > 0)
//    {
//        CCDictionary* pDict = CCDictionary::createWithContentsOfFile(fullPath.c_str());
//        if (pDict)
//        {
//            CCDictionary* pMetadata = (CCDictionary*)pDict->objectForKey("metadata");
//            int version = ((CCString*)pMetadata->objectForKey("version"))->intValue();
//            if (version != 1)
//            {
//                CCLOG("ERROR: Invalid filenameLookup dictionary version: %ld. Filename: %s", (long)version, filename);
//                return;
//            }
//            setFilenameLookupDictionary((CCDictionary*)pDict->objectForKey("filenames"));
//        }
//    }
//}

std::string CCFileUtils::getFullPathForDirectoryAndFilename(const std::string& strDirectory, const std::string& strFilename)
{
    std::string ret = strDirectory+strFilename;
    if (!isFileExist(ret)) {
        ret = "";
    }
    return ret;
}

    

std::string CCFileUtils::getWritablePath()
{
    return m_strWritablePath;
}
    
std::string CCFileUtils::getCachePath()
{
    return m_strCachePath.length() > 0 ? m_strCachePath : getWritablePath();
}

void CCFileUtils::setWritablePath(const char *writablePath)
{
    m_strWritablePath = writablePath ? writablePath : "";
}

void CCFileUtils::setCachePath(const char *cachePath)
{
    m_strCachePath = cachePath ? cachePath : "";
}

bool CCFileUtils::isAbsolutePath(const std::string& strPath)
{
    return strPath[0] == '/' ? true : false;
}


void CCFileUtils::updateSearchPathArrayCheck(void)
{
    size_t len = m_strDefaultResRootPath.length();
    if (len > 0 && m_strDefaultResRootPath[len - 1] != '/' && m_strDefaultResRootPath[len - 1] != '\\')
    {
#if CC_TARGET_PLATFORM == CC_PLATFORM_WIN32
        m_strDefaultResRootPath.append("\\");
#else
        m_strDefaultResRootPath.append("/");
#endif
    }

    m_searchPathArrayCheck.clear();
    for (std::vector<std::string>::iterator it = m_searchPathArray.begin(); it != m_searchPathArray.end(); ++it)
    {
        std::string path = *it;
        if (!isAbsolutePath(path))
        {
            path = m_strDefaultResRootPath + path;
        }
        m_searchPathArrayCheck.push_back(path);
    }
    if (m_strDefaultResRootPath.length())
    {
        m_searchPathArrayCheck.push_back(m_strDefaultResRootPath);
    }
}

void CCFileUtils::uploadFiles(const char *url, const char *jsonData, JS_Function listener) {
    // do nothing
    CCLOG("convert CCFileUtils to nsdict error");
}

NS_CE_END

