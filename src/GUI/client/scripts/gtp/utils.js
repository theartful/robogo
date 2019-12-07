let socket = new WebSocket("ws://localhost:9002");

/*
 * object.watch polyfill
 *
 * 2012-04-03
 *
 * By Eli Grey, http://eligrey.com
 * Public Domain.
 * NO WARRANTY EXPRESSED OR IMPLIED. USE AT YOUR OWN RISK.
 */

// object.watch
if (!Object.prototype.watch) {
	Object.defineProperty(Object.prototype, "watch", {
		  enumerable: false
		, configurable: true
		, writable: false
		, value: function (prop, handler) {
			var
			  oldval = this[prop]
			, newval = oldval
			, getter = function () {
				return newval;
			}
			, setter = function (val) {
				oldval = newval;
				return newval = handler.call(this, prop, oldval, val);
			}
			;
			
			if (delete this[prop]) { // can't watch constants
				Object.defineProperty(this, prop, {
					  get: getter
					, set: setter
					, enumerable: true
					, configurable: true
				});
			}
		}
	});
}

// object.unwatch
if (!Object.prototype.unwatch) {
	Object.defineProperty(Object.prototype, "unwatch", {
		  enumerable: false
		, configurable: true
		, writable: false
		, value: function (prop) {
			var val = this[prop];
			delete this[prop]; // remove accessors
			this[prop] = val;
		}
	});
}

/**
 * @param   {function}  func    Function that is define in ES6 arrow functoin style
 * @returns {Array}     args    Array containing argument list of the function
 */
let getArrowFunctionArgList = (func) => {
    let firstLine = func.toString().split('\n')[0];
    let args = firstLine.match(/\((.*?)\)/)[1].replace(/ /g,'').split(',');

    args = (args.length === 1 && args[0] === "") ? [] : args;
    return args;
}

/**
 * @param   {Array}     arr1        First Array
 * @param   {Array}     arr2        Second Array
 * @returns {Boolean}   matched     "true" if both array are indentical, "false" otherwise
 */
let arraysMatch = (arr1, arr2) => {
	// Check if the arrays are the same length
	if (arr1.length !== arr2.length) return false;

	// Check if all items exist and are in the same order
	for (var i = 0; i < arr1.length; i++) {
		if (arr1[i] !== arr2[i]) return false;
	}

	// Otherwise, return true
	return true;

};

/**
 * 
 * @param   {string}    num     String that contains a float value
 * @returns {float}     float   integer value of num
 */
let toFloat = (num) => {
    let float = parseFloat(num, 10);
    if (float.toString() !== num)
        throw "not a float value";

    return float;
};

/**
 * 
 * @param   {string}    num    String that contains an integer value
 * @returns {int}       int    integer value of num
 */
let toInt = (num) => {
    let int = parseInt(num, 10);
    if (int.toString() !== num)
        throw "not an integer value";

    return int;
};

/**
 * 
 * @param   {number}    num     Variable to check if it's integer or not
 * @returns {Boolean}   isInt   "true" if num is an integer, "false" otherwise
 */
let isInt = (num) => {
    if (typeof num === "number" && num === parseInt(num, 10) && parseInt(num, 10) >= 0)
        return true;

	return false;
};