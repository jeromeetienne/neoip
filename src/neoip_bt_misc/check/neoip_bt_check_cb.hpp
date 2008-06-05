/*! \file
    \brief Declaration of the bt_check_t
    
*/


#ifndef __NEOIP_BT_CHECK_CB_HPP__ 
#define __NEOIP_BT_CHECK_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	bt_check_t;
class	bt_err_t;
class	bt_swarm_resumedata_t;

/** \brief the callback class for bt_check_t
 */
class bt_check_cb_t {
public:
	/** \brief callback notified by \ref bt_check_t 
	 */
	virtual bool neoip_bt_check_cb(void *cb_userptr, bt_check_t &cb_bt_check, const bt_err_t &bt_err
				, const bt_swarm_resumedata_t &swarm_resumedata)	throw() = 0;
	// virtual destructor
	virtual ~bt_check_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_CHECK_CB_HPP__  */



