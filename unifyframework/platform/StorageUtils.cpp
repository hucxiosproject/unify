//
//  StorageUtils.cpp
//  unifyframework
//
//  Created by Vlad on 4/13/16.
//  Copyright © 2016 vocinno. All rights reserved.
//

#include "StorageUtils.h"

#include "CCFileUtils.h"

NS_CE_BEGIN




StorageUtils::StorageUtils()
{
    
    std::string filePath =  CCFileUtils::getInstance()->getWritablePath();
    
    
    leveldb::DB* db;
    leveldb::Options options;
    options.create_if_missing = true;
    lastStatus = leveldb::DB::Open(options, filePath + "/general", &db);
    assert(lastStatus.ok());
    
    levelDb = std::shared_ptr<leveldb::DB>(db);
}
 
void StorageUtils::SetValueForKey(const std::string& key, const std::string& value)
{
    lastStatus = levelDb->Put(leveldb::WriteOptions(), key, value);
    
    assert(lastStatus.ok());
}
 
std::string StorageUtils::GetValueForKey(const std::string& key, const std::string& defaultValue)
{
    std::string value;
    
    lastStatus = levelDb->Get(leveldb::ReadOptions(), key, &value);
    
    if (lastStatus.IsNotFound())
    {
        return defaultValue;
    }
    
    return value;
}

std::shared_ptr<StorageUtils> storageUtils;


std::shared_ptr<StorageUtils> StorageUtils::getStorageUtils()
{
    if (storageUtils == nullptr)
    {
        storageUtils = std::shared_ptr<StorageUtils>(new StorageUtils());
    }
    return storageUtils;
}




NS_CE_END
