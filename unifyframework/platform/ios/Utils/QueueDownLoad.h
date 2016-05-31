//
//  QueueDownLoad.h
//  laohaowan
//
//  Created by Vocinno Mac Mini 1 on 16/3/9.
//
//

#import <Foundation/Foundation.h>

@interface QueueDownLoad : NSObject
+(void)queueDownLoadAllFiles:(NSMutableArray*)address path:(NSString*)path endBlock:(void(^)(bool success))block;
+(id)getOperationWithUrl:(NSString *)stringUrl fileId:(NSString*)fileId path:(NSString*)path  endBlock:(void(^)(bool success))block;
@end
