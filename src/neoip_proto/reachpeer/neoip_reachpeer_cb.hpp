/*! \file
    \brief Header of the \ref reachpeer_t 's callback
*/


#ifndef __NEOIP_REACHPEER_CB_HPP__ 
#define __NEOIP_REACHPEER_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	reachpeer_t;
class	reachpeer_event_t;

/** \brief the callback class to notify event from \ref reachpeer_t
 */
class reachpeer_cb_t {
public:
	virtual bool neoip_reachpeer_event_cb(void *cb_userptr, reachpeer_t &cb_reachpeer
						, const reachpeer_event_t &reachpeer_event )
						throw() = 0;
	virtual ~reachpeer_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_REACHPEER_CB_HPP__  */



