/*! \file
    \brief Declaration of the rate_limit_t
    
*/


#ifndef __NEOIP_RATE_LIMIT_CB_HPP__ 
#define __NEOIP_RATE_LIMIT_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class rate_limit_t;

/** \brief the callback class for rate_limit_t
 */
class rate_limit_cb_t {
public:
	/** \brief callback notified by \ref rate_limit_t becomes unblocked
	 */
	virtual bool neoip_rate_limit_cb(void *cb_userptr, rate_limit_t &cb_rate_limit)	throw() = 0;
	// virtual destructor
	virtual ~rate_limit_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RATE_LIMIT_CB_HPP__  */



