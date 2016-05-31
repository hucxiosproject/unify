module.exports = {
  /**
  ios: ios
  android: android
  */
  platform: ["node", "ios", "android"][PlatformHelper.getTargetPlatform()],

  /**
  获取当前设备的语言
  */
  getCurrentLanguage: function() {
      return PlatformHelper.getCurrentLanguage();
  }
};
