/*! \file
    \brief Declaration of the oload_itor_t
    
*/


#ifndef __NEOIP_OLOAD_ITOR_CB_HPP__ 
#define __NEOIP_OLOAD_ITOR_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	oload_itor_t;
class	bt_err_t;
class	bt_mfile_t;

/** \brief the callback class for oload_itor_t
 */
class oload_itor_cb_t {
public:
	/** \brief callback notified by \ref oload_itor_t when it is completed
	 * 
	 * @return true if the oload_itor_t is still valid after the callback
	 */
	virtual bool neoip_oload_itor_cb(void *cb_userptr, oload_itor_t &cb_oload_itor
					, const bt_err_t &bt_err
					, const bt_mfile_t &bt_mfile)	throw() = 0;
	// virtual destructor
	virtual ~oload_itor_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_OLOAD_ITOR_CB_HPP__  */



