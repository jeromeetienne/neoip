/*! \file
    \brief Header of the \ref nlay_itor_t

- see \ref neoip_nlay_itor.cpp
*/


#ifndef __NEOIP_NLAY_ITOR_CB_HPP__ 
#define __NEOIP_NLAY_ITOR_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	nlay_itor_t;
class	nlay_event_t;

/** \brief callback declaration for \ref nlay_itor_t
 */
class nlay_itor_cb_t {
public:	
	/** \brief Callback notified with \ref nlay_itor_t report an event
	 * 
	 * @return true if the object MUST be kept, false otherwise
	 */
	virtual bool neoip_nlay_itor_event_cb(void *cb_userptr, nlay_itor_t &cb_nlay_itor
					, const nlay_event_t &nlay_event)	throw() = 0;
	//! virtual destructor
	virtual ~nlay_itor_cb_t() {};
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NLAY_ITOR_CB_HPP__  */



