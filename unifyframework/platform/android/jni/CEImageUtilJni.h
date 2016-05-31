#ifndef __CEImageUtilJni_H__
#define __CEImageUtilJni_H__
#include <jni.h>
#include <string>
#include "CEImageUtilsAndroid.h"

extern void uploadImagesJni(ce::CEImageUtilsAndroid* imageUploader);
extern void downloadImageJni(ce::CEImageUtilsAndroid* imageDownloader);

#endif