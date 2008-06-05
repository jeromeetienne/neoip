/*! \file
    \brief Declaration of the bt_io_write_t
    
*/


#ifndef __NEOIP_BT_IO_WRITE_HPP__ 
#define __NEOIP_BT_IO_WRITE_HPP__ 

/* system include */
/* local include */
#include "neoip_file_size.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief the callback class for bt_io_write_t
 */
class bt_io_write_t {
public:
	//! return the written_len for this bt_io_write_t
	virtual const file_size_t &	written_len()	const throw() = 0;

	// virtual destructor
	virtual ~bt_io_write_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_WRITE_HPP__  */



