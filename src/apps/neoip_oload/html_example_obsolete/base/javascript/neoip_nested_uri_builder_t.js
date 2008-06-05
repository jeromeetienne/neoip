/*! \file
    \brief Definition of the nested_uri_builder_t

\par Brief Description
nested_uri_builder_t is a class to help build neoip-oload nested uri.

\par Implementation notes
- this object exist in javascript and actionscript. it is very similar in both
  - neoip_nested_uri_builder_t.js and neoip_nested_uri_builder_t.as
  - this is a straight forward porting
  - any modification made in one, must be done in the other

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
neoip.nested_uri_builder_t = function()
{
	// zero some values
	this.m_dupuri_arr	= new Array;
	this.m_var_arr		= new Array;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief to set the inner_uri
 */
neoip.nested_uri_builder_t.prototype.inner_uri	= function(val)	{ this.m_inner_uri = val;}

/** \brief to set the outter_uri
 */
neoip.nested_uri_builder_t.prototype.outter_uri	= function(val)	{ this.m_outter_uri= val;}


/** \brief To set any outter_var
 */
neoip.nested_uri_builder_t.prototype.set_var	= function(key, val)
{
	if( key == "dupuri" ){
		// handled separatly because it is legitimate to have multiple dupuri
		this.m_dupuri_arr.push(val);
	}else if( key == "subfile_path" ){
		// add a variable 'subfile_level' in m_var_arr to give the 'level' of the path		
		this.m_var_arr['subfile_level']	= val.split("/").length - 1;
		// copy the value
		this.m_subfile_path	= val;
	}else{
		// push this variable into the m_var_arr
		this.m_var_arr[key]	= val;
	}
}

/** \brief Helper to set outter_var via an array
 */
neoip.nested_uri_builder_t.prototype.set_var_arr	= function(var_arr)
{
	// set all the variables of var_arr as outter_var
	for(var key in var_arr)	this.set_var(key, var_arr[key]);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief throw an exception is this object is not considered sane 
 */
neoip.nested_uri_builder_t.prototype._is_sane_internal	= function()
{
	if( !this.m_outter_uri )	throw new Error("No outter_uri");
	if( !this.m_inner_uri )		throw new Error("No inner_uri");
	
	// TODO do all the sanity check here
	// - if subfile_level exist, a subfile_path MUST too
	// - subfile_path MUST always start with '/'
	// - if 'type' check the value is a legal one
	// - if 'mod' check the value is a legal one
	// - for dupuri and announce uri, it MUST start by 'http://'
}

/** \brief If this object is considered sane, return true. false otherwise
 */
neoip.nested_uri_builder_t.prototype.is_sane	= function()
{
	try {
		// call the version with exception
		this._is_sane_internal();
	}catch(error) {
		console.log("nested_uri_build_t not sane due to " + error);
		return	false;		
	}
	// if all previous tests passed, this is considered sane
	return true;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			to_string() function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return a string of the nested_uri
 */
neoip.nested_uri_builder_t.prototype.to_string	= function()
{
	var	result	= "";
	// sanity check - the object MUST be sane
	console.assert( this.is_sane() );

	// start building the nested_uri
	result	+= this.m_outter_uri + "/";
	
	// put the 'mod' variable first
	if( this.m_var_arr['mod'] )	result += this.m_var_arr['mod'] + "/";
	
	// put all the outter variables
	for(var var_key in this.m_var_arr){
		// if 'mod', goto the next - it has already been handled
		if( var_key == "mod" )				continue;
		// if it is a "neoip_metavar_", goto the next - it will be handled later
		if( var_key.indexOf("neoip_metavar_") == 0 )	continue;
		// put the key of the variable
		result	+= "*" + var_key + "*";
		// put the values according to the keys
		if( var_key == "announce_uri" ){
			// announce uri is specific - values are encoded in base64-urlsafe
			result	+= neoip_base64.encode_safe(this.m_var_arr[var_key]);
		}else{
			result	+= this.m_var_arr[var_key];
		}
		// add the separator
		result	+= "/";
	}
	// put all the dupuri with value in base64-urlsafe encoding
	for(var dupuri_idx in this.m_dupuri_arr){
		result	+= "*dupuri*";
		result	+= neoip_base64.encode_safe(this.m_dupuri_arr[dupuri_idx]);
		result	+= "/";
	}

	// put the inner_uri at the end
	// - made complex by the need to put the m_subfile_path between the 
	//   path and the query part of the inner_uri
	var	query_pos		 = this.m_inner_uri.indexOf("?");
	if( query_pos != -1 )	result	+= this.m_inner_uri.substr(0, query_pos);
	else			result	+= this.m_inner_uri;
	if(this.m_subfile_path)	result	+= this.m_subfile_path
	if( query_pos != -1 )	result	+= this.m_inner_uri.substr(query_pos, this.m_inner_uri.length);
	
	// put all the inner variables aka "neoip_metavar_"
	for(var var_key in this.m_var_arr){
		// if it is NOT a "neoip_metavar_", goto the next - it has been handled before
		if( var_key.indexOf("neoip_metavar_") != 0 )	continue;
		// put the variable separator
		result	+= result.indexOf('?') == -1 ? "?" : "&";
		// put the key of the variable
		result	+= var_key + "=" + escape(this.m_var_arr[var_key]);
	}
	

	// return the just built nested_uri
	return result;
}

