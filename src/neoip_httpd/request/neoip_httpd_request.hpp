/*! \file
    \brief Header of the \ref httpd_t class
    
*/


#ifndef __NEOIP_HTTPD_REQUEST_HPP__ 
#define __NEOIP_HTTPD_REQUEST_HPP__ 
/* system include */
#include <sstream>
#include <string>
/* local include */
#include "neoip_property.hpp"
#include "neoip_html_builder.hpp"
#include "neoip_slotpool_id.hpp"
#include "neoip_datum.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	tcp_full_t;

/** \brief Context for a httpd_t request - used to exchange data between the httpd_t and the handler
 */
class httpd_request_t : NEOIP_COPY_CTOR_ALLOW {
private:
	/*************** data from the request	*******************************/
	std::string		req_method;	//!< the http method
	std::string		uri_path;	//!< the path of the uri
	property_t		uri_variable;	//!< the variable contained in the uri
	property_t		req_header;	//!< all the http header
	slot_id_t		slot_id;	//!< the http connection slot_id - used for delayed reply
	tcp_full_t *		tcp_full;	//!< the tcp_full from which this request comes from
						//!< TODO what is this pointer !??!?! slot_id should be enought
	datum_t			posted_data;	//!< contains the posted data IIF req_method = "POST"

	/*************** data for the reply	*******************************/
	std::ostringstream	reply;		//!< the reply from the handler
	std::string		reply_mimetype;	//!< the mimetype of the reply (if .empty() text/html
						//!< is assumed)

	void copy(const httpd_request_t &other)	throw();
	
public:
	/*************** ctor/dtor	***************************************/
	httpd_request_t(const std::string &req_method, const std::string &uri_path
			, const property_t &uri_variable, const property_t &req_header
			, const datum_t	&posted_data, slot_id_t slot_id
			, tcp_full_t *tcp_full)	throw();
	httpd_request_t()	throw()	{}

	/*************** copy stuff	***************************************/
	httpd_request_t(const httpd_request_t &other)			throw();
	httpd_request_t &operator = (const httpd_request_t & other)	throw();

	//! return true if the object is null, false otherwise
	bool	is_null()			const throw();

	//! return the method from the url
	const std::string &	get_method()		const throw()	{ return req_method;	}
	std::string &		get_method()		throw() 	{ return req_method;	}
	//! return the path from the url
	const std::string &	get_path()		const throw()	{ return uri_path;	}
	std::string &		get_path()		throw()		{ return uri_path;	}
	//! return a variable associated with the request
	const std::string &	get_variable(const std::string &name, const std::string &default_val="")
				const throw() { return uri_variable.find_string(name, default_val);	}
	const property_t &	get_variable_property()	const throw()	{ return uri_variable;	}
	//! return a header associated with the request
	const std::string &	get_header(const std::string &name, const std::string &default_val="")
				const throw() { return req_header.find_string(name, default_val);	}
	const property_t &	get_header_property()	const throw()	{ return req_header;	}
	//! return the posted_datapath from the url
	const datum_t	&	get_posted_data()	const throw()	{ return posted_data;	}
	datum_t	&		get_posted_data()	throw()		{ return posted_data;	}
	//! return the slot_id of the httpd connection
	slot_id_t		get_slot_id()		const throw() {	return slot_id;		}	
	//! return the tcp_full of the httpd connection
	const tcp_full_t *	get_tcp_full()		const throw() {	return tcp_full;	}
	/** \brief return a reference on the stream which contains the reply of this request
	 * 
	 * - it is to be used to build the reply in the handler and it is used 
	 *   in \ref neoip_httpd_handler to get the built reply.
	 */
	std::ostringstream & 		get_reply()	throw()		{ return reply;	}
	const std::ostringstream &	get_reply()	const throw()	{ return reply;	}
	//! return the reply_mimetype from the url
	const std::string &	get_reply_mimetype()	const throw()	{ return reply_mimetype;	}
	std::string &		get_reply_mimetype()	throw()		{ return reply_mimetype;	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTPD_REQUEST_HPP__  */



