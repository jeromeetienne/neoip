/*! \file
    \brief Header of the \ref nlay_full_t

- see \ref neoip_nlay_full.cpp
*/


#ifndef __NEOIP_NLAY_FULL_CB_HPP__ 
#define __NEOIP_NLAY_FULL_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	nlay_full_t;
class	nlay_event_t;

/** \brief callback declaration for \ref nlay_full_t
 */
class nlay_full_cb_t {
public:	
	/** \brief Callback notified with \ref nlay_full_t report an event
	 * 
	 * @return true if the object MUST be kept, false otherwise
	 */
	virtual bool neoip_nlay_full_event_cb(void *cb_userptr, nlay_full_t &cb_nlay_full
					, const nlay_event_t &nlay_event)	throw() = 0;
	//! virtual destructor
	virtual ~nlay_full_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NLAY_FULL_CB_HPP__  */



