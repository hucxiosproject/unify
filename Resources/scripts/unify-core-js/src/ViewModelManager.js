var ViewModelManager = {};

ViewModelManager.viewModels = {};

ViewModelManager.registerViewModel = function(viewmodel){
	if (!ViewModelManager.retrieveViewModel(viewmodel.name)) {
		print("viewmodel name is " + viewmodel.name);
		ViewModelManager.viewModels[viewmodel.name] = viewmodel;
	} else {
		print("Can't register viewmodel, (" + viewmodel.name + ") is registered! ");
	}
};

ViewModelManager.unregisterViewmodel = function(viewmodel) {
  if (!viewmodel || !ViewModelManager.retrieveViewModel(viewmodel.name)) return;

  if (typeof viewmodel.cleanup == "function") { viewmodel.cleanup(); }
  delete ViewModelManager.viewModels[viewmodel.name];
};

ViewModelManager.retrieveViewModel = function(viewmodelName) {
  return ViewModelManager.viewModels[viewmodelName];
};

module.exports = ViewModelManager;
