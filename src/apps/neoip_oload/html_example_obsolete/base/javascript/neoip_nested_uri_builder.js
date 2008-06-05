/**

\par Brief Description
this script implement a builder of nested uri. mainly for neoip-oload

\par TODO
- this has been coded at my very begining js time. so real crappy
- make it more flexible. nested_uri_build_t.as is much better
- handle the metavar of the inner uri
 
*/


// defined the namespace
if( typeof neoip == 'undefined' )	var neoip	= {};

/**
 *
 */
neoip.nested_uri_builder = function() {
	var _outter_uri;
	var _inner_uri;
	var _subfile_path;
	var _subfile_byteoffset;
	var _mod;
	var _link_type;
	var _dupuri_arr;
	var _httpo_maxrate;
	var _httpo_maxrate_thres;
	var _read_ahead;
	var _announce_uri;
	var _piecelen;	
}


neoip.nested_uri_builder.prototype.outter_uri = function(value) {
	this._outter_uri	= value;
	// return the object itself
	return this;
}
neoip.nested_uri_builder.prototype.inner_uri = function(value) {
	this._inner_uri	= value;
	// return the object itself
	return this;
}
neoip.nested_uri_builder.prototype.subfile_path = function(value) {
	this._subfile_path	= value;
	// return the object itself
	return this;
}
neoip.nested_uri_builder.prototype.subfile_byteoffset = function(value) {
	this._subfile_byteoffset	= value;
	// return the object itself
	return this;
}
neoip.nested_uri_builder.prototype.mod = function(value) {
	this._mod		= value;
	// return the object itself
	return this;
}
neoip.nested_uri_builder.prototype.link_type = function(value) {
	this._link_type		= value;
	// return the object itself
	return this;
}
neoip.nested_uri_builder.prototype.dupuri = function(value) {
	// init the _dupuri if not yet done
	if( !this._dupuri_arr )	this._dupuri_arr = new Array();
	// append the new value into the _dupuri_arr
	this._dupuri_arr.push(value);
	// return the object itself
	return this;
}

neoip.nested_uri_builder.prototype.httpo_maxrate = function(value) {
	this._httpo_maxrate	= value;
	// return the object itself
	return this;
}

neoip.nested_uri_builder.prototype.httpo_maxrate_thres = function(value) {
	this._httpo_maxrate_thres	= value;
	// return the object itself
	return this;
}

neoip.nested_uri_builder.prototype.read_ahead = function(value) {
	this._read_ahead	= value;
	// return the object itself
	return this;
}

neoip.nested_uri_builder.prototype.announce_uri = function(value) {
	this._announce_uri	= value;
	// return the object itself
	return this;
}

neoip.nested_uri_builder.prototype.piecelen = function(value) {
	this._piecelen		= value;
	// return the object itself
	return this;
}

neoip.nested_uri_builder.prototype.set_outter_var = function(key, value)
{
	// call the nested_uri function 
	// - TODO: is this eval() dangerous ?
	// - NOTE: little kludge to handle dupuri to get multiple dupuri in a array
	//   - any outter_var key containing dupuri string is a play dupuri
	//   - typically if there is several dupuri use dupuri_1 dupuri_2 etc...
	if( key.match("dupuri") ){
		eval('this.dupuri("' + value+ '")');
	}else{
		eval('this.' + key + '("' + value + '")');
	}
	// return the object itself
	return this;
}


neoip.nested_uri_builder.prototype.set_outter_var_arr = function(outter_var_arr)
{
	for(var key in outter_var_arr )	this.set_outter_var(key, outter_var_arr[key]);
}

/** \brief Return a string of the nested_uri to use
 *
 */
neoip.nested_uri_builder.prototype.to_string = function() {
	var	result	= "";
	result	+= this._outter_uri;
	// append the mod, if specified
	if( this._mod )		result	+= "/" + this._mod;
	// append the link_type if specified
	if( this._link_type )	result	+= "/*link_type*" + this._link_type;
	// append the httpo_maxrate if specified
	// - TODO the this._httpo_maxrate is not usable as httpo_maxrate may be 0
	// - it is a generic issue on testing if a variable is set or not
	// - aka find a good way to do it, and do it regularly everywhere
	if( typeof(this._httpo_maxrate) != "undefined"){
	 	result	+= "/*httpo_maxrate*" + this._httpo_maxrate;
		if( typeof(this._httpo_maxrate_thres) != "undefined"){
		 	result	+= "/*httpo_maxrate_thres*" + this._httpo_maxrate_thres;
	 	}
	}
	// append the read_ahead if specified
	if( this._read_ahead )	result	+= "/*read_ahead*" + this._read_ahead;
	// append the read_ahead if specified
	if( this._subfile_byteoffset )	result	+= "/*subfile_byteoffset*" + this._subfile_byteoffset;
	// append the piecelen if specified
	if( this._piecelen )	result	+= "/*piecelen*" + this._piecelen;

	// append the announce_uri if specified
	if( this._announce_uri){
		// NOTE: the uri is encoded in base64 with url-safe alphabet as an escaping mechanism.
		result	+= "/*announce_uri*" + neoip_base64.encode_safe(this._announce_uri);
	}
	
	// if some dupuri are specificied, write them now
	if( this._dupuri_arr ){
		// go thru the whole _dupuri_arr
		for(var i = 0; i < this._dupuri_arr.length; i++){
			// NOTE: the dupuri is encoded in base64 with url-safe alphabet
			// as an escaping mechanism.
			result	+= "/*dupuri*" + neoip_base64.encode_safe(this._dupuri_arr[i]);
		}
	}

	// add the subfile outter variable if there is a subfile_path
	if( this._subfile_path ){
		var	nb_elem = 1;
		// count the number of element of the subfile_path
		for(var i = 0; i < this._subfile_path.length; i++){
			if( this._subfile_path.charAt(i) == '/' )	nb_elem++;
		}
		// add the subfile outter variable
		result	+= "/*subfile_level*" + nb_elem;
	}


	result	+= "/";

	// split the inner_uri
	query_start	= this._inner_uri.indexOf("?")
	if( query_start != -1 ){
		// TODO i dont like this '7' which is "http:/".length
		var inner_1st	= this._inner_uri.substr(7, query_start - 7)
		var inner_2nd	= this._inner_uri.substr(query_start+1)
	}else{
		var inner_1st	= this._inner_uri.substr(7)
		var inner_2nd	= null;
	}

	// append the inner_uri
	// - it is rather complex due to the need to add a subfile_path if it exist
	// - TODO i use http%3a/ to workaround a bug in vlc mozilla plugin 0.8.6 :)
	//   -  http://example.com/http://slota.bla is interpreted as http://slota.bla
	//   - im not sure about the escaping anyway
	result	+= "http%3a/"
	result	+= inner_1st;
	if( this._subfile_path)	result	+= "/" + this._subfile_path;
	if( inner_2nd )		result	+= "?" + inner_2nd;

	// return the result
	return	result;
}
