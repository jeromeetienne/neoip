/*! \file
    \brief Class to handle the bt_io_cache_write_t

*/

/* system include */
/* local include */
#include "neoip_bt_io_cache_write.hpp"
#include "neoip_bt_io_cache.hpp"
#include "neoip_bt_io_cache_block.hpp"
#include "neoip_bt_io_cache_blkrd.hpp"
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
bt_io_cache_write_t::bt_io_cache_write_t(bt_io_cache_t *p_io_cache
			, const file_range_t &p_totfile_range, const datum_t &data2write
			, bt_io_write_cb_t *callback, void *userptr)		throw()
{
	// log to debug
	KLOG_DBG("enter totfile_range=" << p_totfile_range);
	// sanity check - bt_io_cache_write_t MUST NOT be used if bt_io_cache_t is in write-thru
	DBG_ASSERT( p_io_cache->profile().write_thru() == false );
	// copy the parameter
	this->m_io_cache	= p_io_cache;
	this->m_totfile_range	= p_totfile_range;
	this->data2write	= data2write;
	this->callback		= callback;
	this->userptr		= userptr;
	// link to the bt_io_cache_t
	m_io_cache->write_dolink(this);


	// update the statistics bt_io_stats_t in bt_io_cache_t
	io_cache()->stats().update_for_write(totfile_range());

	// set some field
	m_written_len	= 0;
	m_cache_blkrd	= NULL;

	// sanity check - the tofile_range.len() MUST be equal to the data2write.get_len()
	DBG_ASSERT( totfile_range().length() == data2write.length() );

	// launch the init_zerotimer to avoid a 'notification within ctor'
	// - TODO this may not be required if no notification is made
	//   - e.g. if some block must be read from disk anyway
	// - in this case this is a useless timer... so waste of rescource
	// - to comment in the header and using zerotimer is simpler for a first version
	// - moreover how costy it is . does the speedup gain worth the added complexity
	init_zerotimer.append(this, NULL);
}

/** \brief Destructor
 */
bt_io_cache_write_t::~bt_io_cache_write_t()		throw()
{
	// unlink from the bt_io_cache_t
	m_io_cache->write_unlink(this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       ini_zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	bt_io_cache_write_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
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
bool	bt_io_cache_write_t::launch_next_chunk()	throw()
{
	bt_io_cache_block_t *	cache_block;
	// log to debug 
	KLOG_DBG("enter");

	// sanity check - cache_blkrd MUST be NULL
	DBG_ASSERT( !m_cache_blkrd );

	// compute the begining offset of the current chunk
	file_size_t	chunk_beg	= totfile_range().beg() + written_len();
	// try to find a exiting bt_io_cache_block_t for this chunk
	cache_block	= io_cache()->block_find(chunk_beg);

	// if a cache_block is existing, do the chunk_post_process on this cache_block
	if( cache_block )	return chunk_post_process(cache_block);	

	// if no cache_block is existing, create one and do chunk_post_process later
	DBG_ASSERT( !cache_block );
	
	// if this write would completly overwrite the whole block, create one and postprocess
	// - no need to read it from subio_vapi as it would be completely be overwritten
	file_range_t	block_range	= io_cache()->block_range_for(chunk_beg);
	if( block_range.fully_included_in(totfile_range()) ){
		// create an empty bt_io_cache_block_t for this block_range
		cache_block	= nipmem_new bt_io_cache_block_t(io_cache(), block_range);
		// immediatly do the portprocess on it
		return chunk_post_process(cache_block);
	}

	// start the bt_io_cache_blkrd_t for this chunk_beg
	bt_err_t	bt_err;	
	m_cache_blkrd	= nipmem_new bt_io_cache_blkrd_t();
	bt_err		= m_cache_blkrd->start(chunk_beg, io_cache(), this, NULL);
	if( bt_err.failed() ){
		KLOG_ERR("cant start m_cache_blkrd due to " << bt_err);
		return notify_callback(bt_err);
	}
	// return tokeep
	return true;
}

/** \brief Post processing of a chunk 
 * 
 * @return a tokeep for the bt_io_cache_write_t
 */
bool bt_io_cache_write_t::chunk_post_process(bt_io_cache_block_t * cache_block)	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// handle the delayed_write_err
	if( io_cache()->delayed_write_err().failed() )
		return notify_callback( io_cache()->delayed_write_err() );		

	// sanity check - the cache_block MUST be the one for the current offset
	DBG_ASSERT(cache_block == io_cache()->block_find(totfile_range().beg() + written_len()));

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
	// update the block_datum with the data from data2write
	memcpy( block_datum.char_ptr() + datum_beg, data2write.char_ptr() + written_len().to_size_t(), datum_len );

	// update the written_len
	m_written_len	+= datum_len;

	// notify the cache_block that it has been written
	cache_block->notify_write();

	// if written_len == totfile_range().length(), there are nomore chunk, notify success
	if( written_len() == totfile_range().length() )	return notify_callback(bt_err_t::OK);	

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
bool	bt_io_cache_write_t::neoip_bt_io_cache_blkrd_cb(void *cb_userptr, bt_io_cache_blkrd_t &cb_cache_blkrd
			, const bt_err_t &bt_err, bt_io_cache_block_t *cache_block)	throw()
{
	// log to debug
	KLOG_ERR("enter bt_err=" << bt_err);
	// if cache_blkrd return an error, notify the callback 
	if( bt_err.failed() )	return notify_callback(bt_err); 

	// delete the cache_blkrd
	nipmem_zdelete	m_cache_blkrd;
	
	// call the chunk_post_process with the new cache_block
	return chunk_post_process(cache_block);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool bt_io_cache_write_t::notify_callback(const bt_err_t &bt_err)		throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_io_write_cb(userptr, *this, bt_err);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// sanity check - tokeep MUST be false as caller MUST always delete this object in the callback
	DBG_ASSERT( !tokeep );
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





