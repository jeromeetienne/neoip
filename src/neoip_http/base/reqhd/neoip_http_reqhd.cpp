/*! \file
    \brief Definition of the \ref http_reqhd_t
  
*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_http_reqhd.hpp"
#include "neoip_file_range.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   CTOR/DTOR
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/* \brief Default contructor
 */
http_reqhd_t::http_reqhd_t()					throw()
{
	// set some default values
	method("GET");
	version("HTTP/1.1");
	// set the header_db into key_ignorecase - it is more robust to buggy http implementation
	// - this is to workaround bugs in upnp http server
	header_db().key_ignorecase(true);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if this header_key is present in this http_reqhd_t
 */
bool	http_reqhd_t::has_header(const std::string &header_key)	const throw()
{
	return header_db().contain_key(header_key);
}

/** \brief Return the value of this header_key in this http_reqhd_t
 * 
 * - NOTE: the header_key MUST be present
 */
const std::string &	http_reqhd_t::get_header_value(const std::string &header_key)	const throw()
{
	// sanity check - the header_key MUST be present
	DBG_ASSERT( has_header(header_key) );
	// get the key_idx in this header_key
	size_t	key_idx	= header_db().first_key_idx(header_key);
	DBG_ASSERT( key_idx != strvar_db_t::INDEX_NONE );
	// return the header_value
	return header_db()[key_idx].val();;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    header helper
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a file_range_t matching the "Range" header - rfc2616.14.35
 * 
 * - if Range header is not present or if there is an error in the parsing, return a null object
 */
file_range_t	http_reqhd_t::range()						const throw()
{
	// if the variable is not in the header, return a null object
	if( !header_db().contain_key("Range") )	return file_range_t();
	// get the value of the variable
	const std::string &	value_str	= header_db().get_first_value("Range");
	// get the units from the value_str
	std::vector<std::string> unit_words	= string_t::split(value_str, "=", 2);
	// if the unit_words has not a size of 2, return a null object
	if( unit_words.size() != 2 )		return file_range_t();
	// if the first word is not "bytes", return a null object
	if( unit_words[0] != "bytes" )		return file_range_t();
	// get the first and last from the string
	std::vector<std::string> firstlast_words= string_t::split(unit_words[1], "-", 2);
	// if the unit_words has not a size of 2, return a null object
	if( firstlast_words.size() != 2 )	return file_range_t();
	// set the result beg to the first words
	file_range_t	result;
	result.beg	( atoi(firstlast_words[0].c_str()) );
	// if this is a set of range, return null object
	if( firstlast_words[1].find(',') != std::string::npos )	return file_range_t();
	// if the last is implicit, set the end to file_size_t::MAX
	if(firstlast_words[1].empty() )		result.end	( file_size_t::MAX 			);
	else					result.end	( atoi(firstlast_words[1].c_str())	);
	// return the resulting file_range_t
	return result;
}

/** \brief Set the request range from a file_range_t - rfc2616.14.35
 */
http_reqhd_t &	http_reqhd_t::range(const file_range_t &file_range)		throw()
{
	std::ostringstream	oss;
	// if file_range_t is null, remove the value
	if( file_range.is_null() ){
		// Remove the Range variable in the header - if any
		size_t	var_idx	= header_db().first_key_idx("Range");
		if( var_idx != strvar_db_t::INDEX_NONE )	header_db().remove(var_idx);
		// return the object itself
		return *this;
	}
	// build the value of the Range variable
	oss << "bytes=";
	oss << file_range.beg();
	oss << "-";
	if( file_range.end() != file_size_t::MAX )	oss << file_range.end();
	// set the variable in the header
	header_db().update("Range", oss.str());
	// return the object itself
	return *this;
}


/** \brief Return a file_size_t matching the "Content-Length" header - rfc2616.14.13
 * 
 * - if Content-Length is not present or if there is an error in the parsing, return a null object
 */
file_size_t	http_reqhd_t::content_length()					const throw()
{
	file_size_t	result;
	// if the variable is not in the header, return a null object
	if( !header_db().contain_key("Content-Length") )	return file_size_t();
	// get the value of the variable
	const std::string & value_str	= header_db().get_first_value("Content-Length");	
	// parse the value
	result	= atoi(value_str.c_str());
	// return the resulting file_range_t
	return result;
}

/** \brief Set the Content-Length header from a file_size_t - rfc2616.14.13
 */
http_reqhd_t &	http_reqhd_t::content_length(const file_size_t &file_size)	throw()
{
	std::ostringstream	oss;
	// sanity check - the file_size_t MUST NOT be null
	DBG_ASSERT( !file_size.is_null() );
	// build the value of the variable
	oss << file_size;
	// set the variable in the header
	header_db().update("Content-Length", oss.str());
	// return the object itself
	return *this;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 convertion function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief build a http_reqhd_t from a std::string containing the http request header
 * 
 * - build a null object if the parsing fails.
 */
http_reqhd_t	http_reqhd_t::from_http(const std::string &orig_reqhd_str)		throw()
{
	std::string	reqhd_str(orig_reqhd_str);
	// log to debug
	KLOG_DBG("enter reqhd_str=" << reqhd_str);

	// get the req_line - see rfc2616.5.1
	// - NOTE: parsed after the message header to get the "Host:" for the uri
	std::string	req_line	= consume_line(reqhd_str);

	// parse all the remaining line as message header - see rfc2616.4.2
	strvar_db_t	header_db;
	while( reqhd_str.empty() == false ){
		// consume one header_line
		std::string		header_line	= consume_line(reqhd_str);
		// if it is a blank line, this is the last line of the header
		if( header_line.empty() && reqhd_str.empty() )	break;
		// parse the header line
		std::vector<std::string> header_words	= string_t::split(header_line, ":", 2);
		// if the number of words is != 2, this is a bogus input and return a null object
		if( header_words.size() != 2 )	return http_reqhd_t();
		// strip any "any leading or trailing LWS: linear white space" 
		header_words[1]	= string_t::strip(header_words[1]);
		// insert it in the header_db
		header_db.append(header_words[0], header_words[1]);
	}
	// set the header_db into key_ignorecase - it is more robust to buggy http implementation
	// - this is to workaround bugs in upnp http server
	header_db.key_ignorecase(true);

	
	// parse the req_line - done later to get the Host: message header if possible
	std::vector<std::string> req_words = string_t::split(req_line, " ");
	// if the number of words is != 3, this is a bogus input and return a null object
	if( req_words.size() != 3 )		return http_reqhd_t();
	// get the http_method_t
	http_method_t	method	= req_words[0].c_str();
	if( method == http_method_t::NONE )	return http_reqhd_t();
	// build the uri with the "Host:" message header if present, else a dummy host:port
	http_uri_t	uri;
	if( header_db.contain_key("Host") )	uri = "http://" + header_db.get_first_value("Host")+ req_words[1];
	else					uri = "http://0.0.0.0" + req_words[1];
	if( uri.is_null() )			return http_reqhd_t();
	// get the http_version_t
	http_version_t	version	= req_words[2].c_str();
	if( version == http_version_t::NONE )	return http_reqhd_t();
	
	// NOTE: if the parsing reached this point, all the fields are considered valid
	
	// build and return the resulting http_reqhd_t
	return http_reqhd_t().method(method).uri(uri).version(version).header_db(header_db);
}

/** \brief return the HTTP string for the request in this http_reqhd_t
 */
std::string	http_reqhd_t::to_http()		const throw()
{
	std::ostringstream	oss;
	// put the first line
	oss << method()			<< " ";
	oss << uri().pathquery_str()	<< " ";
	oss << version()		<< "\r\n";
	// add all header
	for(size_t i = 0; i < header_db().size(); i++)
		oss << header_db()[i].key() << ": " << header_db()[i].val() << "\r\n";
	// put the last \r\n which show the end of the http_reqhd_t
	oss << "\r\n";	
	// return the just built header
	return oss.str();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                            Utility functions
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief read a line from a http request header aka a normal line but ended with \r\n
 */
std::string http_reqhd_t::consume_line(std::string &base_str)	throw()
{
	// find the position of the end of line
	size_t		end_of_line	= base_str.find("\r\n");
	// extract the line to consume
	std::string	line		= base_str.substr(0, end_of_line);
	// consume the line
	if( end_of_line == std::string::npos )	base_str	= std::string();
	else					base_str.erase(0, end_of_line + 2 /* for \r\n */ );
	// return the consumed line
	return line;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main compare function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief compare 2 objects (ala memcmp) - as in rfc2616.3.2.3
 * 
 * - it return a value <  0 if the local object is less than the external one
 * - it return a value == 0 if the local object is equal to the external one
 * - it return a value >  0 if the local object is greater than the external one
 */
int http_reqhd_t::compare(const http_reqhd_t & other)  const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null
	
	// compare the method
	if( method()	< other.method() )	return -1;
	if( method()	> other.method() )	return +1;
	
	// compare the uri
	if( uri()	< other.uri() )		return -1;
	if( uri()	> other.uri() )		return +1;
	
	// compare the variable
	if( version()	< other.version() )	return -1;
	if( version()	> other.version() )	return +1;
	
	// compare the anchor
	if( header_db() < other.header_db() )	return -1;
	if( header_db() > other.header_db() )	return +1;
	
	// note: here both are considered equal
	return 0;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string http_reqhd_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string
	oss << "[";
	oss << "method="	<< method();
	oss << " uri="		<< uri();
	oss << " version="	<< version();
	oss << " header="	<< header_db();
	oss << "]";
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END;






