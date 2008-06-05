/*! \file
    \brief Declaration of the casto_itor_t
    
*/


#ifndef __NEOIP_CASTO_ITOR_CB_HPP__ 
#define __NEOIP_CASTO_ITOR_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	casto_itor_t;
class	bt_err_t;
class	bt_cast_mdata_t;

/** \brief the callback class for casto_itor_t
 */
class casto_itor_cb_t {
public:
	/** \brief callback notified by \ref casto_itor_t when it is completed
	 * 
	 * @return true if the casto_itor_t is still valid after the callback
	 */
	virtual bool neoip_casto_itor_cb(void *cb_userptr, casto_itor_t &cb_casto_itor
					, const bt_err_t &bt_err
					, const bt_cast_mdata_t &cast_mdata)	throw() = 0;
	// virtual destructor
	virtual ~casto_itor_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CASTO_ITOR_CB_HPP__  */



