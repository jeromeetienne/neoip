/*! \file
    \brief Definition of the \ref http_sclient_res_t

- TODO rename full_get_ok as get_full_ok()
- TODO rename is_part_get_ok as get_part_ok()
- those function test the http replied

*/


/* system include */
#include <iostream>
/* local include */
#include "neoip_http_sclient_res.hpp"
#include "neoip_file_range.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return true if the http_reqhd_t was a 'whole' get and the replies matches it, false otherwise
 *   
 */
bool	http_sclient_res_t::full_get_ok()					const throw()
{
	// if an error occured at the connection level, return false
	if( cnx_err().failed() )					return false;
	// if the http_reqhd_t method IS NOT get, return false
	if( http_reqhd().method() != http_method_t::GET )		return false;
	// if the http_reqhd_t DOES contains a range, return false
	if( !http_reqhd().range().is_null() )				return false;
	// if the http_rephd_t status_code IS NOT 200 - OK, return false
	if( http_rephd().status_code() != 200 )				return false;
	// return true - here the http_sclient_res_t is considered a valid full get exchange
	return true;
}

/** \brief Return true if the http_reqhd_t was a 'partial' GET and the replies matches it, false otherwise
 */
bool	http_sclient_res_t::part_get_ok()					const throw()
{
	// if an error occured at the connection level, return false
	if( cnx_err().failed() )					return false;
	// if the http_reqhd_t method IS NOT get, return false
	if( http_reqhd().method() != http_method_t::GET )		return false;
	// get the request_range
	file_range_t	request_range	= http_reqhd().range();
	// if the http_reqhd_t doesnt contains a range, return false
	if( request_range.is_null() )					return false;
	// if the http_rephd_t status_code IS NOT 206 - partial data, return false
	if( http_rephd().status_code() != 206 )				return false;
	// if the reply_body IS NOT as long as the requested range, return false;
	if( request_range.length() != reply_body().size() )		return false;
	// return true - here the http_sclient_res_t is considered a valid parial get exchange
	return true;
}

/** \brief Return true if the http_reqhd_t was a 'whole' head and the replies 
 * 	   matches it, false otherwise
 */
bool	http_sclient_res_t::full_head_ok()					const throw()
{
	// if an error occured at the connection level, return false
	if( cnx_err().failed() )					return false;
	// if the http_reqhd_t method IS NOT get, return false
	if( http_reqhd().method() != http_method_t::HEAD )		return false;
	// if the http_reqhd_t DOES contains a range, return false
	if( !http_reqhd().range().is_null() )				return false;
	// if the http_rephd_t status_code IS NOT 200 - OK, return false
	if( http_rephd().status_code() != 200 )				return false;
	// return true - here the http_sclient_res_t is considered a valid full get exchange
	return true;
}
/** \brief Return true if the http_reqhd_t was a 'whole' post and the replies matches it, false otherwise
 *   
 */
bool	http_sclient_res_t::is_post_ok()				const throw()
{
	// if an error occured at the connection level, return false
	if( cnx_err().failed() )					return false;
	// if the http_reqhd_t method IS NOT post, return false
	if( http_reqhd().method() != http_method_t::POST )		return false;
	// if the http_rephd_t status_code IS NOT 200 - OK, return false
	if( http_rephd().status_code() != 200 )				return false;
	// return true - here the http_sclient_res_t is considered a valid full get exchange
	return true;
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        display function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	http_sclient_res_t::to_string()				const throw()
{
	std::ostringstream	oss;
	if( is_null() )	return "null";
	// build the string to return
	oss << "http_reqhd="		<< http_reqhd();
	oss << " cnx_err="		<< cnx_err();
	oss << " http_reqhd="		<< http_rephd();
	oss << " reply_body.size()="	<< reply_body().size();
	// return the just built string
	return oss.str();
}
NEOIP_NAMESPACE_END

