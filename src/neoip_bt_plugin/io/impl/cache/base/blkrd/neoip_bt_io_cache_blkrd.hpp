/*! \file
    \brief Header of the bt_io_cache_blkrd_t
    
*/


#ifndef __NEOIP_BT_IO_CACHE_BLKRD_HPP__ 
#define __NEOIP_BT_IO_CACHE_BLKRD_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_io_cache_blkrd_wikidbg.hpp"
#include "neoip_bt_io_cache_blkrd_cb.hpp"
#include "neoip_bt_io_read_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_file_range.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_io_cache_t;

/** \brief class definition for bt_io_cache_blkrd_t
 */
class bt_io_cache_blkrd_t : NEOIP_COPY_CTOR_DENY, private bt_io_read_cb_t
		, private wikidbg_obj_t<bt_io_cache_blkrd_t, bt_io_cache_blkrd_wikidbg_init> {
private:
	bt_io_cache_t *	m_io_cache;	//!< backpointer on the bt_io_cache_t
	file_range_t	m_block_range;	//!< the file_range_t of the block being read

	/*************** subio_read stuff	*******************************/
	bt_io_read_t *	m_subio_read;
	bool		neoip_bt_io_read_cb(void *cb_userptr, bt_io_read_t &cb_bt_io_read
				, const bt_err_t &bt_err, const datum_t &read_data)	throw();
	
	/*************** callback stuff	***************************************/
	bt_io_cache_blkrd_cb_t*callback;//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const bt_err_t &bt_err
					, bt_io_cache_block_t *cache_block)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_io_cache_blkrd_t() 		throw();
	~bt_io_cache_blkrd_t()		throw();

	/*************** Setup function	***************************************/
	bt_err_t	start(const file_size_t &chunk_off, bt_io_cache_t *p_io_cache
					, bt_io_cache_blkrd_cb_t *callback
					, void *userptr)	throw();

	/*************** Query function	***************************************/
	bt_io_cache_t *		io_cache()	const throw()	{ return m_io_cache;	}
	const file_range_t &	block_range()	const throw()	{ return m_block_range;	}
	
	/*************** List of friend class	*******************************/
	friend class	bt_io_cache_blkrd_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_CACHE_BLKRD_HPP__  */



