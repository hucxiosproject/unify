//
//  CCJsDebuggerUtils.h
//  unifyframework
//
//  Created by Vlad on 4/18/16.
//  Copyright © 2016 vocinno. All rights reserved.
//

#ifndef CCJsDebuggerUtils_h
#define CCJsDebuggerUtils_h


#ifdef __cplusplus
extern "C" {
#endif


void OnDebuggerStateChanged(int state, const char* fileName, const char* funcName, int line, int pc);
void OnDebuggerGetLocals(const char* namesArr, const char* valuesArr);
void OnReceiveExceptionNotification(int fatal, const char* msg, const char* fileName, int line);
    
#ifdef __cplusplus
} //  extern "C"
#endif
    
#endif /* CCJsDebuggerUtils_h */
