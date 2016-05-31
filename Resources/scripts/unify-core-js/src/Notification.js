var Notification = {
  observers: {}
};

Notification.addObserver = function(name, observer) {
    Notification.observers[name] = Notification.observers[name] || [];
    Notification.observers[name].push({
        name: name,
        observer: observer,
    });
};


Notification.removeObserver = function(name, observer) {
    var obs = Notification.observers[name];
    if (obs && obs.length > 0) {
        var i, delIndexs = [];
        for (i = 0; i < obs.length; i++) {
            if (obs[i].observer == observer) {
                delIndexs.push(i);
            }
        }

        for (i = 0; i < delIndexs.length; i++) {
            obs.splice(delIndexs, 1);
        }
    }
};

Notification.notifyJs = function(name, body) {
    var obs = Notification.observers[name];
    if (obs && obs.length > 0) {
        for (var i in obs) {
            var observer = obs[i].observer;
            observer.handleNotification({ name : name , body: body });
        }
    }
};

module.exports = Notification;
