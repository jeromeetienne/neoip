/*! \file
    \brief Header of the \ref simuwan_t callbacks

*/


#ifndef __NEOIP_SIMUWAN_CB_HPP__ 
#define __NEOIP_SIMUWAN_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	simuwan_t;
class	simuwan_event_t;

/** \brief the callback class for \ref simuwan_t
 */
class simuwan_cb_t {
public:
	/** \brief callback notified when \ref simuwan_t send an event.
	 * 
	 * @return true if the simuwan_t is still valid after the callback, false otherwise
	 */
	virtual bool neoip_simuwan_event_cb(void *cb_userptr, simuwan_t &cb_simuwan
						, const simuwan_event_t &simuwan_event )	throw() = 0;
	virtual ~simuwan_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SIMUWAN_CB_HPP__  */



