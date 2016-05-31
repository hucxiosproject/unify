var Model = function(unify) {
  this.unify = unify;
};

Model.prototype.notify = function(name, body) {
  unify.Notification.notify(name, body);
};

module.exports = Model;
