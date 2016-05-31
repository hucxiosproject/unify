#ifndef __CEImageUtilsAndroid_H__
#define __CEImageUtilsAndroid_H__

#include "platform/CCImageUtils.h"

#include <string>
#include <vector>

NS_CE_BEGIN

class CC_DLL CEImageUtilsAndroid : public CCImageUtils 
{
	friend class CCImageUtils;
private:
	std::string _url;
	std::string _files;
	bool _cached;
	JS_Function _jsListener;

public:
	CEImageUtilsAndroid(std::string url, std::string files, bool cached, JS_Function listener);
	virtual ~CEImageUtilsAndroid();

	void callback(int statusCode, std::string response);
	std::string getUrl() { return _url; }
	std::string getFiles() { return _files; }
	JS_Function getListener()  { return _jsListener; }
	bool getCached() { return _cached; }
};

NS_CE_END

#endif