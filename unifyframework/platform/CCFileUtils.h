#ifndef __CC_FILEUTILS_H__
#define __CC_FILEUTILS_H__

#include <string>
#include <vector>
#include <map>
#include "common.h"
#include "duktapeExtra.h"

NS_CE_BEGIN

//! @brief  Helper class to handle file operations
class CCFileUtils
{
public:

    /**
     *  Gets the instance of CCFileUtils.
     *  @js getInstance
     */
    static CCFileUtils* getInstance();
    
    /**
     *  Destroys the instance of CCFileUtils.
     */
    static void purgeFileUtils();
    
    /**
     *  The destructor of CCFileUtils.
     *  @js NA
     *  @lua NA
     */
    virtual ~CCFileUtils();
    
    /**
     *  Purges the file searching cache.
     */
    virtual void purgeCachedEntries();
    
    /**
     *  Gets resource file data
     *
     *  @param[in]  pszFileName The resource file name which contains the path.
     *  @param[in]  pszMode The read mode of the file.
     *  @param[out] pSize If the file read operation succeeds, it will be the data size, otherwise 0.
     *  @return Upon success, a pointer to the data is returned, otherwise NULL.
     *  @warning Recall: you are responsible for calling delete[] on any Non-NULL pointer returned.
     *  @js NA
     */
    virtual unsigned char* getFileData(const char* pszFileName, const char* pszMode, unsigned long * pSize);

    /**
     *  Gets resource file data from a zip file.
     *
     *  @param[in]  pszFileName The resource file name which contains the relative path of the zip file.
     *  @param[out] pSize If the file read operation succeeds, it will be the data size, otherwise 0.
     *  @return Upon success, a pointer to the data is returned, otherwise NULL.
     *  @warning Recall: you are responsible for calling delete[] on any Non-NULL pointer returned.
     *  @js NA
     */
    virtual unsigned char* getFileDataFromZip(const char* pszZipFilePath, const char* pszFileName, unsigned long * pSize);
    
    
    virtual bool writeToFile(const unsigned char* data, size_t dataLen, const char* fullPath);

    
    /**
     Returns the fullpath for a given filename.
     */
    virtual std::string fullPathForFilename(const char* pszFileName);
    
    /**
     * Loads the filenameLookup dictionary from the contents of a filename.
     * 
     * @note The plist file name should follow the format below:
     * 
     * @code
     * <?xml version="1.0" encoding="UTF-8"?>
     * <!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
     * <plist version="1.0">
     * <dict>
     *     <key>filenames</key>
     *     <dict>
     *         <key>sounds/click.wav</key>
     *         <string>sounds/click.caf</string>
     *         <key>sounds/endgame.wav</key>
     *         <string>sounds/endgame.caf</string>
     *         <key>sounds/gem-0.wav</key>
     *         <string>sounds/gem-0.caf</string>
     *     </dict>
     *     <key>metadata</key>
     *     <dict>
     *         <key>version</key>
     *         <integer>1</integer>
     *     </dict>
     * </dict>
     * </plist>
     * @endcode
     * @param filename The plist file name.
     *
     * @since v2.1
     * @loadFilenameLookup
     */
//    virtual void loadFilenameLookupDictionaryFromFile(const char* filename);
    
    /** 
     *  Sets the filenameLookup dictionary.
     *
     *  @param pFilenameLookupDict The dictionary for replacing filename.
     *  @since v2.1
     *  @lua NA
     */
//    virtual void setFilenameLookupDictionary(CCDictionary* pFilenameLookupDict);
    
    /**
     *  Gets full path from a file name and the path of the reletive file.
     *  @param pszFilename The file name.
     *  @param pszRelativeFile The path of the relative file.
     *  @return The full path.
     *          e.g. pszFilename: hello.png, pszRelativeFile: /User/path1/path2/hello.plist
     *               Return: /User/path1/path2/hello.pvr (If there a a key(hello.png)-value(hello.pvr) in FilenameLookup dictionary. )
     *
     */
    virtual const char* fullPathFromRelativeFile(const char *pszFilename, const char *pszRelativeFile);

    /** 
     *  Sets the array that contains the search order of the resources.
     *
     *  @param searchResolutionsOrder The source array that contains the search order of the resources.
     *  @see getSearchResolutionsOrder(void), fullPathForFilename(const char*).
     *  @since v2.1
     *  @js NA
     *  @lua NA
     */
//    virtual void setSearchResolutionsOrder(const std::vector<std::string>& searchResolutionsOrder);

    /**
      * Append search order of the resources.
      *
      * @see setSearchResolutionsOrder(), fullPathForFilename().
      * @since v2.1
      */
//    virtual void addSearchResolutionsOrder(const char* order);
    
    /**
     *  Gets the array that contains the search order of the resources.
     *
     *  @see setSearchResolutionsOrder(const std::vector<std::string>&), fullPathForFilename(const char*).
     *  @since v2.1
     *  @js NA
     *  @lua NA
     */
//    virtual const std::vector<std::string>& getSearchResolutionsOrder();
    
    /** 
     *  Sets the array of search paths.
     * 
     *  You can use this array to modify the search path of the resources.
     *  If you want to use "themes" or search resources in the "cache", you can do it easily by adding new entries in this array.
     *
     *  @note This method could access relative path and absolute path.
     *        If the relative path was passed to the vector, CCFileUtils will add the default resource directory before the relative path.
     *        For instance:
     *        	On Android, the default resource root path is "assets/".
     *        	If "/mnt/sdcard/" and "resources-large" were set to the search paths vector,
     *        	"resources-large" will be converted to "assets/resources-large" since it was a relative path.
     *
     *  @param searchPaths The array contains search paths.
     *  @see fullPathForFilename(const char*)
     *  @since v2.1
     *  @js NA
     *  @lua NA
     */
    virtual void setSearchPaths(const std::vector<std::string>& searchPaths);
    
    /**
     * Set search root path.
      */
    void setSearchRootPath(const char* path);

    /**
      * Add search path.
      *
      * @since v2.1
      */
     void addSearchPath(const char* path);

    /**
      * Removes all paths.
      *
      * @since v2.2
      * @lua NA
      */
	 void removeAllPaths();
    
    /**
     *  Gets the array of search paths.
     *  
     *  @return The array of search paths.
     *  @see fullPathForFilename(const char*).
     */
    virtual const std::vector<std::string>& getSearchPaths();


    /**
     * Set writable/cache path
     */
    virtual std::string getWritablePath();
    virtual std::string getCachePath();

    /**
     *  Set writable/cache path
     */
    virtual void setWritablePath(const char *writablePath);
    virtual void setCachePath(const char *cachePath);
    
    /**
     *  Checks whether a file exists.
     *
     *  @note If a relative path was passed in, it will be inserted a default root path at the beginning.
     *  @param strFilePath The path of the file, it could be a relative or absolute path.
     *  @return true if the file exists, otherwise it will return false.
     */
    virtual bool isFileExist(const std::string& strFilePath) = 0;
    
    /**
     *  Checks whether a directory exists.
     *
     *  @param strDirPath The path of the directory, it could be an absolute path only.
     *  @return true if the directory exists, otherwise it will return false.
     */
//    virtual bool isDirectoryExist(const std::string& strDirPath) = 0;
    
    /**
     *  Checks whether the path is an absolute path.
     *
     *  @note On Android, if the parameter passed in is relative to "assets/", this method will treat it as an absolute path.
     *        Also on Blackberry, path starts with "app/native/Resources/" is treated as an absolute path.
     *
     *  @param strPath The path that needs to be checked.
     *  @return true if it's an absolute path, otherwise it will return false.
     */
    virtual bool isAbsolutePath(const std::string& strPath);

    /**
     *
     * upload files
     *
     */
    virtual void uploadFiles(const char *url, const char *jsonData, JS_Function listener);
    
protected:
    /**
     *  The default constructor.
     */
    CCFileUtils();
    
    virtual bool init();
    
    /**
     *  Gets the new filename from the filename lookup dictionary.
     *  @param pszFileName The original filename.
     *  @return The new filename after searching in the filename lookup dictionary.
     *          If the original filename wasn't in the dictionary, it will return the original filename.
     */
    virtual std::string getNewFilename(const char* pszFileName);
    
    /**
     *  Gets full path for filename, resolution directory and search path.
     *
     *  @param filename The file name.
     *  @param resolutionDirectory The resolution directory.
     *  @param searchPath The search path.
     *  @return The full path of the file. It will return an empty string if the full path of the file doesn't exist.
     */
    virtual std::string getPathForFilename(const std::string& filename, const std::string& resolutionDirectory, const std::string& searchPath);
    
    /**
     *  Gets full path for the directory and the filename.
     *
     *  @note Only iOS and Mac need to override this method since they are using
     *        `[[NSBundle mainBundle] pathForResource: ofType: inDirectory:]` to make a full path.
     *        Other platforms will use the default implementation of this method.
     *  @param strDirectory The directory contains the file we are looking for.
     *  @param strFilename  The name of the file.
     *  @return The full path of the file, if the file can't be found, it will return an empty string.
     */
    virtual std::string getFullPathForDirectoryAndFilename(const std::string& strDirectory, const std::string& strFilename);
    
    /**
     *  Creates a dictionary by the contents of a file.
     *  @note This method is used internally.
     */
//    virtual CCDictionary* createCCDictionaryWithContentsOfFile(const std::string& filename);
    
    /**
     *  Write a dictionary to a plist file.
     *  @note This method is used internally.
     */
//    virtual bool writeToFile(CCDictionary *dict, const std::string& fullPath);
    
    /**
     *  Creates an array by the contents of a file.
     *  @note This method is used internally.
     */
//    virtual CCArray* createCCArrayWithContentsOfFile(const std::string& filename);
    
    void updateSearchPathArrayCheck(void);
    
    /** Dictionary used to lookup filenames based on a key.
     *  It is used internally by the following methods:
     *
     *  std::string fullPathForFilename(const char*);
     *
     *  @since v2.1
     */
//    CCDictionary* m_pFilenameLookupDict;
    
    /** 
     *  The vector contains resolution folders.
     *  The lower index of the element in this vector, the higher priority for this resolution directory.
     */
    std::vector<std::string> m_searchResolutionsOrderArray;
    
    /**
     * The vector contains search paths.
     * The lower index of the element in this vector, the higher priority for this search path.
     */
    std::vector<std::string> m_searchPathArray;
    std::vector<std::string> m_searchPathArrayCheck;
    
    /**
     *  The default root path of resources.
     *  If the default root path of resources needs to be changed, do it in the `init` method of CCFileUtils's subclass.
     *  For instance:
     *  On Android, the default root path of resources will be assigned with "assets/" in CCFileUtilsAndroid::init().
     *  Similarly on Blackberry, we assign "app/native/Resources/" to this variable in CCFileUtilsBlackberry::init().
     */
    std::string m_strDefaultResRootPath;
    
    /**
     *  The full path cache. When a file is found, it will be added into this cache. 
     *  This variable is used for improving the performance of file search.
     */
    std::map<std::string, std::string> m_fullPathCache;
    
    /**
     * Writable path (for debug)
     */
    std::string m_strWritablePath;

    /**
     * Cache path (for debug)
     */
    std::string m_strCachePath;
    
    /**
     *  The singleton pointer of CCFileUtils.
     */
    static CCFileUtils* _instance;
};

NS_CE_END

#endif    // __CC_FILEUTILS_H__
