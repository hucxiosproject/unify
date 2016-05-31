var NotifyResult = function(code, data, params) {
	this.code = code;
	if (code == 200) {
		this.data = data;
		this.msg = "";
		if (!params)
			this.params = {};
		else
			this.params = params;
	} else {
		this.msg = data;
		this.params = {};
	}

	if (!this.data)
		this.data = {};
	if (!this.msg)
		this.msg = "";
};

module.exports = NotifyResult;
