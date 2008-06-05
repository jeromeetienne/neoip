/*! \file
    \brief Header of the bt_io_cache_blkwr_t
    
*/


#ifndef __NEOIP_BT_IO_CACHE_BLKWR_HPP__ 
#define __NEOIP_BT_IO_CACHE_BLKWR_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_io_cache_blkwr_wikidbg.hpp"
#include "neoip_bt_io_cache_blkwr_cb.hpp"
#include "neoip_bt_io_write_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_file_range.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_io_cache_t;

/** \brief class definition for bt_io_cache_blkwr_t
 */
class bt_io_cache_blkwr_t : NEOIP_COPY_CTOR_DENY, private bt_io_write_cb_t
		, private wikidbg_obj_t<bt_io_cache_blkwr_t, bt_io_cache_blkwr_wikidbg_init> {
private:
	bt_io_cache_t *	m_io_cache;	//!< backpointer on the bt_io_cache_t
	file_range_t	m_blkwr_range;	//!< the file_range_t of the block being written
	
	/*************** subio_write stuff	*******************************/
	bt_io_write_t *	m_subio_write;
	bool		neoip_bt_io_write_cb(void *cb_userptr, bt_io_write_t &cb_bt_io_write
						, const bt_err_t &bt_err)	throw();
	
	/*************** callback stuff	***************************************/
	bt_io_cache_blkwr_cb_t*callback;//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const bt_err_t &bt_err)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_io_cache_blkwr_t() 		throw();
	~bt_io_cache_blkwr_t()		throw();

	/*************** Setup function	***************************************/
	bt_err_t	start(const file_size_t &chunk_off, bt_io_cache_t *p_io_cache
					, bt_io_cache_blkwr_cb_t *callback
					, void *userptr)	throw();

	/*************** Query function	***************************************/
	bt_io_cache_t *		io_cache()	const throw()	{ return m_io_cache;	}
	const file_range_t &	blkwr_range()	const throw()	{ return m_blkwr_range;	}
	
	/*************** List of friend class	*******************************/
	friend class	bt_io_cache_blkwr_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_CACHE_BLKWR_HPP__  */



