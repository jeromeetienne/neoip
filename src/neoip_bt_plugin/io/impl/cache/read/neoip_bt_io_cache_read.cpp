/*! \file
    \brief Class to handle the bt_io_cache_read_t

*/

/* system include */
/* local include */
#include "neoip_bt_io_cache_read.hpp"
#include "neoip_bt_io_cache.hpp"
#include "neoip_bt_io_cache_block.hpp"
#include "neoip_bt_io_cache_blkrd.hpp"
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
bt_io_cache_read_t::bt_io_cache_read_t(bt_io_cache_t *p_io_cache
				, const file_range_t &p_totfile_range
				, bt_io_read_cb_t *callback, void *userptr)	throw()
{
	// log to debug
	KLOG_DBG("enter totfile_range=" << p_totfile_range );
	// zero some variables
	this->m_cache_blkrd	= NULL;

	// copy the parameter
	this->m_io_cache	= p_io_cache;
	this->m_totfile_range	= p_totfile_range;
	this->callback		= callback;
	this->userptr		= userptr;
	// link to the bt_io_cache_t
	m_io_cache->read_dolink(this);
	
	// preallocate the data_queue to avoid reallocation during the read
	buffer		= bytearray_t(totfile_range().length().to_size_t());

	// update the statistics bt_io_stats_t in bt_io_cache_t
	io_cache()->stats().update_for_read(totfile_range());
	
	// launch the init_zerotimer to avoid a 'notification within ctor'
	// - TODO this may not be required if no notification is made
	//   - aka if some block must be read from disk anyway
	// - in this case this is a useless timer... so waste of rescource
	// - to comment in the header and using zerotimer is simpler for a first version
	// - moreover how costy it is . does the speedup gain worth the added complexity
	init_zerotimer.append(this, NULL);
}

/** \brief Destructor
 */
bt_io_cache_read_t::~bt_io_cache_read_t()		throw()
{
	// unlink from the bt_io_cache_t
	io_cache()->read_unlink(this);
	// delete the cache_blkrd if needed
	nipmem_zdelete	m_cache_blkrd;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       ini_zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	bt_io_cache_read_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// just forward to the chunk
	return launch_next_chunk();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			processing for each chunk
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch the next chunk
 */
bool	bt_io_cache_read_t::launch_next_chunk()	throw()
{
	bt_io_cache_block_t *	cache_block;
	// log to debug 
	KLOG_DBG("enter");

	// sanity check - cache_blkrd MUST be NULL
	DBG_ASSERT( !m_cache_blkrd );

	// compute the begining offset of the current chunk
	file_size_t	chunk_beg	= totfile_range().beg() + buffer.length();
	// try to find a exiting bt_io_cache_block_t for this chunk
	cache_block	= io_cache()->block_find(chunk_beg);

	// if a cache_block is existing, do the chunk_post_process on this cache_block
	if( cache_block )	return chunk_post_process(cache_block);	

	// if no cache_block is existing, create one and do chunk_post_process later
	DBG_ASSERT( !cache_block );

	// start the bt_io_cache_blkrd_t for this chunk_beg
	bt_err_t	bt_err;	
	m_cache_blkrd	= nipmem_new bt_io_cache_blkrd_t();
	bt_err		= m_cache_blkrd->start(chunk_beg, io_cache(), this, NULL);
	if( bt_err.failed() ){
		KLOG_ERR("cant start m_cache_blkrd due to " << bt_err);
		return notify_callback_failure(bt_err);
	}
	// return tokeep
	return true;
}

/** \brief Post processing of a chunk 
 * 
 * @return a tokeep for the bt_io_cache_read_t
 */
bool bt_io_cache_read_t::chunk_post_process(bt_io_cache_block_t * cache_block)	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// sanity check - the cache_block MUST be the one for the current offset
	DBG_ASSERT(cache_block==io_cache()->block_find(totfile_range().beg()+buffer.length()));

	// get the common range between the cache_block and the totfile_range
	file_range_t	common_range	= cache_block->common_range( totfile_range() );
	// sanity check - common_range MUST NOT be null 
	// - because cache_block is here *because* it has common range
	DBG_ASSERT( !common_range.is_null() );
	// shift the common_range to get 0 at the begining of the cache_block->beg
	// - aka to get the range unit *within* the cache_block->datum()
	common_range	-= cache_block->beg();
	// some alias to ease readability
	const datum_t &	block_datum	= cache_block->datum();
	size_t		datum_beg	= common_range.beg().to_size_t();
	size_t		datum_len	= common_range.length().to_size_t();
	// sanity check - datum_beg + datum_len MUST be less than cache_block->datum().length()
	DBG_ASSERT( datum_beg + datum_len <= cache_block->datum().length() );
	// sanity check - the buffer is supposed to be preallocated
	DBG_ASSERT( buffer.tail_freelen() >= datum_len);
	// append the common_range of the cache_block->datum() to the buffer
	buffer.append( block_datum.range(datum_beg, datum_len, datum_t::NOCOPY) );

	// notify the cache_block that it has been read
	cache_block->notify_read();

	// if buffer.length() == totfile_range.length, there are nomore chunk, notify success
	if( buffer.length() == totfile_range().length().to_size_t() )
		return notify_callback(bt_err_t::OK, buffer.to_datum(datum_t::NOCOPY));	

	// launch the next chunk
	return launch_next_chunk();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			cache_blkrd callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_io_read_t when to notify the result of the operation
 */
bool	bt_io_cache_read_t::neoip_bt_io_cache_blkrd_cb(void *cb_userptr, bt_io_cache_blkrd_t &cb_cache_blkrd
			, const bt_err_t &bt_err, bt_io_cache_block_t *cache_block)	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// delete the cache_blkrd
	nipmem_zdelete	m_cache_blkrd;

	// if cache_blkrd return an error, notify the callback 
	if( bt_err.failed() )	return notify_callback_failure(bt_err); 
	
	// call the chunk_post_process with the new cache_block
	bool	tokeep	= chunk_post_process(cache_block);
	if( !tokeep )	return false;

	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback of the caller in case of failure
 */
bool	bt_io_cache_read_t::notify_callback_failure(const bt_err_t &bt_err) 	throw()
{
	return notify_callback(bt_err, datum_t());
}

/** \brief notify the callback of the caller
 */
bool	bt_io_cache_read_t::notify_callback(const bt_err_t &bt_err
					, const datum_t &read_data)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_io_read_cb(userptr, *this, bt_err, read_data);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// sanity check - tokeep MUST be false as caller MUST always delete this object in the callback
	DBG_ASSERT( !tokeep );
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





