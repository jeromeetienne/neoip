/*! \file
    \brief Definition of the \ref http_rephd_t
    
*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_http_rephd.hpp"
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
http_rephd_t::http_rephd_t()					throw()
{
	// zero the status_code to ensure the default http_rephd_t is null
	status_code()	= 0;
	// set some default values
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

/** \brief Return true if this header_key is present in this http_rephd_t
 */
bool	http_rephd_t::has_header(const std::string &header_key)	const throw()
{
	return header_db().contain_key(header_key);
}

/** \brief Return the value of this header_key in this http_rephd_t
 * 
 * - NOTE: the header_key MUST be present
 */
const std::string &	http_rephd_t::get_header_value(const std::string &header_key)	const throw()
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

/** \brief Return a file_range_t and instance_len from the "Content-Range" header - rfc2616.14.16
 * 
 * - the "*" means undertermined, not infinite
 * - if the file_range_t is "*", return a null file_range_t
 * - if the instance_len is "*", return a null file_size_t
 * - NOTE: if there is an error in the parsing, file_range_t AND instance_len
 *   are BOTH null. 
 *   - it is ok as "* / * " is not allowed in http
 */
file_range_t	http_rephd_t::content_range(file_size_t *instance_len_out)	const throw()
{
	file_range_t	res_range;
	file_size_t	res_ilen;
	// reset the instance_len
	if( instance_len_out )	*instance_len_out	= file_size_t();

	// if the variable is not in the header, return a null object
	if( !header_db().contain_key("Content-Range") )	return file_range_t();
	// get the value of the variable
	const std::string &	value_str	= header_db().get_first_value("Content-Range");

	// get the units from the value_str
	std::vector<std::string> unit_words	= string_t::split(value_str, " ", 2);
	// if the unit_words has not a size of 2, return a null object
	if( unit_words.size() != 2 )		return file_range_t();
	// if the first word is not "bytes", return a null object
	if( unit_words[0] != "bytes" )		return file_range_t();

	// get the first and last from the string
	std::vector<std::string> rangeilen_words= string_t::split(unit_words[1], "/", 2);
	// if the unit_words has not a size of 2, return a null object
	if( rangeilen_words.size() != 2 )	return file_range_t();

	// get the instance len
	if( !rangeilen_words[1].empty() && rangeilen_words[1] != "*" )
		res_ilen	= atoi(rangeilen_words[1].c_str());

	// if the range is unspecified, return a file_range_t() BUT NOT an error is instance_len is set
	if( rangeilen_words[0] == "*" ){
		if( instance_len_out )	*instance_len_out	= res_ilen;
		return file_range_t();
	}

	// get the first and last from the string
	std::vector<std::string> firstlast_words= string_t::split(rangeilen_words[0], "-", 2);
	// if the unit_words has not a size of 2, return a null object
	if( firstlast_words.size() != 2 )	return file_range_t();
	// set the result beg to the first words
	res_range.beg	( atoi(firstlast_words[0].c_str()) );
	res_range.end	( atoi(firstlast_words[1].c_str()) );
	// return the resulting file_range_t
	if( instance_len_out )	*instance_len_out	= res_ilen;
	return res_range;
}

/** \brief Set the Content-Range from a file_range_t and instance_len - rfc2616.14.16
 */
http_rephd_t &	http_rephd_t::content_range(const file_range_t &file_range
					, const file_size_t &instance_len)	throw()
{
	std::ostringstream	oss;
	// sanity check - either file_range_t is not null or instance_len is not null
	// but they MUST NOT be null both at the same time. it is forbidden by http
	DBG_ASSERT( !file_range.is_null() || !instance_len.is_null() );
	// build the value of the variable
	oss << "bytes";
	oss << " ";
	if( !file_range.is_null() ){
		// sanity check - the file_range_t MUST NOT be open ended
		DBG_ASSERT( file_range.end() != file_size_t::MAX );
		
		oss << file_range.beg();
		oss << "-";
		oss << file_range.end();
	}else{
		oss << "*";
	}
	oss << "/";
	if( !instance_len.is_null() )	oss << instance_len;
	else				oss << "*";
	// set the variable in the header
	header_db().update("Content-Range", oss.str());
	// return the object itself
	return *this;	
}

/** \brief Return a file_size_t matching the "Content-Length" header - rfc2616.14.13
 * 
 * - if Content-Length is not present or if there is an error in the parsing, return a null object
 */
file_size_t	http_rephd_t::content_length()					const throw()
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
http_rephd_t &	http_rephd_t::content_length(const file_size_t &file_size)	throw()
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

/** \brief Return true if it is possible accept to send range request - rfc2616.14.5
 * 
 * - if Accept-Ranges is not present or if there is an error in the parsing, return false
 */
bool	http_rephd_t::accept_ranges()					const throw()
{
	file_size_t	result;
	// if the variable is not in the header, return false
	if( !header_db().contain_key("Accept-Ranges") )	return false;
	// get the value of the variable
	const std::string & value_str	= header_db().get_first_value("Accept-Ranges");	
	// if the value_str is "bytes", return true
	if( value_str == "bytes" )	return true;
	// else return false
	return false;
}

/** \brief Set the "Accept-Ranges" header - rfc2616.14.5
 */
http_rephd_t &	http_rephd_t::accept_ranges(bool doaccept)	throw()
{
	std::ostringstream	oss;
	// build the value of the variable
	if( doaccept )	oss << "bytes";
	else		oss << "none";
	// set the variable in the header
	header_db().update("Accept-Ranges", oss.str());
	// return the object itself
	return *this;
}

/** \brief Return a string matching the "Transfer-Encoding" header - rfc2616.3.6
 * 
 * - if Transfer-Encoding is not present, return an empty string
 */
std::string	http_rephd_t::transfer_encoding()				const throw()
{
	// if the variable is not in the header, return a null object
	if( !header_db().contain_key("Transfer-Encoding") )	return std::string();
	// get the value of the variable
	return header_db().get_first_value("Transfer-Encoding");
}

/** \brief Set the "Transfer-Encoding" header - rfc2616.3.6
 */
http_rephd_t &	http_rephd_t::transfer_encoding(const std::string &value)	throw()
{
	// set the variable in the header
	header_db().update("Transfer-Encoding", value);
	// return the object itself
	return *this;
}

/** \brief Return true if the http_rephd_t content a chunked "Transfer-Encoding"  - rfc2616.3.6.1
 */
bool	http_rephd_t::is_chunked_encoded()					const throw()
{
	return transfer_encoding() == "chunked";
}

/** \brief Return true if this connection is marked as 
 */
bool	http_rephd_t::is_connection_close()				const throw()
{
	// if http_version_t::V1_0, "Connection: close" is assumed
	// - TODO not sure about this one..
	if( version() == http_version_t::V1_0 )	return true; 
	// get the "Connection" field value - see rfc2616.14.10
	std::string	value	= header_db().get_first_value("Connection");
	// if there is no "Connection" value, assume that connection are persistent
	// - "HTTP/1.1 applications that do not support persistent connections MUST 
	//    include the "close" connection option in every message." - rfc2616.14.10
	if( value.empty() )			return false;
	// if the "Connection" value is "close", the connection is declared persistent
	if( value == "close" )			return true;
	// if this point is reached, the "Connection: close" is assumed
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 convertion function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief build a http_rephd_t from a std::string containing the http request header
 * 
 * - build a null object if the parsing fails.
 */
http_rephd_t	http_rephd_t::from_http(const std::string &orig_rephd_str)		throw()
{
	std::string	rephd_str(orig_rephd_str);
	// get the rep_line - see rfc2616.6.1
	std::string	rep_line	= consume_line(rephd_str);

	// parse the rep_line - done later to get the Host: message header if possible
	std::vector<std::string> rep_words = string_t::split(rep_line, " ", 3);
	// if the number of words is < 2, this is a bogus input and return a null object
	if( rep_words.size() < 2 )		return http_rephd_t();
	// get the http_version_t
	http_version_t	version		= rep_words[0].c_str();
#if 1	// NOTE: some shoutcast return a ICY 200 OK instead of a HTTP-1.0 200 OK
	// - this is a lame workaround
	if( version == http_version_t::NONE )	version	= http_version_t::V1_0;
#endif
	if( version == http_version_t::NONE )	return http_rephd_t();
	// get the status code
	size_t		status_code	= atoi(rep_words[1].c_str());
	// TODO to do a test to see if status_code is valid, else return 
	// build the reason_phase
	std::string	reason_phrase;
	if( rep_words.size() > 2 )	reason_phrase = rep_words[2];
	
	// parse all the remaining line as message header - see rfc2616.4.2
	strvar_db_t	header_db;
	while( rephd_str.empty() == false ){
		// consume one header_line
		std::string		header_line	= consume_line(rephd_str);
		// if it is a blank line, this is the last line of the header
		if( header_line.empty() && rephd_str.empty() )	break;
		// parse the header line
		std::vector<std::string> header_words	= string_t::split(header_line, ":", 2);
		// if the number of words is != 2, this is a bogus input and return a null object
		if( header_words.size() != 2 )	return http_rephd_t();
		// strip any "any leading or trailing LWS: linear white space" 
		header_words[1]	= string_t::strip(header_words[1]);
		// insert it in the header_db
		header_db.append(header_words[0], header_words[1]);
	}
	// set the header_db into key_ignorecase - it is more robust to buggy http implementation
	// - this is to workaround bugs in upnp http server
	header_db.key_ignorecase(true);
	

	// NOTE: if the parsing reached this point, all the fields are considered valid
	
	// build and return the resulting http_rephd_t
	return http_rephd_t().version(version).status_code(status_code).reason_phrase(reason_phrase)
							.header_db(header_db);
}

/** \brief return the HTTP string for the request in this http_rephd_t
 */
std::string	http_rephd_t::to_http()		const throw()
{
	std::ostringstream	oss;
	// put the first line
	oss << version()	<< " ";
	oss << status_code()	<< " ";
	oss << reason_phrase()	<< "\r\n";
	// add all header
	for(size_t i = 0; i < header_db().size(); i++)
		oss << header_db()[i].key() << ": " << header_db()[i].val() << "\r\n";
	// put the last \r\n which show the end of the http_rephd_t
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
std::string http_rephd_t::consume_line(std::string &base_str)	throw()
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
int http_rephd_t::compare(const http_rephd_t & other)  const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )		return -1;
	if( !is_null() &&  other.is_null() )		return +1;
	if(  is_null() &&  other.is_null() )		return  0;
	// NOTE: here both are NOT null
	
	// compare the version
	if( version()	< other.version() )		return -1;
	if( version()	> other.version() )		return +1;
	
	// compare the status_code
	if( status_code() < other.status_code() )	return -1;
	if( status_code() > other.status_code() )	return +1;
	
	// compare the reason_phrase
	if( reason_phrase() < other.reason_phrase() )	return -1;
	if( reason_phrase() > other.reason_phrase() )	return +1;
	
	// compare the anchor
	if( header_db() < other.header_db() )		return -1;
	if( header_db() > other.header_db() )		return +1;
	
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
std::string http_rephd_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string
	oss << "[";
	oss << "version="	<< version();
	oss << " status code="	<< status_code();
	oss << " reason phrase="<< reason_phrase();
	oss << " header="	<< header_db();
	oss << "]";
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END;






