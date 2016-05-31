#import <Foundation/Foundation.h>
#import <UIKit/UIDevice.h>
#include <string>
#include "unzip.h"
#include "CCFileUtilsIOS.h"
#include "CEFuncUtilsIOS.h"
#include "CEJsEngine.h"
#import "AFHTTPRequestOperationManager.h"


/////////////////////////////////////
//
// Image uploader
//
/////////////////////////////////////
@interface FileUploader : NSObject

@property(assign)JS_Function jsCallback;
@property(strong)NSString* url;
@property(strong)NSMutableArray* files;

-(void)uploadFiles:(NSString*)url files:(NSMutableArray*)files params:(NSDictionary*)params callback:(JS_Function)callback;

@end

@implementation FileUploader

-(void)uploadFiles:(NSString*)url files:(NSMutableArray*)files params:(NSDictionary*)params callback:(JS_Function)callback {
    self.jsCallback = callback;
    self.url = url;
    self.files = files;
    
    AFHTTPRequestOperationManager *manager = [AFHTTPRequestOperationManager manager];
    NSMutableDictionary *dicForRequest = [[NSMutableDictionary alloc] init];
    NSEnumerator *enumerator = [params keyEnumerator];
    NSString *key;
    while (key = [enumerator nextObject]) {
        NSString *value = [params objectForKey:key];
        [dicForRequest setValue:value forKey:key];
    }
    NSMutableArray *fileArray = [[NSMutableArray alloc]init];
    [manager POST:url parameters:dicForRequest constructingBodyWithBlock:^(id<AFMultipartFormData> formData) {
        for (NSDictionary *file in files) {
            NSString *filePath = [file objectForKey:@"path"];
            bool localFileExist = ce::CCFileUtilsIOS::getInstance()->isFileExist([filePath UTF8String]);
            if (!localFileExist) {
                NSLog(@"file not found when upload, file path is %@", filePath);
                return;
            }
//            UIImage *uiimage = [UIImage imageNamed:filePath];
//            //            NSData *data = UIImageJPEGRepresentation(uiimage, 1.0);
//            NSData *data = UIImagePNGRepresentation(uiimage);
//
            NSMutableDictionary *fileNameDict = [[NSMutableDictionary alloc]init];
            NSString *fileName = [NSString stringWithFormat:@"img-%lu", (unsigned long)[filePath hash]];
//            NSString *mimeType = [self getMimeTypeForImage:data];
//            [formData appendPartWithFileData:data name:filePath fileName:fileName mimeType:mimeType];
            [formData appendPartWithFileURL:[NSURL fileURLWithPath:filePath] name:fileName error:nil];
            NSArray *array = [filePath componentsSeparatedByString:@"/"];
            NSString *realname = array[array.count - 1];
            
            [fileNameDict setValue: filePath forKey:@"filePath"];
            [fileNameDict setValue: realname forKey:@"fileName"];
            [fileArray addObject:fileNameDict ];
        }
    } success:^(AFHTTPRequestOperation *operation, id responseObject) {
        if ([responseObject isKindOfClass:[NSDictionary class]]){
            NSMutableArray *arr = [[NSMutableArray alloc]initWithArray:[responseObject objectForKey:@"data"]];
            NSMutableDictionary *dic;
            for (int i = 0; i < arr.count; i ++) {
                dic =[[NSMutableDictionary alloc]initWithDictionary:arr[i]];
                for (int j = 0; j < fileArray.count; j++) {
                    if([[dic objectForKey:@"name"] isEqualToString:[fileArray[j] objectForKey:@"fileName"]]){
                        NSString *str = [fileArray[j] objectForKey:@"filePath"];
                        [dic setValue:str forKey:@"name"] ;
                        [arr replaceObjectAtIndex:i withObject:dic];
                    }
                }
            }
            
            NSString *response = [CEFuncUtilsIOS convertArrayToJson:arr];
            [self callback:200 response:response];
        }else if([responseObject isKindOfClass:[NSArray class]]){
            NSMutableArray *arr = [[NSMutableArray alloc] initWithArray:responseObject];
            NSMutableDictionary *dic;
            for (int i = 0; i < arr.count; i ++) {
                dic =[[NSMutableDictionary alloc]initWithDictionary:arr[i]];
                for (int j = 0; j < fileArray.count; j++) {
                    if([[dic objectForKey:@"name"] isEqualToString:[fileArray[j] objectForKey:@"fileName"]]){
                        NSString *str = [fileArray[j] objectForKey:@"filePath"];
                        [dic setValue:str forKey:@"name"] ;
                        [arr replaceObjectAtIndex:i withObject:dic];
                    }
                }
            }
            
            NSString *response = [CEFuncUtilsIOS convertArrayToJson:arr];
            [self callback:200 response:response];
        }
       
    } failure:^(AFHTTPRequestOperation *operation, NSError *error) {
        NSLog(@"image upload error1 %ld", (long)[error code]);
        NSLog(@"image upload error2 %@", [error userInfo]);
        [self callback:-1 response:[error localizedDescription]];
    }];
}

- (void)callback:(int)statusCode response:(NSString*)response {
    
    
    ce::CEJsEngine::getInstance()->callback(self.jsCallback, statusCode, [response UTF8String]);

}

@end


NS_CE_BEGIN

CCFileUtils* CCFileUtils::getInstance()
{
    if (_instance == NULL)
    {
        _instance = new CCFileUtilsIOS();
        _instance->init();
    }
    return _instance;
}


static NSFileManager* s_fileManager = [NSFileManager defaultManager];

std::string CCFileUtilsIOS::getWritablePath()
{
    // save to document folder
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    std::string strRet = [documentsDirectory UTF8String];
    strRet.append("/");
    return strRet;
}

std::string CCFileUtilsIOS::getCachePath()
{
    // save to lib/cache folder
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSCachesDirectory, NSUserDomainMask, YES);
    NSString *documentsDirectory = [paths objectAtIndex:0];
    std::string strRet = [documentsDirectory UTF8String];
    strRet.append("/");
    return strRet;
}

bool CCFileUtilsIOS::isFileExist(const std::string& strFilePath)
{
    if (0 == strFilePath.length())
    {
        return false;
    }

    bool bRet = false;
    
    if (strFilePath[0] != '/')
    {
        std::string path;
        std::string file;
        size_t pos = strFilePath.find_last_of("/");
        if (pos != std::string::npos)
        {
            file = strFilePath.substr(pos+1);
            path = strFilePath.substr(0, pos+1);
        }
        else
        {
            file = strFilePath;
        }
        
        NSString* fullpath = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:file.c_str()]
                                                             ofType:nil
                                                        inDirectory:[NSString stringWithUTF8String:path.c_str()]];
        if (fullpath != nil) {
            bRet = true;
        }
    }
    else
    {
        // Search path is an absolute path.
        if ([s_fileManager fileExistsAtPath:[NSString stringWithUTF8String:strFilePath.c_str()]]) {
            bRet = true;
        }
    }
    
    return bRet;
}

bool CCFileUtilsIOS::isDirectoryExist(const std::string& strDirPath)
{
    if (0 == strDirPath.length())
    {
        return false;
    }

    bool bRet = false;

    if (strDirPath[0] == '/')
    {
        BOOL isDirectory = NO;
        if ([s_fileManager fileExistsAtPath:[NSString stringWithUTF8String:strDirPath.c_str()] isDirectory:&isDirectory]) {
            bRet = isDirectory == YES;
        }
    }

    return bRet;
}

std::string CCFileUtilsIOS::getFullPathForDirectoryAndFilename(const std::string& strDirectory, const std::string& strFilename)
{
    if (strDirectory[0] != '/')
    {
        NSString* fullpath = [[NSBundle mainBundle] pathForResource:[NSString stringWithUTF8String:strFilename.c_str()]
                                                             ofType:nil
                                                        inDirectory:[NSString stringWithUTF8String:strDirectory.c_str()]];
        if (fullpath != nil) {
            return [fullpath UTF8String];
        }
    }
    else
    {
        std::string fullPath = strDirectory+strFilename;
        // Search path is an absolute path.
        if ([s_fileManager fileExistsAtPath:[NSString stringWithUTF8String:fullPath.c_str()]]) {
            return fullPath;
        }
    }
    return "";
}

bool CCFileUtilsIOS::isAbsolutePath(const std::string& strPath)
{
    NSString* path = [NSString stringWithUTF8String:strPath.c_str()];
    return [path isAbsolutePath] ? true : false;
}

void CCFileUtilsIOS::uploadFiles(const char *url, const char *jsonData, JS_Function listener) {
    
    NSData *uploadImages = [[NSString stringWithUTF8String:jsonData] dataUsingEncoding:NSASCIIStringEncoding];
    NSError *error = nil;
    id jsonObject = [NSJSONSerialization JSONObjectWithData:uploadImages
                                                    options:NSJSONReadingAllowFragments
                                                      error:&error];
    FileUploader *uploader = [[FileUploader alloc] init];
    if (jsonObject) {
        NSDictionary *dic = (NSDictionary *)jsonObject;
        NSMutableArray *files = [dic objectForKey:@"files"];
        NSDictionary *paramsDic = [dic objectForKey:@"params"];
        [uploader uploadFiles:[NSString stringWithUTF8String:url] files:files params:paramsDic callback:listener];
    } else {
        CCLOG("convert %s to nsdict error", [NSString stringWithUTF8String:jsonData]);
        [uploader callback:false response:[NSString stringWithUTF8String:jsonData]];
    }
}

NS_CE_END
