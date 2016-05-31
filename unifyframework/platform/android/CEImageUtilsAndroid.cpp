#include "CCImageUtils.h"
#include "CEImageUtilsAndroid.h"
#include "CEJsEngine.h"
#include "jni/CEImageUtilJni.h"

NS_CE_BEGIN

CEImageUtilsAndroid::CEImageUtilsAndroid(std::string url, std::string files, bool cached, JS_Function listener) {
	_url = url;
	_files = files;
	_jsListener = listener;
  _cached = cached;
}

CEImageUtilsAndroid::~CEImageUtilsAndroid() {

}

void CEImageUtilsAndroid::callback(int statusCode, std::string response) {


  CEJsEngine::getInstance()->callback(_jsListener, statusCode, response.c_str());
  /*
	duk_context* ctx = CEJsEngine::getInstance()->getJsState();
  duk_push_heapptr(ctx, _jsListener);
  duk_push_int(ctx, statusCode);
  duk_push_string(ctx, response.c_str());
  if (statusCode != 200) {
    duk_push_string(ctx, response.c_str());
  } else {
    duk_push_string(ctx, "fuck android");
  }
  if (duk_pcall(ctx, 3) != DUK_EXEC_SUCCESS)
  {
      CCLOG("Call js function error: %s", duk_to_string(ctx, -1));
  }
  
  CEJsEngine::getInstance()->clean();*/
}

void CCImageUtils::downloadImage(const char *url, const bool saveCached, JS_Function listener) {
  std::string urlString = url;
  std::string jsonString = "";
  CEImageUtilsAndroid* imageUtil = new CEImageUtilsAndroid(urlString, jsonString, saveCached, listener);
  downloadImageJni(imageUtil);
}

void CCImageUtils::uploadImages(const char *url, const char *jsonData, JS_Function listener) {
	std::string urlString = url;
	std::string jsonString = jsonData;
	CEImageUtilsAndroid* imageUtil = new CEImageUtilsAndroid(urlString, jsonString, false, listener);
  uploadImagesJni(imageUtil);
}

NS_CE_END