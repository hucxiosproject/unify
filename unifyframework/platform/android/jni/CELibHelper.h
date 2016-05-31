#ifndef __CELibHelper_H__
#define __CELibHelper_H__
#include <jni.h>
#include <string>

extern const char * getApkPath();
extern void terminateProcessJNI();
extern std::string getCurrentLanguageJNI();
extern std::string getPackageNameJNI();
extern std::string getFileDirectoryJNI();
extern void enableAccelerometerJNI();
extern void disableAccelerometerJNI();
extern void setAccelerometerIntervalJNI(float interval);
extern bool inDirectoryExistsJNI(const char* path);


extern void notifyToJni(const char* noticeName, const char* className, const char* data);


// functions for CCUserDefault
namespace unify {
    bool getBoolForKeyJNI(const char *pKey, bool defaultValue);

    int getIntegerForKeyJNI(const char *pKey, int defaultValue);

    float getFloatForKeyJNI(const char *pKey, float defaultValue);

    double getDoubleForKeyJNI(const char *pKey, double defaultValue);

    std::string getStringForKeyJNI(const char *pKey, const char *defaultValue);

    void setBoolForKeyJNI(const char *pKey, bool value);

    void setIntegerForKeyJNI(const char *pKey, int value);

    void setFloatForKeyJNI(const char *pKey, float value);

    void setDoubleForKeyJNI(const char *pKey, double value);

    void setStringForKeyJNI(const char *pKey, const char *value);

}


#endif /* __CELibHelper_H__ */
