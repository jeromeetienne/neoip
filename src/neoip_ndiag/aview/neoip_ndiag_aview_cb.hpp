/*! \file
    \brief Declaration of the ndiag_aview_t callback
    
*/


#ifndef __NEOIP_NDIAG_AVIEW_CB_HPP__ 
#define __NEOIP_NDIAG_AVIEW_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	ndiag_aview_t;
class	ipport_addr_t;

/** \brief the callback class for ndiag_aview_t
 */
class ndiag_aview_cb_t {
public:
	/** \brief callback notified by \ref ndiag_aview_t on completion
	 */
	virtual bool neoip_ndiag_aview_cb(void *cb_userptr, ndiag_aview_t &cb_ndiag_aview
					, const ipport_addr_t &new_ipport_pview)	throw() = 0;
	virtual ~ndiag_aview_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NDIAG_AVIEW_CB_HPP__  */



