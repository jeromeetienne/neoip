/*! \file
    \brief Declaration of the bt_io_read_t
    
*/


#ifndef __NEOIP_BT_IO_READ_HPP__ 
#define __NEOIP_BT_IO_READ_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief the callback class for bt_io_read_t
 */
class bt_io_read_t {
public:
	// virtual destructor
	virtual ~bt_io_read_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_READ_HPP__  */



