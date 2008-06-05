/*! \file
    \brief Declaration of the xmlrpc_client_t
    
*/


#ifndef __NEOIP_XMLRPC_CLIENT_CB_HPP__ 
#define __NEOIP_XMLRPC_CLIENT_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	xmlrpc_client_t;
class	xmlrpc_err_t;
class	datum_t;

/** \brief the callback class for xmlrpc_client_t
 */
class xmlrpc_client_cb_t {
public:
	/** \brief callback to notify result from xmlrpc_client_t
	 */
	virtual bool neoip_xmlrpc_client_cb(void *cb_userptr, xmlrpc_client_t &cb_xmlrpc_client
					, const xmlrpc_err_t &xmlrpc_err
					, const datum_t &xmlrpc_resp)	throw() = 0;
	// virtual destructor
	virtual ~xmlrpc_client_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_XMLRPC_CLIENT_CB_HPP__  */



