load('api_pwm.js');
load('api_gpio.js');
load('api_sys.js');

let timeProfile = {};
let logHead = (typeof logHead !== 'undefined') ? logHead : "TOOLS - ";

let TOOLS = {
	name: "TOOLS",
	_doProfile: false,
	_last: 0,
	_debugTime: 0,
	_getMacAddress: ffi('char *tools_get_mac_addr_fmt(int)'),
	_getDeviceId: ffi('char *tools_get_device_id(void)'),
	_getIpAddr: ffi('char *tools_get_device_ip(void)'),
	_toUpper: ffi('void tools_to_upper_case(char *)'),
	_toLower: ffi('void tools_to_lower_case(char *)'),
	_getFileSystemInfo: ffi('char *tools_get_fs_info(char *)'),

	mergeObjects: function(objA, objB, clone) {
		let key, res = {};
		if (clone === true) {
			objA = JSON.parse(JSON.stringify(objA));
			objB = JSON.parse(JSON.stringify(objB));
		}
		for (key in objA) {
			res[key] = objA[key];
		}
		for (key in objB) {
			res[key] = objB[key];
		}
		return res;
	},

	getKeys: function(obj) {
		let key, res = [];
		for (key in obj) {
			res.push(key);
		}
		return res;
	},

	replaceString: function(inTxt, search, replace, all) {
		let searchLen = search.length;
		let out = '';
		let nPos = inTxt.indexOf(search);
		let restTxt = inTxt;
		while (nPos > -1) {
			let left = (nPos > 0) ? restTxt.slice(0, nPos) : '';
			out = out + left + replace;
			restTxt = restTxt.slice(nPos + searchLen);
			nPos = (all === true) ? restTxt.indexOf(search) : -1;
		}
		return out + restTxt;
	},

	splitStringFrom: function(inTxt, sepChr, start) {
		let buff = '';
		let out = [];
		let found = 0;
		for (let i = 0; i < inTxt.length; i++) {
			if (inTxt[i] !== sepChr) {
				if (found >= start) {
					buff = buff + inTxt[i];
				}
			} else {
				if (buff.length > 0) {
					if (found >= start) {
						out.push(buff);
					}
				}
				buff = '';
				found++;
			}
		}
		if (buff.length > 0) {
			out.push(buff);
		}
		return out;
	},
	
	splitString: function(inTxt, sepChr, noTrim) {
		let buff = '';
		let out = [];
		for (let i = 0; i < inTxt.length; i++) {
			if (inTxt[i] !== sepChr) {
				buff = buff + inTxt[i];
			} else {
				if (buff.length > 0) {
					out.push(noTrim ? buff : this.trimString(buff));
					buff = '';
				}
			}
		}
		if (buff.length > 0) {
			out.push(buff);
		}
		return out;
	},
  
	joinArrToString: function(inArr, sepChr, noTrim) {
		let out = '';
		for (let i = 0; i < inArr.length; i++) {
			let entry = inArr[i];
			entry = (noTrim === true) ? entry : this.trimString(entry);
			out = out + ((out.length > 0) ? sepChr : '') + entry;
		}
		return out;
	},

  ltrimString: function(inTxt) {
  	let len = inTxt.length - 1;
	  let i = 0;
	  let found = false;
    while(i <= len && !found) {
    	found = (inTxt[i] === ' ');
    	i++;
   	}
    if (found) {
	    inTxt.splice(0, len - i - 1);
    }
    return inTxt;
  },
 
  rtrimString: function(inTxt) {
  	let len = inTxt.length - 1;
  	let i = len;
	  let found = false;
    while(i >= 0 && !found) {
    	found = (inTxt[i] === ' ');
    	i--;
    }
    if (found) {
	    inTxt.splice(i + 1, len);
    }
    return inTxt;
  },

	trimString: function(inTxt) {
		// just call rtrim and ltrim
	  return this.ltrimString(this.rtrimString(inTxt));
	},
	
	checkTime: function() {
	  let time = Timer.now();
	  let tStamp = Math.floor(time ? time : 0);
		return (tStamp > 1514764800);
	},

	profileTime: function(msg, start, end) {
		if (this._doProfile === false) {
			return;
		}
		
		if (typeof start === 'object') {
			start.duration = 0;
			start.time = Math.floor(Timer.now() * 100);
		}
		let first = this.last === 0 ? true : false;
		this._last = this._debugTime;
		this._debugTime = first ? 0.0 : Math.floor(Timer.now() * 100);
		if (typeof end === 'object') {
			end.duration = Math.floor(Timer.now() * 100) - end.time;
			end.time = Math.floor(Timer.now() * 100);
			Log.info(logHead + 'Timer step: (' + msg + ' -> Duration - ' + JSON.stringify(end.duration / 100.0) + ' s)');		
		} else {
			Log.info(logHead + 'Timer step: (' + msg + ') - ' + JSON.stringify((this._debugTime - this._last) / 100.0) + ' s');		
		}
	},

	getFileSystemInfo: function(path) {
		return this._getFileSystemInfo(path);
	},

	getMacAddress: function(separator) {
		let sep = separator ? separator : 58;
		return this._getMacAddress(sep);
	},

	getDeviceId: function() {
		return this._getDeviceId();
	},

	getDeviceIpAddr: function() {
		return this._getIpAddr(0);
	},

	toUpperCase: function(inTxt) {
		let outTxt = ("out" + inTxt);
		this._toUpper(outTxt);
		return outTxt.slice(3);
	},

	toLowerCase: function(inTxt) {
		let outTxt = ("out" + inTxt);
		this._toLower(outTxt);
		return outTxt.slice(3);
	},

	isInArr: function(entry, arrTest) {
		for (let i = 0; i < arrTest.length; i++) {
			if (entry === arrTest[i]) {
				return true;
			}
		}
		return false;
	}

}
