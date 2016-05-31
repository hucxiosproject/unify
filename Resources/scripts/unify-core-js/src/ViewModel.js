var ViewModel = function(unify) {
    this.name = "";
    this.unify = unify;
};

ViewModel.prototype.init = function() {
    print("viewmodel init");
    var interests = this.listNotificationInterests();
    for (var i in interests) {
        this.unify.Notification.addObserver(interests[i], this);
    }

    // register viewmodel to manager
    // this.unify.ViewModelManager.registerViewModel(this);
};

/**
notify to native
@param name notice name
@param body notice data
*/
ViewModel.prototype.notify = function(name, body) {
    // var NC = NotificationCenter.getInstance();
    // NotificationCenter.notify(NC, name, body);

    //notify是原生方法，由运行平台实现
    notify.notify(name, body);
};

ViewModel.prototype.cleanup = function() {
    var interests = this.listNotificationInterests();
    for (var i in interests) {
        this.unify.Notification.removeObserver(interests[i], this);
    }
};

ViewModel.prototype.clearData = function() {

};

ViewModel.prototype.listNotificationInterests = function() {
    return [];
};

ViewModel.prototype.handleNotification = function(notification) {
    print("receive notification:", notification.name);
};

ViewModel.prototype.getInstance = function() {
    return this;
};

module.exports = ViewModel;
