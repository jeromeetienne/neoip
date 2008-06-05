/*! \file
    \brief Declaration of the bt_io_read_t
    
*/


#ifndef __NEOIP_BT_IO_READ_CB_HPP__ 
#define __NEOIP_BT_IO_READ_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	bt_io_read_t;
class	bt_err_t;
class	datum_t;

/** \brief the callback class for bt_io_read_t
 */
class bt_io_read_cb_t {
public:
	/** \brief callback notified by \ref bt_io_read_t when to notify the result of the operation
	 */
	virtual bool neoip_bt_io_read_cb(void *cb_userptr, bt_io_read_t &cb_bt_io_read
			, const bt_err_t &bt_err, const datum_t &read_data)	throw() = 0;
	// virtual destructor
	virtual ~bt_io_read_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_READ_CB_HPP__  */



