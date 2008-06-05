/*! \file
    \brief Class to handle the bt_io_cache_pool_t
    
\par Brief Description
bt_io_cache_pool_t contains all the bt_io_cache_block_t for each bt_io_cache_t.
- bt_io_cache_pool_t is to be for several bt_swarm_t.
  - like in bt_ezsession_t
- bt_io_cache_t is for a single bt_swarm_t
  - like in bt_ezswarm_t

\par About cache pool management
- the policy is a simply least-recently-used is discard when room is needed
- the usage of the bt_io_cache_block_t are accounted in the bt_io_cache_pool_t::block_lru
  - bt_io_cache_read_t and bt_io_cache_write_t are responsable to notify the 
    bt_io_cache_pool_t of their operation on the block via the function
    bt_io_cache_block_t::notify_*()

\par Algorithm to shrink
- a shrink is needed when the bt_io_cache_pool_t::pool_curlen() is more than 
  the profile().pool_maxlen()
- NOTE: it is possible that the bt_io_cache_pool_t pool_curlen is actually higher
  than pool_maxlen() but only while waiting for block to be written on disk.
  - moreover the periodic cleaning in bt_io_cache_t may reduces the frequency of 
    this case by writing blocks before they are deleted by a bt_io_cache_pool_t shrink
- ALGO:
  - after the creation of each new bt_io_cache_block_t, do a shrink_if_needed
  - inside each notify_blkwr_completion() from the bt_io_cache_t, do a shrink_if_needed
  - in shrink_if_needed, do shrink_one_block until it is useless or impossible to continue
    - useless to continue because the pool_curlen is <= than the max length
    - impossible to continue because the block to shrink is to be written or being written
  - in shrunk_one_block:
    - get the least recently used block
    - if it is clean, delete it
    - if it is dirty, start a blkwr on it
    - if it is cleaning, do nothing
  

*/

/* system include */
/* local include */
#include "neoip_bt_io_cache_pool.hpp"
#include "neoip_bt_io_cache.hpp"
#include "neoip_bt_io_cache_block.hpp"
#include "neoip_bt_io_cache_blkwr.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_io_cache_pool_t::bt_io_cache_pool_t()		throw()
{
	// set for variables
	m_pool_curlen	= 0;
}

/** \brief Destructor
 */
bt_io_cache_pool_t::~bt_io_cache_pool_t()		throw()
{
	// close all pending bt_io_cache_cache_t
	DBGNET_ASSERT( cache_db.empty() );
	while( !cache_db.empty() )	nipmem_delete cache_db.front();	

	// sanity check - the block_lru MUST be empty
	// - as it is at least emptied in deleting all bt_io_cache_t
	DBG_ASSERT( block_lru.empty() );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
bt_io_cache_pool_t & bt_io_cache_pool_t::profile(const bt_io_cache_pool_profile_t &p_profile)
										throw()
{
	// sanity check - the profile MUST be check().succeed()
	DBG_ASSERT( p_profile.check().succeed() );	
	// copy the parameter
	this->m_profile	= p_profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
bt_err_t	bt_io_cache_pool_t::start()	throw()
{
	// NOTE: do nothing
	
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			block_lru stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Insert a new block into the pool
 */
void	bt_io_cache_pool_t::block_lru_dolink(bt_io_cache_block_t *block)	throw()
{
	// sanity check - aka it MUST NOT be already in the block_lru
	DBG_ASSERT( block_lru.find(block) == block_lru.end() );

	// put this block at the head of the list
	block_lru.push_front(block->lru_item(), block);
	// update the pool_curlen
	m_pool_curlen	+= block->length();

	// shrink the pool if needed
	shrink_if_needed();
}

/** \brief Remove a block from the pool
 */
void	bt_io_cache_pool_t::block_lru_unlink(bt_io_cache_block_t *block)	throw()
{
	// sanity check - aka it MUST NOT be already in the block_lru
	DBG_ASSERT( block_lru.find(block) != block_lru.end() );

	// remove this block from the list
	block_lru.remove(block->lru_item());
	// update the pool_curlen
	DBG_ASSERT( pool_curlen() >= block->length() );
	m_pool_curlen	-= block->length();
}

/** \brief Remove a block from the pool
 */
void	bt_io_cache_pool_t::block_lru_update(bt_io_cache_block_t *block)	throw()
{
	// sanity check - aka it MUST already be in the block_lru
	DBG_ASSERT( block_lru.find(block) != block_lru.end() );

	// remove this block from the list
	block_lru.remove(block->lru_item());
	// put this block at the head of the list
	block_lru.push_front(block->lru_item(), block);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			shrinking process
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Initialize the pool shrink process if needed
 * 
 * - aka do shrink_one_block until it is either
 *   - useless to continue because the pool_curlen is <= than the max length
 *   - impossible to continue because the block to shrink is to be written or being
 *     written
 */
void	bt_io_cache_pool_t::shrink_if_needed()	throw()
{
	// if pool_curlen is less than pool_maxlen, do nothing
	if( pool_curlen() <= profile().pool_maxlen() )	return;

	// shrink block until either it is impossible to continue or if the size if now ok
	while( pool_curlen() > profile().pool_maxlen() ){
		// try to shrink this block
		bool tocontinue	= shrink_one_block();
		// if the shrinking cant continue, leave the loop now
		if( !tocontinue )	break;
	}
}

/** \brief Try to shrink one block
 * 
 * @return a tocontinue, aka true if one might try to continue the shrinking
 *         false otherwise
 */
bool	bt_io_cache_pool_t::shrink_one_block()	throw()
{
	// if block_lru is empty, return dontcontinue
	if( block_lru.empty() )	return false;

	// get the least recently used block
	bt_io_cache_block_t *	last_block	= block_lru.back();

	// handle it differently depending on the last_block state
	switch( last_block->state().get_value() ){
	case bt_io_cache_state_t::CLEAN:
			// if the last_block is clean, simply delete it
			nipmem_zdelete	last_block;
			// return tocontinue
			return true;
	case bt_io_cache_state_t::DIRTY:
			// launch a bt_io_cache_blkwr_t on this last_block
			last_block->io_cache()->start_blkwr( last_block->beg() );
			// return dontcontinue
			return false;
	case bt_io_cache_state_t::CLEANING:
			// do nothing, shrink_if_needed will be called when 
			// this block will be written
			
			// return dontcontinue
			return false;
	default:	DBG_ASSERT( 0 );
	}
	// NOTE: this point should never be reached
	DBG_ASSERT( 0 );
	return false;
}

/** \brief Function notified when bt_io_cache_t complete a bt_io_cache_blkwr_t
 */
void	bt_io_cache_pool_t::notify_blkwr_completion()	throw()
{
	// reinitiate a shrink if needed
	shrink_if_needed();
}
NEOIP_NAMESPACE_END





