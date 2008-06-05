/*! \file
    \brief Declaration of the bt_io_vapi_t
    
*/


#ifndef __NEOIP_BT_IO_VAPI_HPP__ 
#define __NEOIP_BT_IO_VAPI_HPP__ 

/* system include */
#include <stdio.h>	// just to get the size_t
/* local include */
#include "neoip_bt_io_read_cb.hpp"
#include "neoip_bt_io_write_cb.hpp"
#include "neoip_bt_io_stopping_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_mfile_t;
class	bt_io_mode_t;
class	bt_io_read_t;
class	bt_io_write_t;
class	bt_io_stopping_t;
class	file_range_t;

/** \brief Virtual API for the io plugins
 * 
 * - it handles the input/output of the local data for the bt_swarm_t
 * - it MUST be constructed BEFORE the bt_swarm_t and deleted AFTER.
 */
class bt_io_vapi_t {
public:
	// return the bt_mfile_t on which this bt_io_vapi_t operate
	virtual const bt_mfile_t &	bt_mfile()	const throw() = 0;
	// return the bt_io_mode_t of this bt_io_vapi_t
	virtual const bt_io_mode_t &	mode()		const throw() = 0;
	//! contruct a read request
	virtual bt_io_read_t *	read_ctor(const file_range_t &totfile_range
				, bt_io_read_cb_t *callback, void *userptr)	throw() = 0;
	//! contruct a write request
	virtual bt_io_write_t *	write_ctor(const file_range_t &totfile_range, const datum_t &data2write
				, bt_io_write_cb_t *callback, void *userptr)	throw() = 0;
	//! remove a piece_idx synchrnously (no callback)
	virtual bt_err_t	remove(const file_range_t &totfile_range)	throw() = 0;
	//! return true if this bt_io_vapi_t need a stopping
	virtual bool		need_stopping()					const throw() = 0;
	//! contruct a stopping request
	virtual bt_io_stopping_t*stopping_ctor(bt_io_stopping_cb_t *callback
							, void *userptr)	throw() = 0;

	// virtual destructor
	virtual ~bt_io_vapi_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_VAPI_HPP__  */



