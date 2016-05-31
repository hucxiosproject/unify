#include "CELocalKeyValueUtils.h"
#include "jni/CELibHelper.h"

NS_CE_BEGIN

bool CELocalKeyValueUtils::saveInteger(const char* key, int value) {
	unify::setIntegerForKeyJNI(key, value);
	return true; 
}

bool CELocalKeyValueUtils::saveString(const char *key, const char *value) {
	unify::setStringForKeyJNI(key, value);
	return true;
}

int CELocalKeyValueUtils::getInteger(const char *key, int defaultValue) {
	return unify::getIntegerForKeyJNI(key, defaultValue);
}

std::string CELocalKeyValueUtils::getString(const char *key, const char *defaultValue) {
	return unify::getStringForKeyJNI(key, defaultValue);
}

NS_CE_END