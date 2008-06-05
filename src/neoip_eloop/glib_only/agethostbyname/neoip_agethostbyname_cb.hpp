/*! \file
    \brief Header of the neoip_agethostbyname class
    
*/


#ifndef __NEOIP_AGETHOSTBYNAME_CB_HPP__ 
#define __NEOIP_AGETHOSTBYNAME_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"
#include "neoip_ip_addr.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class agethostbyname_t;
class hostent_t;
	
/** \brief the callback class for agethostbyname_t
 */
class agethostbyname_cb_t {
public:
	/** \brief callback notified by agethostbyname_t when the result is known
	 * 
	 * - this callback MAY delete the agethostbyname_t
	 * 
	 * @param cb_userptr		the userptr provided by the caller
	 * @param agethostbyname	the object created by the caller
	 * @param 			hostent a hostent of the result, may be null if an error occurs
	 */
	virtual void neoip_agethostbyname_cb(void *cb_userptr, agethostbyname_t &cb_agethostbyname
						, const hostent_t &hostent)	throw() = 0;
	virtual ~agethostbyname_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_AGETHOSTBYNAME_CB_HPP__  */



