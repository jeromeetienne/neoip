/*! \file
    \brief Header of the bt_io_cache_read_t
    
*/


#ifndef __NEOIP_BT_IO_CACHE_READ_HPP__ 
#define __NEOIP_BT_IO_CACHE_READ_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_io_cache_read_wikidbg.hpp"
#include "neoip_bt_io_cache_blkrd_cb.hpp"
#include "neoip_bt_io_read_cb.hpp"
#include "neoip_bt_io_read.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_file_range.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_io_cache_t;
class	bt_io_cache_block_t;

/** \brief class definition for bt_io_cache_read
 */
class bt_io_cache_read_t : NEOIP_COPY_CTOR_DENY, public bt_io_read_t
			, private bt_io_cache_blkrd_cb_t
			, private zerotimer_cb_t
			, private wikidbg_obj_t<bt_io_cache_read_t, bt_io_cache_read_wikidbg_init> {
private:
	bt_io_cache_t *	m_io_cache;	//!< backpointer on the attached bt_io_cache_t
	file_range_t	m_totfile_range;//!< the totfile_range to read
	bytearray_t	buffer;		//!< the buffer to store the read data

	/*************** init_zerotimer	***************************************/
	zerotimer_t	init_zerotimer;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();

	/*************** Internal function	*******************************/
	bool		launch_next_chunk()					throw();
	bool		chunk_post_process(bt_io_cache_block_t * cache_block)	throw();	
	
	/*************** subio_read stuff	*******************************/
	bt_io_cache_blkrd_t *	m_cache_blkrd;
	bool		neoip_bt_io_cache_blkrd_cb(void *cb_userptr, bt_io_cache_blkrd_t &cb_cache_blkrd
					, const bt_err_t &bt_err
					, bt_io_cache_block_t *cache_block)	throw();
	
	/*************** callback stuff	***************************************/
	bt_io_read_cb_t*callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback_failure(const bt_err_t &bt_err) 	throw();
	bool		notify_callback(const bt_err_t &bt_err, const datum_t &io_cache_read_data) throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_io_cache_read_t(bt_io_cache_t *io_cache, const file_range_t &p_totfile_range
				, bt_io_read_cb_t *callback, void *userptr) 		throw();
	~bt_io_cache_read_t()								throw();

	/*************** query function	***************************************/
	bt_io_cache_t *		io_cache()	const throw()	{ return m_io_cache;	}
	const file_range_t &	totfile_range()	const throw()	{ return m_totfile_range;	}

	/*************** List of friend class	*******************************/
	friend class	bt_io_cache_read_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_CACHE_READ_HPP__  */



