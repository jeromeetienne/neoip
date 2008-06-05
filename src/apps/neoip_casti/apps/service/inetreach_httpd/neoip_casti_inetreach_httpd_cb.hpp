/*! \file
    \brief Declaration of the casti_inetreach_httpd_t
    
*/


#ifndef __NEOIP_CASTI_INETREACH_HTTPD_CB_HPP__ 
#define __NEOIP_CASTI_INETREACH_HTTPD_CB_HPP__ 

/* system include */
#include <string>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	casti_inetreach_httpd_t;
class	ipport_addr_t;

/** \brief the callback class for casti_inetreach_httpd_t
 */
class casti_inetreach_httpd_cb_t {
public:
	/** \brief callback notified by \ref casti_inetreach_httpd_t
	 */
	virtual bool neoip_casti_inetreach_httpd_cb(void *cb_userptr
				, casti_inetreach_httpd_t &cb_inetreach_httpd
				, const ipport_addr_t &new_ipport_pview)	throw() = 0;
	// virtual destructor
	virtual ~casti_inetreach_httpd_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CASTI_INETREACH_HTTPD_CB_HPP__  */



