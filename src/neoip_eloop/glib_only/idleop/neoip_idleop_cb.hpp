/*! \file
    \brief Declaration of the idleop_t
    
*/


#ifndef __NEOIP_IDLEOP_CB_HPP__ 
#define __NEOIP_IDLEOP_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	idleop_t;

/** \brief the callback class for idleop_t
 */
class idleop_cb_t {
public:
	/** \brief callback notified by \ref idleop_t when some time are available
	 */
	virtual bool neoip_idleop_cb(void *cb_userptr, idleop_t &cb_idleop)	throw() = 0;
	// virtual destructor
	virtual ~idleop_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_IDLEOP_CB_HPP__  */



