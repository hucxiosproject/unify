#ifndef __CEHttpRequestJni_H__
#define __CEHttpRequestJni_H__
#include <jni.h>
#include <string>
#include "platform/network/HttpRequest.h"

extern void httpGetJni(ce::HttpRequest* request);
extern void httpPostJni(ce::HttpRequest* request);
extern void httpDeleteJni(ce::HttpRequest* request);
extern void httpPutJni(ce::HttpRequest* request);


#endif /* __CEHttpRequestJni_H__ */
