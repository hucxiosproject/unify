var Notification = require("./Notification");

Facade = {
  proxys: {}
};

Facade.registerProxy = function(proxy) {
    if (!Facade.retrieveProxy(proxy.name)) {
        Facade.proxys[proxy.name] = proxy;
    } else {
        print("Can't register proxy, (" + proxy.name + ") is registered! ");
    }
};

Facade.unregisterProxy = function(proxy) {
    if (!proxy || !Facade.retrieveProxy(proxy.name)) return;

    if (typeof proxy.cleanup == "function") { proxy.cleanup(); }
    delete Facade.proxys[proxy.name];
};

Facade.retrieveProxy = function(proxyName) {
    return Facade.proxys[proxyName];
};

module.exports = Facade;
