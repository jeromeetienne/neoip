/*! \file
    \brief Class to handle the bt_io_cache_blkwr_t

\par Brief Description
bt_io_cache_blkwr_t does a delayed write on all the bt_io_cache_block_t in
bt_io_cache_state_t::DIRTY AND contiguous to the block containing
the asked_off.
- NOTE: this contiguous part is made to reduce the number of operation sent
  to the subio_vapi and hopefully reduce its overhead on disk

\par About relation between with bt_io_blkwr_t and bt_io_cache_state_t
- NOTE: just after the bt_io_cache_blkwr_t::start(), all the bt_io_cache_block_t 
  which are concerned by this bt_io_cache_blkwr_t are passed into
  bt_io_cache_state_t::CLEANING.
- so those blocks can no more be selected by another bt_io_cache_blkwr_t during
  the execution of this one.

*/

/* system include */
/* local include */
#include "neoip_bt_io_cache_blkwr.hpp"
#include "neoip_bt_io_cache.hpp"
#include "neoip_bt_io_cache_block.hpp"
#include "neoip_bt_io_write.hpp"
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
bt_io_cache_blkwr_t::bt_io_cache_blkwr_t()	throw()
{
	// zero some variables
	m_subio_write	= NULL;
}

/** \brief Destructor
 */
bt_io_cache_blkwr_t::~bt_io_cache_blkwr_t()		throw()
{
	// delete the subio_write
	nipmem_zdelete	m_subio_write;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_io_cache_blkwr_t::start(const file_size_t &chunk_beg
				, bt_io_cache_t *p_io_cache
				, bt_io_cache_blkwr_cb_t *callback, void *userptr)	throw()
{
	std::list<bt_io_cache_block_t *>		block_db;
	std::list<bt_io_cache_block_t *>::iterator	iter;
	// copy the parameter
	this->m_io_cache	= p_io_cache;
	this->callback		= callback;
	this->userptr		= userptr;
	
	// get the block_db - aka all the dirty continuous block containing chunk_beg
	block_db	= io_cache()->block_dirty_contiguous(chunk_beg);	
	
	// TODO if block_db.size() is equal to 1, it is useless to create another
	// datum_t to copy it, i can use the one in block_db
	// - do launch_subio_write_one(block_db) and launch_subio_write_many(block_db)
	//   to handle this case
	
	// sanity check - the block_db MUST NOT be empty
	DBG_ASSERT( !block_db.empty() );
	
	// compute the total_len of the block_db
	for(iter = block_db.begin(); iter != block_db.end(); iter++){
		bt_io_cache_block_t *	cache_block	= *iter;
		// sanity check - all the blocks MUST be in DIRTY
		DBG_ASSERT( cache_block->state().is_dirty() );
		// if m_blkwr_range is not yet init, set it to this cache_block->range()
		if( m_blkwr_range.is_null() )	m_blkwr_range	= cache_block->range();
		// update the blkwr_range
		m_blkwr_range.beg	( std::min( m_blkwr_range.beg(), cache_block->beg()) );	 
		m_blkwr_range.end	( std::max( m_blkwr_range.end(), cache_block->end()) );
	}

	// build the data2write datum
	datum_t	data2write(blkwr_range().length().to_size_t());
	char *	ptr	= data2write.char_ptr();
	for(iter = block_db.begin(); iter != block_db.end(); iter++){
		bt_io_cache_block_t *	cache_block	= *iter;
		// copy this cache_block into the data2write datum_t
		memcpy( ptr, cache_block->datum().char_ptr(), cache_block->length() );
		// update the ptr 
		ptr	+= cache_block->length();
	}
	
	// construct a bt_io_write_t in the subio_vapi
	bt_io_vapi_t *	subio_vapi	= io_cache()->subio_vapi();
	m_subio_write	= subio_vapi->write_ctor(blkwr_range(), data2write, this, NULL);	

	// notify the cleaning to all the blocks of block_db 
	for(iter = block_db.begin(); iter != block_db.end(); iter++){
		bt_io_cache_block_t *	cache_block	= *iter;
		// notify cleaning for this block
		cache_block->notify_cleaning();
	}

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			subio_write callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_io_write_t when to notify the result of the operation
 */
bool	bt_io_cache_blkwr_t::neoip_bt_io_write_cb(void *cb_userptr, bt_io_write_t &cb_bt_io_write
						, const bt_err_t &bt_err)	throw()
{
	// log to debug
	KLOG_ERR("enter");

	// if subio_write return an error, notify the callback 
	if( bt_err.failed() ){
		// TODO pass all the block in block_db into dirty ?
		// - TODO where to i handle the delayed_write error in bt_io_cache_t
		return notify_callback(bt_err);
	} 

	// delete the subio_write
	nipmem_zdelete	m_subio_write;

// pass all the blocks cleaning in blkwr_range in clean now
	// get all the blocks within blkwr_range
	std::list<bt_io_cache_block_t *>	block_db;
	block_db	= io_cache()->block_fully_included_in(blkwr_range());
	// notify the cleaned to all the blocks of block_db 
	std::list<bt_io_cache_block_t *>::iterator	iter;
	for(iter = block_db.begin(); iter != block_db.end(); iter++){
		bt_io_cache_block_t *	cache_block	= *iter;
		// if this cache_block is no more in cleaning, goto the next
		// NOTE: it is possible that this block has been modified DURING the 
		//       cleaning and so is now back in dirty 
		if( cache_block->state().is_not_cleaning() ){
			DBG_ASSERT( cache_block->state().is_dirty() );
			continue;
		}
		// notify cleaned for this block
		cache_block->notify_cleaned();
	}
	
	// notify the caller - NOTE: the cache_block is now owned by the caller
	bool tokeep	= notify_callback(bt_err_t::OK);
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
bool	bt_io_cache_blkwr_t::notify_callback(const bt_err_t &bt_err)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_io_cache_blkwr_cb(userptr, *this, bt_err);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// sanity check - tokeep MUST be false as caller MUST always delete this object in the callback
	DBG_ASSERT( !tokeep );
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





