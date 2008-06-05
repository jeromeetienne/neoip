/*! \file
    \brief Declaration of the btrelay_itor_t
    
*/


#ifndef __NEOIP_BTRELAY_ITOR_CB_HPP__ 
#define __NEOIP_BTRELAY_ITOR_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	btrelay_itor_t;
class	bt_err_t;
class	bt_mfile_t;
class	bt_cast_mdata_t;

/** \brief the callback class for btrelay_itor_t
 */
class btrelay_itor_cb_t {
public:
	/** \brief callback notified by \ref btrelay_itor_t when it is completed
	 * 
	 * @return true if the btrelay_itor_t is still valid after the callback
	 */
	virtual bool neoip_btrelay_itor_cb(void *cb_userptr, btrelay_itor_t &cb_btrelay_itor
					, const bt_err_t &bt_err, const bt_mfile_t &bt_mfile
					, const bt_cast_mdata_t &cast_mdata)	throw() = 0;
	// virtual destructor
	virtual ~btrelay_itor_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BTRELAY_ITOR_CB_HPP__  */



