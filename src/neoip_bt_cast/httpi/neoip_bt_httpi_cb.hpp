/*! \file
    \brief Declaration of the bt_httpi_t
    
*/


#ifndef __NEOIP_BT_HTTPI_CB_HPP__ 
#define __NEOIP_BT_HTTPI_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	bt_httpi_t;
class	bt_httpi_event_t;

/** \brief the callback class for bt_httpi_t
 */
class bt_httpi_cb_t {
public:
	/** \brief callback notified by \ref bt_httpi_t 
	 */
	virtual bool neoip_bt_httpi_cb(void *cb_userptr, bt_httpi_t &cb_bt_httpi
				, const bt_httpi_event_t &httpi_event)	throw() = 0;
	// virtual destructor
	virtual ~bt_httpi_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HTTPI_CB_HPP__  */



