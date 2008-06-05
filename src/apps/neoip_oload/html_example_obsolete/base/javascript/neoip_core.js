/*! \file
    \brief Definition of various core functions

\par Brief Description
This file contains various core functions. which are very close to js
- e.g. neoip.object_clone do a copy of js object

*/

// defined the namespace if not yet done
if( typeof neoip == 'undefined' )	var neoip	= {};
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
neoip.core_t	= function(){}

// create neoip.core_t as an object with only static functions
neoip.core	= new neoip.core_t();

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			object_clone
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Copy a javascript object (if reccursive == true, this is a deep copy, else it is 
 *         it is a shallow copy
 */
neoip.core_t.prototype.object_clone	= function(src_obj, reccursive)
{
	// if src_obj is not an 'object', simply return it
	if(typeof(src_obj) != 'object')	return src_obj;
	// if src_obj is the null object, simply return it
	if(src_obj == null)		return src_obj;
	// create the new object
	var dst_obj	= new Object();
	// copy each field of src_obj into dst_obj
	if( reccursive == true ){
		for(var i in src_obj)	dst_obj[i] = neoip.object_clone(src_obj[i], reccursive);
	}else{
		for(var i in src_obj)	dst_obj[i] = src_obj[i];
	}
	// return the newly created object
	return dst_obj;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			build_nonce_str
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a nonce_str of nonce_len character 
 * 
 * - take random number and convert them into [A-Z][a-z][0-9]
 */
neoip.core_t.prototype.build_nonce_str	= function(nonce_len)
{
	var nonce	= "";
	// loop nonce_len time
	for(var i = 0; i < nonce_len; i++){
		var rnd	= Math.floor(Math.random() * (26 + 26 + 10));
		if( rnd < 26 ){
			nonce += String.fromCharCode(rnd 	+ "A".charCodeAt(0));
		}else if( rnd < (26+26)){
			nonce += String.fromCharCode(rnd-(26)	+ "a".charCodeAt(0));
		}else{
			nonce += String.fromCharCode(rnd-(26+26)+ "0".charCodeAt(0));
		}
	}
	// return the just built nonce
	return nonce;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			flash_wait4init
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief to wait for a given flash element to be fully initialized by the browser
 * 
 * - this is done by testing if the flash_elem id="html_id" exist
 *   and if so, if this element for a function called "fct_str"
 * - once it is found, the callback is notified
 * - there is a exponantial backoff
 */
// TODO pass this function in neoip.core_t - some issue with the timeout and closure
objembed_wait4init	= function(html_id, fct_str, cur_delay, max_delay, callback)
{
	// get the flash_elem in the page
	var flash_elem	= document.getElementById(html_id);
	// test if flash_elem exists and has fct_str
	if( flash_elem == undefined || eval("typeof(flash_elem."+fct_str+")") != "function"){
		// log to debug
		//console.info("html_id("+html_id+"."+fct_str+") is NOT loaded... retrying in " + cur_delay/1000 + "-sec");
		// do the exponantial backoff, clamped by max_delay
		var next_delay	= cur_delay * 2;
		next_delay	= Math.min(next_delay, max_delay);
		// init the timeout for the next call
		var timeout_cb	= function() {
			objembed_wait4init(html_id, fct_str, next_delay, max_delay, callback); 
		};
		setTimeout(timeout_cb, cur_delay, this);
		return;
	}
	// log to debug
	//console.info("html_id("+html_id+"."+fct_str+") is NOW loaded... notifying the callback");
	// notifying the callback
	callback();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			dom event
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief add a event_listener to an object
 */
neoip.core_t.prototype.dom_event_listener	= function(obj, event_type, fct)
{ 
	if( obj.addEventListener ){
		obj.addEventListener(event_type, fct, false);
		return true; 
	}else if( obj.attachEvent ){
		return	obj.attachEvent("on" + event_type, fct);
	}else{
		return false;
	} 
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief constructor for a basic_cb_t
 *
 * - see http://www.dustindiaz.com/javascript-curry/ for principle description 
 */
neoip.basic_cb_t	= function(fct, p_scope) 
{
	var	scope	= p_scope || window;
	return	function() { fct.call(scope); };
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			download_file_insync
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Load the content of a url synchronously
 */
neoip.core_t.prototype.download_file_insync	= function(url)
{
	var xmlhttp	= new XMLHttpRequest();
	// do a open insync
	xmlhttp.open("GET", url, false);
	// start the request
	xmlhttp.send(null);
	// log to debug
	//console.info("response=" + xmlhttp.responseText);
	//console.info("status=" + xmlhttp.status);
	// if request failed, return a null object 
	if( xmlhttp.status != 200 )	return null;
	// else return the content of the file as text
	return xmlhttp.responseText;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			cookie management
// - taken from http://www.quirksmode.org/js/cookies.html
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief create a cookie
 */
neoip.core_t.prototype.cookie_write	= function(name,value,days)
{
	if (days) {
		var date = new Date();
		date.setTime(date.getTime()+(days*24*60*60*1000));
		var expires = "; expires="+date.toGMTString();
	}
	else var expires = "";
	document.cookie = name+"="+value+expires+"; path=/";
}

/** \brief read a cookie
 */
neoip.core_t.prototype.cookie_read	= function(name)
{
	var nameEQ = name + "=";
	var ca = document.cookie.split(';');
	for(var i=0;i < ca.length;i++) {
		var c = ca[i];
		while (c.charAt(0)==' ') c = c.substring(1,c.length);
		if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length,c.length);
	}
	return null;
}

/** \brief delete a cookie
 */
neoip.core_t.prototype.cookie_delete	= function(name)
{
	createCookie(name,"",-1);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			doc_urivar_get
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to extract the variable from the page location url
 */
neoip.core_t.prototype.doc_urivar_get	= function(varname)
{
	var query_str	= window.location.search.substring(1);
	var urivar_arr	= query_str.split("&");
	// go thru all the variable of the uri
	for(var i=0; i < urivar_arr.length; i++ ){
		var pair = urivar_arr[i].split("=");
		// if this variable name matches, return its value
		if( pair[0] == varname) return pair[1];
	}
	// if no variable matches, return null
	return null;
}

	