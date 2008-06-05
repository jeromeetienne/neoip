/*! \file
    \brief Definition of the \ref string_t class

http://inner.org/inner_path/var_key=value/

\par Possible Improvement
- add the possibility to have variable and anchor in the outter url
  - http://outter.org/outter_path/var_key=value/http/innet.org
  - aka put a predefined suffix to the var and make it appears as a name in the path

*/

/* system include */
#include <string>
/* local include */
#include "neoip_http_nested_uri.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref http_nested_uri_t constant
// TODO issue here the ESC_STRING is in the http_uri_t::UNRESERVED_CHARSET
const std::string	http_nested_uri_t::ESC_STRING	= "*";
// end of constants definition


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 basic testing function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the http_uri_t is valid to be a outter http_uri_t in http_nested_uri_t
 * 
 * - Currently the requirements are:
 *   - anchor MUST NOT be present
 *   - variable MUST NOT be present
 *   - the path MUST NOT contains a url scheme keyword (currently it is only 'http:')
 */
bool	http_nested_uri_t::is_valid_outter(const http_uri_t &outter_uri)	throw()
{
	// log to debug
	KLOG_DBG("enter outter_uri=" << outter_uri);
	// if the outter_uri is null, return false
	if( outter_uri.is_null() )		return false;

	// if the outter_uri contains the seperator_esc, this is not a valive outter_uri
	std::string	outter_uri_str	= outter_uri.path_str();
	std::string	separator_esc	= "/http%3a/";
	size_t		separator_pos	= outter_uri_str.find(separator_esc);
	if( separator_pos != std::string::npos )	return false;	

	// if the path contain a url scheme keyword, return false
	const file_path_t & uri_path	= outter_uri.path();
	for(size_t i = 0; i < uri_path.size(); i++){
		std::string	path_name	= uri_path[i].to_string();
		// if this path_name starts with the ESC_STRING, return false
		if( path_name.substr(0, ESC_STRING.size()) == ESC_STRING )	return false;
	}

	// if any variable key or val contain ESC_STRING return false
	for(size_t i = 0; i < outter_uri.var().size(); i++){
		const std::string & key	= outter_uri.var()[i].key();
		const std::string & val	= outter_uri.var()[i].val();
		if(string_t::find_substr(key, ESC_STRING) != std::string::npos)	return false;
		if(string_t::find_substr(val, ESC_STRING) != std::string::npos)	return false;
	}	

	// if the outter_uri has anchor, return false
	if( !outter_uri.anchor().empty() )	return false;
	// if all the previous tests passed, return true
	return true;
}

/** \brief Return true if the http_uri_t is a valid nested uri
 */
bool	http_nested_uri_t::is_valid_nested(const http_uri_t &nested_uri)	throw()
{
	// if the inner http_uri_t can't be parsed, return false
	if( parse_inner(nested_uri).is_null() )		return false;
	// if the outter http_uri_t can't be parsed, return false
	if( parse_outter(nested_uri).is_null() )	return false;
	// if all the previous tests passed, it is considered a valid nested http_uri_t
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                build a nested uri
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a nested http_uri_t from a outter and inner http_uri_t
 */
http_uri_t	http_nested_uri_t::build(const http_uri_t &outter_uri, const http_uri_t &inner_uri) throw()
{
	std::ostringstream 	nested_oss;
	KLOG_ERR("enter outter_uri=" << outter_uri << " inner_uri=" << inner_uri);
	// sanity check - the outter_uri MUST be a valid one
	DBG_ASSERT(  is_valid_outter(outter_uri) );
	DBG_ASSERT( !outter_uri.is_null() && !inner_uri.is_null() );
	// build the string for the nested uri
	nested_oss << "http://";
	// put the outter_uri.hostport_str
	nested_oss << outter_uri.hostport_str();
	nested_oss << outter_uri.path();
	// put the outter_uri.var()
	for(size_t i = 0; i < outter_uri.var().size(); i++){
		std::string	key	= outter_uri.var()[i].key();
		std::string	val	= outter_uri.var()[i].val();
		// perform a double escape to counter the unescape of the http_uri_t(str)
		key	= string_t::escape_not_in(key, http_uri_t::UNRESERVED_CHARSET);
		val	= string_t::escape_not_in(val, http_uri_t::UNRESERVED_CHARSET);
		// add this variable as a path member
		nested_oss << "/";
		nested_oss << ESC_STRING << string_t::escape_not_in(key, http_uri_t::UNRESERVED_CHARSET);
		nested_oss << ESC_STRING << string_t::escape_not_in(val, http_uri_t::UNRESERVED_CHARSET);
	}
	nested_oss << "/";
	nested_oss << "http:";
	nested_oss << "/";
	nested_oss << inner_uri.hostport_str();	
	nested_oss << "/";
	nested_oss << inner_uri.pathquery_str();
	// log to debug
	KLOG_DBG("nested_oss=" << nested_oss.str());
	// return the just built string as a http_uri_t
	return http_uri_t(nested_oss.str());	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//          parse the inner and outter http_uri_t from a nested one
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse and return the outter http_uri_t from a nested http_uri_t
 * 
 * - return a null one if the parsing fails
 */
http_uri_t	http_nested_uri_t::parse_outter(const http_uri_t &nested_uri)	throw()
{
	/*************** strip the outter_uri from the nested_uri	*******/
	// convert the nested_uri to a string
	std::string	nested_uri_str	= nested_uri.to_string();
	// get the separator escaped as it is in the nested_uri path
	std::string	separator_esc	= "/http%3a/";
	// try to find the separator in the nested_uri_str
	size_t		separator_pos	= nested_uri_str.find(separator_esc);
	// if the separator has not been found, return a null uri
	if( separator_pos == std::string::npos )	return http_uri_t();
	// build the tmp_inner_uri
	http_uri_t tmp_inner_uri= nested_uri_str.substr(0, separator_pos);

	/*************** parse the outter_var	*******************************/
 	std::ostringstream 	outter_oss;
	size_t			i;
	// build the outter_uri string
	outter_oss << "http://";
	outter_oss << nested_uri.hostport_str();
	// log to debug
	KLOG_DBG("outter_oss=" << outter_oss.str());
	// copy all the elements of the nested_uri path until 'http' is found
	const file_path_t & uri_path	= tmp_inner_uri.path();
	for(i = 0; i < uri_path.size(); i++){
		std::string path_name	= uri_path[i].to_string();
		// add the directory separator
		outter_oss << "/";
		// if the path_name starts with ESC_STRING, leave the loop
		if( path_name.substr(0, ESC_STRING.size()) == ESC_STRING )	break;
		// add the name into the outter_uri string
		outter_oss << uri_path[i];
	}
	// log to debug
	KLOG_DBG("outter_oss=" << outter_oss.str());
	// parse the variable
	for(size_t j = 0; i < uri_path.size(); i++, j++){
		std::string path_name	= uri_path[i].to_string();
		// if the path_name starts with ESC_STRING, return a null http_uri_t
		if( path_name.substr(0, ESC_STRING.size()) != ESC_STRING )	return http_uri_t();
		// add the variable separator
		if( j == 0 )	outter_oss << "?";
		else		outter_oss << "&";
		// parse the variable
		DBG_ASSERT( ESC_STRING.size() == 1 );
		std::vector<std::string> words	= string_t::split(path_name, ESC_STRING);
		if( words.size() != 3 )						return http_uri_t();
		// TODO some escaping are missing - not sure as it is an oss
		outter_oss	<< words[1] << "=" << words[2];
	}
	// log to debug
	KLOG_DBG("outter_oss=" << outter_oss.str());
	// sanity check - the result MUST be a valid outter_uri
	DBG_ASSERT( is_valid_outter(outter_oss.str()) );
	// return the just built outter_uri
	return outter_oss.str();	
 }

/** \brief Parse and return the inner http_uri_t from a nested http_uri_t
 * 
 * - return a null one if the parsing fails
 */
http_uri_t	http_nested_uri_t::parse_inner(const http_uri_t &nested_uri)	throw()
{
	// convert the nested_uri to a string
	std::string	nested_uri_str	= nested_uri.to_string();
	// get the separator escaped as it is in the nested_uri path
	std::string	separator_esc	= "/http%3a/";
	// try to find the separator in the nested_uri_str
	size_t		separator_pos	= nested_uri_str.find(separator_esc);
	// if the separator has not been found, return a null uri
	if( separator_pos == std::string::npos )	return http_uri_t();
	// build the inner_uri_str
	std::string	inner_uri_str	= "http://" + nested_uri_str.substr(separator_pos + separator_esc.size());
	inner_uri_str	= string_t::unescape(inner_uri_str);
	// return the inner_uri	
	return http_uri_t(inner_uri_str);
}


NEOIP_NAMESPACE_END


