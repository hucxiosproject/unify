/*依赖与以下object
io: {
  read: function(){},
  write: function(){},
  readFromZip: function(){},
  saveInteger: function(){},
  getInteger: function(){},
  saveString: function(){},
  getString: function(){}
}
http: {
  post: function() {},
  get: function() {},
  delete: function() {},
  put: function() {}
}
image: {
  download: function(),
  upload: function()
}
print: function()
notify: function()
PlatformHelper: {
  getTargetPlatform: function(){},
  getCurrentLanguage: function(){}
}
*/
var init = function() {
  var unify = {};
  unify.Notification = require("./Notification");
  unify.Model = require("./Model");
  unify.ViewModel = require("./ViewModel");
  unify.Facade = require("./Facade");
  unify.ViewModelManager = require("./ViewModelManager");
  unify.HttpStatusCode = require("./HttpStatusCode");
  unify.NotifyResult = require("./NotifyResult");

  global.unify = unify;
  // cache init
  global.cache = {};
  global.cache.init = true;

  return unify;
};

module.exports = init;
