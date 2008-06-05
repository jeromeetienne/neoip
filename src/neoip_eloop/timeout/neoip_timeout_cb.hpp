/*! \file
    \brief Header of the neoip_timeout
    
*/


#ifndef __NEOIP_TIMEOUT_CB_HPP__ 
#define __NEOIP_TIMEOUT_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class timeout_t;

/** \brief the callback class for \ref timeout_t
 */
class timeout_cb_t {
public:
	/** \brief callback called when the timeout_t expire
	 * 
	 * @param userptr	the userptr associated with the callback
	 * @return true to relaunch the timer, false to stop it
	 */
	virtual bool neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout) throw() = 0;
	virtual ~timeout_cb_t() {};
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TIMEOUT_CB_HPP__  */



