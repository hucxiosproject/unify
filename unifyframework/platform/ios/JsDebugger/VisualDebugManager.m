//
//  VisualDebugManager.m
//  unifyframework
//
//  Created by Vlad on 5/25/16.
//  Copyright © 2016 vocinno. All rights reserved.
//

#import "VisualDebugManager.h"
#import "DebuggerViewController.h"


@implementation VisualDebugManager
{
    UIPageViewController* pageViewController;
    UIStoryboard *storyboard;
}



#pragma mark Singleton Methods

+ (id)sharedManager {
    static VisualDebugManager *sharedVisualDebugManager = nil;
    static dispatch_once_t onceToken;
    dispatch_once(&onceToken, ^{
        sharedVisualDebugManager = [self new];
    });
    return sharedVisualDebugManager;
}

- (id)init {
    if (self = [super init]) {
        
    }
    return self;
}


- (void) registerSuperNavController: (id<SuperNavControllerProtocol>)newSuperNavControllerHolder
{
    superNavControllerHolder = newSuperNavControllerHolder;
}

- (void) showDebugWindow
{
    if (superNavControllerHolder == nil)
    {
        return;
    }
    
    NSBundle* bundle = [NSBundle bundleWithIdentifier:@"com.vocinno.framework.unifyframework"];
    
    storyboard = [UIStoryboard storyboardWithName:@"debuggerStoryboard" bundle: bundle];
    
    DebuggerViewController* debuggerViewController = (DebuggerViewController*)[storyboard instantiateViewControllerWithIdentifier:@"debuggerViewController"];
    
    [debuggerViewController view]; //Force load view hierarchy
    
    pageViewController = (UIPageViewController*)[storyboard instantiateViewControllerWithIdentifier:@"pageViewController"];
    
    [debuggerViewController addChildViewController:pageViewController];
    
    pageViewController.view.frame = CGRectMake(0, 0, debuggerViewController.containerView.frame.size.width, debuggerViewController.containerView.frame.size.height);
    [debuggerViewController.containerView addSubview:pageViewController.view];
    
    [pageViewController didMoveToParentViewController:debuggerViewController];
    
    [pageViewController setDataSource:self];
    
    UIViewController* sourceViewController = [storyboard instantiateViewControllerWithIdentifier:@"sourceViewController"];
    
    NSArray *startingViewControllers = @[sourceViewController];
    
    [pageViewController setViewControllers: startingViewControllers
                             direction: UIPageViewControllerNavigationDirectionForward
                              animated: NO
                            completion: nil];
    
    UINavigationController* navController = [superNavControllerHolder superNavController];
    
    //[navController pushViewController:pageViewController animated:YES];
    [navController pushViewController:debuggerViewController animated:YES];
    
}


#pragma mark UIPageViewControllerDataSource

- (UIViewController *) pageViewController: (UIPageViewController *) pageViewController viewControllerBeforeViewController:(UIViewController *) viewController
{
    
    if (viewController.view.tag == 0)
    {
        return nil;
    }
    if (viewController.view.tag == 1)
    {
        return [storyboard instantiateViewControllerWithIdentifier:@"varViewController"];
    }
    return nil;
    /*
    if (viewController == page2)
    {
        return nil;
    }
    return page2;*/
}

- (UIViewController *) pageViewController: (UIPageViewController *) pageViewController viewControllerAfterViewController:(UIViewController *) viewController
{
    
    if (viewController.view.tag == 0)
    {
        return [storyboard instantiateViewControllerWithIdentifier:@"sourceViewController"];
    }
    /*
    if (viewController == page2)
    {
        return nil;
    }*/
    return nil;
    
    /*
    if (viewController == page2)
    {
        return nil;
    }
    return page2;*/
}

#pragma mark -
#pragma mark Page Indicator

- (NSInteger) presentationCountForPageViewController: (UIPageViewController *) pageViewController
{
    return 2;
}

- (NSInteger) presentationIndexForPageViewController: (UIPageViewController *) pageViewController
{
    return 1;
}

@end
