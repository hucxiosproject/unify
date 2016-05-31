//
//  StorageUtils.h
//  unifyframework
//
//  Created by Vlad on 4/13/16.
//  Copyright © 2016 vocinno. All rights reserved.
//

#ifndef StorageUtils_h
#define StorageUtils_h

#include <cassert>
#include <memory>
#include "common.h"
#include "leveldb/db.h"

NS_CE_BEGIN

class StorageUtils
{
public:
    StorageUtils();

    void SetValueForKey(const std::string& key, const std::string& value);
    
    std::string GetValueForKey(const std::string& key, const std::string& defaultValue);

    static std::shared_ptr<StorageUtils> getStorageUtils();
    
protected:
    std::shared_ptr<leveldb::DB> levelDb;
    
    leveldb::Status lastStatus;
    
};

NS_CE_END

#endif /* StorageUtils_h */
