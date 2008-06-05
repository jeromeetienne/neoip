/*! \file
    \brief Class to handle the bt_io_cache_blkrd_t

\par Brief Description
bt_io_cache_blkrd_t reads a single bt_io_cache_block_t data from the 
subio_vapi. It construct and notifies a pointer on bt_io_cache_block_t.
this bt_io_cache_block_t is then owned by the caller.

*/

/* system include */
/* local include */
#include "neoip_bt_io_cache_blkrd.hpp"
#include "neoip_bt_io_cache.hpp"
#include "neoip_bt_io_cache_block.hpp"
#include "neoip_bt_io_read.hpp"
#include "neoip_file_range.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"


NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_io_cache_blkrd_t::bt_io_cache_blkrd_t()	throw()
{
	// zero some variables
	this->m_subio_read	= NULL;
}

/** \brief Destructor
 */
bt_io_cache_blkrd_t::~bt_io_cache_blkrd_t()		throw()
{
	// delete the subio_read if needed
	nipmem_zdelete m_subio_read;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_io_cache_blkrd_t::start(const file_size_t &chunk_beg
					, bt_io_cache_t *p_io_cache
					, bt_io_cache_blkrd_cb_t *callback
					, void *userptr)	throw()
{
	// copy the parameter
	this->m_io_cache	= p_io_cache;
	this->callback		= callback;
	this->userptr		= userptr;

	// get the block_range for this chunk_beg
	m_block_range	= io_cache()->block_range_for(chunk_beg);

	// construct a bt_io_read_t in the subio_vapi
	m_subio_read	= io_cache()->subio_vapi()->read_ctor(block_range(), this, NULL);	

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			subio_read callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_io_read_t when to notify the result of the operation
 */
bool	bt_io_cache_blkrd_t::neoip_bt_io_read_cb(void *cb_userptr, bt_io_read_t &cb_bt_io_read
			, const bt_err_t &bt_err, const datum_t &read_data)	throw()
{
	// log to debug
	KLOG_ERR("enter");

	// if subio_read return an error, notify the callback 
	if( bt_err.failed() )	return notify_callback(bt_err, NULL); 

	// sanity check - the block_range and the read_data MUST have the same length
	DBG_ASSERT( block_range().length() == read_data.length() );

	// build the bt_io_cache_block_t with those data
	bt_io_cache_block_t *	cache_block;
	cache_block	= nipmem_new bt_io_cache_block_t(io_cache(), block_range(), read_data);

	// delete the subio_read
	nipmem_zdelete	m_subio_read;
	
	// notify the caller - NOTE: the cache_block is now owned by the caller
	bool tokeep	= notify_callback(bt_err_t::OK, cache_block);
	if( !tokeep )	return false;
	
	// return dontkeep
	return false;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			main callback function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback of the caller
 */
bool	bt_io_cache_blkrd_t::notify_callback(const bt_err_t &bt_err
				, bt_io_cache_block_t *cache_block)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_io_cache_blkrd_cb(userptr, *this, bt_err, cache_block);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// sanity check - tokeep MUST be false as caller MUST always delete this object in the callback
	DBG_ASSERT( !tokeep );
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





