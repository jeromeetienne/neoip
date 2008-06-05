/*! \file
    \brief Class to handle the bt_io_cache_block_t

- In the ctor, the default state is bt_io_cache_state_t::CLEAN
  - this is in the case the block is read from disk
  - TODO i dont like this, makes it better
    - the state MUST be explicitly set, this reduces the risk of 
      programmatic error


\par Brief Description
bt_io_cache_block_t contains a single block for the bt_io_cache_t.
it is stored in bt_io_cache_pool_t and bt_io_cache_t.

\par About the bt_io_cache_state_t meaning
- a block in bt_io_cache_state_t::CLEAN contains in memory the data which are on disk.
- a block in bt_io_cache_state_t::DIRTY contains in memory the data which are 
  not yet on disk.
- a block in bt_io_cache_state_t::CLEANING contains in memory the data which are 
  not yet on disk, BUT which are currently being written on disk.
  - this is done via the bt_io_cache_blkwr_t


*/

/* system include */
/* local include */
#include "neoip_bt_io_cache_block.hpp"
#include "neoip_bt_io_cache.hpp"
#include "neoip_bt_io_cache_pool.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor when the data are already available
 */
bt_io_cache_block_t::bt_io_cache_block_t(bt_io_cache_t * p_io_cache, const file_range_t &p_range
				, const datum_t &p_datum)	throw()
				: m_io_cache(p_io_cache), m_range(p_range), m_datum(p_datum)
{
	// do the common ctor_postprocess
	ctor_postprocess();
}

/** \brief Constructor when no data are already available
 */
bt_io_cache_block_t::bt_io_cache_block_t(bt_io_cache_t * p_io_cache
				, const file_range_t &p_range)	throw()
				: m_io_cache(p_io_cache), m_range(p_range)
				, m_datum(p_range.length().to_size_t())
{
	// do the common ctor_postprocess
	ctor_postprocess();
}

/** \brief the common postprocessing for both ctor
 */
void	bt_io_cache_block_t::ctor_postprocess()	throw()
{
	bt_io_cache_pool_t * cache_pool	= io_cache()->cache_pool();
	// sanity check - file_range_t::length MUST be size_t ok
	DBG_ASSERT( range().length().is_size_t_ok() );
	// sanity check - file_range_t::length MUST be equal to datum_t length
	DBG_ASSERT( range().length().to_size_t() == datum().length() );

	// set the state to bt_io_cache_state_t::CLEAN
	m_state	= bt_io_cache_state_t::CLEAN;
	// dolink to bt_io_cache_t
	io_cache()->block_dolink(this);
	// dolink to bt_io_cache_pool_t
	cache_pool->block_lru_dolink(this);	
}

/** \brief Destructor
 */
bt_io_cache_block_t::~bt_io_cache_block_t()		throw()
{
	bt_io_cache_pool_t * cache_pool	= io_cache()->cache_pool();
	// unlink from bt_io_cache_t
	io_cache()->block_unlink(this);
	// unlink from bt_io_cache_pool_t
	cache_pool->block_lru_unlink(this);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the common range between the block one and the other 
 */
file_range_t	bt_io_cache_block_t::common_range(const file_range_t &other)	const throw()
{
	// return the range of the block clamped_by the other file_range_t
	file_range_t	common_range	= range();
	common_range.clamped_by(other);
	// return the result
	return common_range;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Function used by caller to notify a read on this bt_io_cache_block_t
 */
void	bt_io_cache_block_t::notify_read()	throw()
{
	bt_io_cache_pool_t * cache_pool	= io_cache()->cache_pool();
	// update bt_io_cache_pool_t block_lru
	cache_pool->block_lru_update(this);
}

/** \brief Function used by caller to notify a write on this bt_io_cache_block_t
 */
void	bt_io_cache_block_t::notify_write()	throw()
{
	bt_io_cache_pool_t * cache_pool	= io_cache()->cache_pool();
	// update bt_io_cache_pool_t block_lru
	cache_pool->block_lru_update(this);
	// change the state to bt_io_cache_state_t::DIRTY
	m_state	= bt_io_cache_state_t::DIRTY;
}

/** \brief Function used by the caller to notify that this block is cleaning right now
 * 
 * - NOTE: it will be notify_cleaned if the cleaning succeed
 */
void	bt_io_cache_block_t::notify_cleaning()	throw()
{
	// sanity check - it MUST NOT happen if the state is not DIRTY
	DBG_ASSERT( state().is_dirty() );		
	
	// change the state to bt_io_cache_state_t::CLEANING
	m_state	= bt_io_cache_state_t::CLEANING;
}

/** \brief Function used by the caller to notify that this block is cleaning right now
 * 
 * - NOTE: it will be notify_cleaned if the cleaning succeed
 */
void	bt_io_cache_block_t::notify_cleaned()	throw()
{
	// if state is dirty, do nothing
	// - NOTE: it may happen if a block has been modified *during* its cleaning
	if( state().is_dirty() )	return;

	// sanity check - it MUST NOT happen if the state is not CLEANING
	DBG_ASSERT( state().is_cleaning() );

	// change the state to bt_io_cache_state_t::CLEAN
	m_state	= bt_io_cache_state_t::CLEAN;
}

NEOIP_NAMESPACE_END





