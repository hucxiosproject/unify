//
//  VisualDebugManager.h
//  unifyframework
//
//  Created by Vlad on 5/25/16.
//  Copyright © 2016 vocinno. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "SuperNavControllerProtocol.h"

@interface VisualDebugManager : NSObject<UIPageViewControllerDataSource> {
    id<SuperNavControllerProtocol> superNavControllerHolder;
}


+ (id) sharedManager;

- (void) registerSuperNavController: (id<SuperNavControllerProtocol>)superNavController;

- (void) showDebugWindow;


@end
