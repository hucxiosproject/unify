#ifndef __JsHelper_H__
#define __JsHelper_H__

#include <string>


#ifdef DUK_OPT_DEBUGGER_SUPPORT
void OnDebuggerStateChanged(int state, const char* fileName, const char* funcName, int line, int pc);
void OnDebuggerGetLocals(const char* namesArr, const char* valuesArr);
void OnReceiveExceptionNotification(int fatal, const char* msg, const char* fileName, int line);
#endif
#endif /* __JsHelper_H__ */
