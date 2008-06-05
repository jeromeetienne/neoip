/*! \file
    \brief Definition of the \ref httpd_request_t class

\par Implementation Notes
- a explicit copy constructor and assignement operator are needed ONLY because
  std::ostringstream doesnt provide it.
  - i dont understand why tho...

*/

/* system include */
/* local include */
#include "neoip_httpd_request.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor with value
 */
httpd_request_t::httpd_request_t(const std::string &req_method, const std::string &uri_path
			, const property_t &uri_variable, const property_t &req_header
			, const datum_t &posted_data, slot_id_t slot_id
			, tcp_full_t *tcp_full)	throw()
{
	// copy the parameter
	this->req_method	= req_method;
	this->uri_path		= uri_path;
	this->uri_variable	= uri_variable;
	this->req_header	= req_header;
	this->posted_data	= posted_data;
	this->slot_id		= slot_id;
	this->tcp_full		= tcp_full;
	
	// set the default reply_mimetype - it may be overwritten by the handler
	this->reply_mimetype	= "text/html";
}

/** \brief copy constructor
 */
httpd_request_t::httpd_request_t(const httpd_request_t &other)	throw()
{
	copy(other);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                        Canonical object management
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Copy the other object into the local one	
 */
void httpd_request_t::copy(const httpd_request_t &other)	throw()
{
	req_method	= other.req_method;
	uri_path	= other.uri_path;
	uri_variable	= other.uri_variable;
	req_header	= other.req_header;
	posted_data	= other.posted_data;
	slot_id		= other.slot_id;
	tcp_full	= other.tcp_full;
	
	reply		<< other.reply.str();
	reply_mimetype	= other.reply_mimetype;
}	


/** \brief assignement operator
 */
httpd_request_t &	httpd_request_t::operator = (const httpd_request_t & other)	throw()
{
	if( this == &other )	return *this;
	copy(other);
	return *this;
}

/** \brief return true if the object is null, false otherwise
 */
bool	httpd_request_t::is_null()	const throw()
{
	return req_method.empty();
}

NEOIP_NAMESPACE_END



