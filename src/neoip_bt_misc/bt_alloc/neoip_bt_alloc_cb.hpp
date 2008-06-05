/*! \file
    \brief Declaration of the bt_alloc_t
    
*/


#ifndef __NEOIP_BT_ALLOC_CB_HPP__ 
#define __NEOIP_BT_ALLOC_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	bt_alloc_t;
class	bt_err_t;

/** \brief the callback class for bt_alloc_t
 */
class bt_alloc_cb_t {
public:
	/** \brief callback notified by \ref bt_alloc_t 
	 */
	virtual bool neoip_bt_alloc_cb(void *cb_userptr, bt_alloc_t &cb_bt_alloc
							, const bt_err_t &bt_err)	throw() = 0;
	// virtual destructor
	virtual ~bt_alloc_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_ALLOC_CB_HPP__  */



