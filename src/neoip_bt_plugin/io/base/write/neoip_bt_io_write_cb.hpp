/*! \file
    \brief Declaration of the bt_io_write_t
    
*/


#ifndef __NEOIP_BT_IO_WRITE_CB_HPP__ 
#define __NEOIP_BT_IO_WRITE_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	bt_io_write_t;
class	bt_err_t;

/** \brief the callback class for bt_io_write_t
 */
class bt_io_write_cb_t {
public:
	/** \brief callback notified by \ref bt_io_write_t when to notify the result of the operation
	 */
	virtual bool neoip_bt_io_write_cb(void *cb_userptr, bt_io_write_t &cb_bt_io_write
						, const bt_err_t &bt_err)	throw() = 0;
	// virtual destructor
	virtual ~bt_io_write_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_WRITE_CB_HPP__  */



