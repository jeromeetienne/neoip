/*! \file
    \brief Declaration of the xmlrpc_resp_t
    
*/


#ifndef __NEOIP_XMLRPC_RESP_CB_HPP__ 
#define __NEOIP_XMLRPC_RESP_CB_HPP__ 

/* system include */
#include <string>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	xmlrpc_resp_t;
class	xmlrpc_err_t;
class	xmlrpc_parse_t;
class	xmlrpc_build_t;

/** \brief the callback class for xmlrpc_resp_t
 */
class xmlrpc_resp_cb_t {
public:
	/** \brief callback notified by \ref xmlrpc_resp_t when to notify an event
	 * 
	 * - WARNING: it is not allowed to destroy the xmlrpc_listener_t on which 
	 *   whic xmlrpc_resp_t is listening on *DURING* the callback.
	 * 
	 * @return true if the xmlrpc_resp_t is still valid after the callback
	 */
	virtual bool neoip_xmlrpc_resp_cb(void *cb_userptr, xmlrpc_resp_t &cb_xmlrpc_resp
			, const std::string &method_name, xmlrpc_err_t &err_out
			, xmlrpc_parse_t &xmlrpc_parse, xmlrpc_build_t &xmlrpc_build)	throw() = 0;
	// virtual destructor
	virtual ~xmlrpc_resp_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_XMLRPC_RESP_CB_HPP__  */



