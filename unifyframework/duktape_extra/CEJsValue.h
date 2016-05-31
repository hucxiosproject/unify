//
//  CEJsValue.h
//  ce
//
//  Created by kaixuan on 15-6-15.
//  Copyright (c) 2015年 kaixuan. All rights reserved.
//

#ifndef __ce__CEJsValue__
#define __ce__CEJsValue__

#include "common.h"
#include "duktape1.4.0/duktape.h"

#include <iostream>
#include <string>
#include <map>
#include <vector>


#define JS_NONE             CEJsValue(JsValueNone)
#define JS_UNDEFINED        CEJsValue(JsValueTypeUndefined)
#define JS_NULL             CEJsValue(JsValueTypeNull)
#define JS_BOOL(v)          CEJsValue::boolValue(v)
#define JS_NUMBER(v)        CEJsValue::numberValue(v)
#define JS_STRING(v)        CEJsValue::stringValue(v)
#define JS_Array(v)         CEJsValue::arrayValue(v)
#define JS_MAP(v)           CEJsValue::mapValue(v)



typedef enum {
    JsValueNone = 0,
    JsValueTypeUndefined,
    JsValueTypeNull,
    JsValueTypeBool,
    JsValueTypeNumber,
    JsValueTypeString,
    JsValueTypeArray,
    JsValueTypeMap,
} CEJsValueType;







class CEJsValue
{
public:
    typedef std::vector<CEJsValue> Array;
    typedef std::map<std::string, CEJsValue> Map;

    typedef union {
        int boolValue;
        double numberValue;
        std::string* stringValue;
        CEJsValue::Array* arrayValue;
        CEJsValue::Map* mapValue;
    } ValueField;

public:
    
    
    CEJsValue() : _type(JsValueNone)
    {
        memset(&_value, 0, sizeof(_value));
    }
    
    CEJsValue(CEJsValueType type) : _type(type)
    {
        memset(&_value, 0, sizeof(_value));
    }
    
    CEJsValue(const CEJsValue& rhs)
    {
        copy(rhs);
    }
    
    CEJsValue& operator =(const CEJsValue& rhs)
    {
        if (this != &rhs) { copy(rhs); }
        return *this;
    }
    
    bool operator <(const CEJsValue& rhs) const
    {
        return false;
    }
    
    ~CEJsValue()
    {
        switch (_type) {
            case JsValueTypeString:
                delete _value.stringValue;
                break;
                
            case JsValueTypeArray:
                delete _value.arrayValue;
                break;
                
            case JsValueTypeMap:
                delete _value.mapValue;
                break;
                
            default:
                break;
        }
    }
    
    static const CEJsValue boolValue(bool v)
    {
        CEJsValue value(JsValueTypeBool);
        value._value.boolValue = v;
        return value;
    }
    
    static const CEJsValue numberValue(double v)
    {
        CEJsValue value(JsValueTypeNumber);
        value._value.numberValue = v;
        return value;
    }
    
    static const CEJsValue stringValue(const char* v)
    {
        CEJsValue value(JsValueTypeString);
        value._value.stringValue = new std::string(v ? v : "");
        return value;
    }
    
    static const CEJsValue arrayValue(const CEJsValue::Array& v)
    {
        CEJsValue value(JsValueTypeArray);
        value._value.arrayValue = new CEJsValue::Array(v);
        return value;
    }
    
    static const CEJsValue mapValue(const CEJsValue::Map& v)
    {
        CEJsValue value(JsValueTypeMap);
        value._value.mapValue = new CEJsValue::Map(v);
        return value;
    }
    
    
    CEJsValueType getType() const { return _type; }
    
    
    bool boolValue() const {
        return _value.boolValue;
    }
    
    double numberValue() const {
        return _value.numberValue;
    }
    
    int intValue() const {
        return int(_value.numberValue);
    }
    
    const std::string& stringValue() const {
        return *_value.stringValue;
    }
    
    const char* cstringValue() const {
        return stringValue().c_str();
    }
    
    CEJsValue::Map mapValue() const {
        return *_value.mapValue;
    }
    
    
    std::string toString() const {
        switch (_type) {
            case JsValueNone: return "none";
            case JsValueTypeUndefined: return "undefined";
            case JsValueTypeNull: return "null";
            case JsValueTypeBool: return this->boolValue() ? "true" : "false";
            case JsValueTypeNumber: return ce::to_string(this->numberValue());
            case JsValueTypeString: return this->cstringValue();
            case JsValueTypeArray: return "array";
            case JsValueTypeMap: return "map";
            default: break;
        }
    }
    
private:
    void copy(const CEJsValue& rhs)
    {
        memcpy(&_value, &rhs._value, sizeof(_value));
        _type = rhs._type;
        
        if (_type == JsValueTypeString)
        {
            _value.stringValue = new std::string(*rhs._value.stringValue);
        }
        else if (_type == JsValueTypeArray)
        {
            _value.arrayValue = new CEJsValue::Array(*rhs._value.arrayValue);
        }
        else if (_type == JsValueTypeMap)
        {
            _value.mapValue = new CEJsValue::Map(*rhs._value.mapValue);
        }
    }
    
    
private:
    CEJsValueType _type;
    ValueField _value;
};


CEJsValue js_to_cejs_value(duk_context* ctx, int index);



#endif
