//
//  QueueDownLoad.m
//  laohaowan
//
//  Created by Vocinno Mac Mini 1 on 16/3/9.
//
//

#import "QueueDownLoad.h"

#import "AFHTTPRequestOperationManager.h"

@implementation QueueDownLoad
static int indexQueue = 0;
static int QueueCount = 0;
+(void)queueDownLoadAllFiles:(NSMutableArray*)address path:(NSString*)path endBlock:(void(^)(bool success))block{
    QueueCount = (int)address.count;
    
    NSMutableArray *operationArray = [NSMutableArray array];
    for (int i =0; i < address.count; i++) {
        
        NSArray *arr = [address objectAtIndex:i];
        if ([arr count] == 3){
            NSString *fileFullPath = [arr objectAtIndex:0];
            NSString *fileId =       [arr objectAtIndex:1];

            AFHTTPRequestOperation *operation = [self getOperationWithUrl:fileFullPath fileId:fileId path:path  endBlock:block];
            //add operation
            [QueueDownLoad addDependency:operation operationArray:operationArray];
        }else{
            continue;
        }
       
    }
    
    
    NSOperationQueue *operationQueue = [[NSOperationQueue alloc] init];
    [operationQueue addOperations:operationArray waitUntilFinished:NO];
}

+(id)getOperationWithUrl:(NSString *)stringUrl fileId:(NSString*)fileId path:(NSString*)path  endBlock:(void(^)(bool success))block{
    NSURL *url = [NSURL URLWithString:stringUrl];
    NSURLRequest *request = [NSURLRequest requestWithURL:url];
    AFHTTPRequestOperation *afOperation = [[AFHTTPRequestOperation alloc] initWithRequest:request];
    
    afOperation.userInfo =  [NSDictionary dictionaryWithObjectsAndKeys:stringUrl,@"imageid",fileId,@"fileId", nil];
    [afOperation setCompletionBlockWithSuccess:^(AFHTTPRequestOperation *operation, id responseObject) {
        // NSLog(@"all keys =%@",operation.userInfo.allKeys);
       // UIImage *image = [UIImage imageWithData:operation.responseData];// operation.responseData
        
        NSString *newFileId = [operation.userInfo objectForKey:@"fileId"];
         NSString *finalPath = [path stringByAppendingPathComponent:newFileId];
        [operation.responseData writeToFile:finalPath atomically:NO];
        
        indexQueue++;
        //write to localdata
        if (indexQueue == QueueCount && block) {
            block(true);
        }
        
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        NSLog(@"Error: %@", operation.userInfo);
        
        if (block) {
            block(false);
        }
    }];
    return afOperation;
}
+(void)addDependency:(AFHTTPRequestOperation*)operation operationArray:(NSMutableArray *)operationArray{
    [operationArray addObject:operation];
    if ([operationArray count] != 1) {
        [[operationArray lastObject] addDependency:[operationArray objectAtIndex:operationArray.count-2]];
    }
}
@end
