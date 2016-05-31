//
//  NotificationCenterIos.h
//  unifyframework
//
//  Created by Vlad on 4/11/16.
//  Copyright © 2016 vocinno. All rights reserved.
//

#ifndef NotificationCenterIos_h
#define NotificationCenterIos_h

#include <objc/message.h>

#include <string>

typedef void (*notifyFunc)(id, SEL, const char*, const char*);


void dispatchInMainThread(notifyFunc f, id object, SEL selector, const char* name, const char* body);

#endif /* NotificationCenterIos_h */
