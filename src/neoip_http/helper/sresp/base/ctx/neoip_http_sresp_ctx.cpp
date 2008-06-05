/*! \file
    \brief Definition of the \ref http_sresp_ctx_t
  
*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_http_sresp_ctx.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   CTOR/DTOR
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/* \brief Default contructor
 */
http_sresp_ctx_t::http_sresp_ctx_t()					throw()
{
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			copy operator 
// - required because std::ostringstream is not copyable... 
//   - unknown reason. stl weirdness
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Copy the other object into the local one	
 */
void http_sresp_ctx_t::copy(const http_sresp_ctx_t &other)	throw()
{
	// copy the normal fields
	reqhd		( other.reqhd() );
	rephd		( other.rephd() );	
	post_data	( other.post_data() );	
	// special case of response_body
	response_body_val << other.response_body().str();
}	

/** \brief copy constructor
 */
http_sresp_ctx_t::http_sresp_ctx_t(const http_sresp_ctx_t &other)	throw()
{
	copy(other);
}

/** \brief assignement operator
 */
http_sresp_ctx_t &	http_sresp_ctx_t::operator = (const http_sresp_ctx_t & other)	throw()
{
	if( this == &other )	return *this;
	copy(other);
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Setup the http_resp_ctx_t to reply an http error
 * 
 * - Helper function to be called inside the http_sresp_cb_t
 */
void	http_sresp_ctx_t::reply_error(size_t status_code, const std::string &reason_phrase)	throw()
{
	// just copy the data in the http_rephd_t
	rephd().status_code(status_code).reason_phrase(reason_phrase);
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
int http_sresp_ctx_t::compare(const http_sresp_ctx_t & other)  const throw()
{
	// handle the case where at least one is null
	if(  is_null() && !other.is_null() )	return -1;
	if( !is_null() &&  other.is_null() )	return +1;
	if(  is_null() &&  other.is_null() )	return  0;
	// NOTE: here both are NOT null
	
	// compare the reqhd
	if( reqhd()	< other.reqhd() )	return -1;
	if( reqhd()	> other.reqhd() )	return +1;

	// compare the rephd
	if( rephd()	< other.rephd() )	return -1;
	if( rephd()	> other.rephd() )	return +1;

	// compare the post_data
	if( post_data()	< other.post_data() )	return -1;
	if( post_data()	> other.post_data() )	return +1;

	// compare the response_body
	if( response_body().str()	< other.response_body().str() )	return -1;
	if( response_body().str()	> other.response_body().str() )	return +1;
	
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
std::string http_sresp_ctx_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// build the string
	oss << "[";
	oss << "reqhd="			<< reqhd();
	oss << " rephd="		<< rephd();
	oss << " post_data="		<< post_data();
	oss << " response_body="	<< response_body().str();
	oss << "]";
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END;






