/*! \file
    \brief Class to handle the bt_io_cache_t

\par Brief Description
bt_io_cache_t is a userspace cache on top of another bt_io_vapi_t.
- it allows write-thru or delayed-write
- it allows to limit the size of the cache via bt_io_cache_pool_t profile
- when delayed-write is used, the actual write operation are bundled if possible
  before being sent to the subio_vapi
- it is possible to clean the cache periodically via timer 

\par About dirty block writing - and centralization in bt_io_cache_t
- there are 3 parts in bt_io_cache_t which are triggering block writting
  - bt_io_cache_pool_t when a DIRTY block needs to be removed from the pool
    in order to make room for a new block.
  - bt_io_cache_stopping_t to write all dirty blocks before destructing the
    bt_io_cache_t itself.
  - bt_io_cache_t to periodically write dirty blocks on disk.
- those 3 parts needs to know when their writting is completed and if it
  produced an errors
- in order to avoid races, and complexity, all the block writing is centralized
  in the bt_io_cache_t.
  - to write a block via bt_io_cache_blkwr_t is started with bt_io_cache_t::start_blkwr(offset)
    - used by bt_io_cache_pool_t to write least recently used blocks to make room
      for a new block
  - to write all dirty blocks is started by bt_io_cache_t::start_blkwr_all()
    - used by bt_io_cache_stopping_t and by bt_io_cache_t periodic cleaning
- on ANY bt_io_cache_blkwr_t notification, ALL parts are notified of the completion
  - bt_io_cache_stopping_t::notify_blkwr_completion() is called for all bt_io_cache_stopping_t
  - bt_io_cache_pool_t::notify_blkwr_completion() is called for the attcached bt_io_cache_pool_t
- all bt_io_cache_block_t which are to be written MUST be bt_io_cache_state_t::DIRTY

\par Periodic cleaning of dirty block
- it is possible to trigger dirty block cleaning periodically with the 
  bt_io_cache_profile_t::cleaning_max_delay()

\par About writing mode: delayed-write and write-thru
- delayed-write means that the bt_io_write_t may notify result *BEFORE* the 
  data is actually written on disk.
- write-thru ensure that the data are written on disk (at least given to the 
  operating system filesystem) before returning.
  - in write-thru, all the blocks are always in bt_io_cache_state_t::CLEAN
    because no blocks is modified in memory 
- delayed-write and error reporting
  - this has consequences in the error reporting.
  - bt_io_write_t may notify success while the data will never been written on 
    disk. e.g. due to no more space or error permission
  - if a delayed write failed, all subsequent write will all return this error
    - TODO not sure it is good because how to get out of this ? aka how to get back
      to a non error state ?
  - this error is contained into bt_io_cache_t::delayed_write_err()
- performance comparison: advantage to delayed-write 
  - delayed-write is able to group multiple io write operation into a single
    call to the subio_vapi.
    - this reduces the overhead
  - delayed-write may simple never write the data on disk, if they are deleted
    before a write on disk is ordered.
  - those 2 factors makes delayed-write more efficients in some cases
    - e.g. neoip-oload or casti/casto which in fact simply transfert data from
      network to network, and so doesnt need it on disk.
- delayed-write and relation with bt_ezswarm_stopping_t
  - as delayed write keeps data to write dangling, it is required to ensure that
    the bt_io_cache_t is entirely clean before leaving bt_ezswarm_t
  - this is done by a bt_io_cache_stopping_t inside bt_ezswarm_stopping_t
  - NOTE: some apps dont handle the bt_ezswarm_stopping_t so it may results
    in inconsistancy with the bt_swarm_resumedata_t and the disk content.
  - so IF the data MUST be on disk at the end of bt_swarm_resumedata_t and IF
    delayed-write is used, bt_ezswarm_stopping_t MUST be honored.

\par About removing and forwarding it to the subio_vapi
- currently the bt_io_cache_t::remove *always* forward it to the subio_vapi
- BUT in some case, the data in the bt_io_cache_block_t has never been
  on disk, so the files dont exists, and forwarding may be useless
  because there are no file to remove

\par Possible Improvement of the write-thru
- the current implementation is to simple forward the bt_io_write_t to the 
  subio_vapi.
- this is simple but suboptimal, because this write doesnt cache the written
  data in memory.
  - so a read on the same data will require some subio_vapi read which are not
    required
- POSSIBLE SOLUTION:
  - have a special bt_io_cache_write_thru_t which is used only for write-thru
  - in this object, first do a subio_vapi write on the data, then from the data
    written create bt_io_cache_block_t if the data are fully covered the block
    range


*/

/* system include */
/* local include */
#include "neoip_bt_io_cache.hpp"
#include "neoip_bt_io_cache_pool.hpp"
#include "neoip_bt_io_cache_read.hpp"
#include "neoip_bt_io_cache_write.hpp"
#include "neoip_bt_io_cache_stopping.hpp"
#include "neoip_bt_io_cache_block.hpp"
#include "neoip_bt_io_cache_blkwr.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_prange.hpp"
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
bt_io_cache_t::bt_io_cache_t()		throw()
{
	// initialize some variables
	m_io_mode		= bt_io_mode_t::CACHE;
	m_cache_pool		= NULL;
	m_subio_vapi		= NULL;
	m_delayed_write_err	= bt_err_t::OK;
}

/** \brief Destructor
 */
bt_io_cache_t::~bt_io_cache_t()		throw()
{
	// close all pending bt_io_cache_read_t
	DBGNET_ASSERT( read_db.empty() );
	while( !read_db.empty() )	nipmem_delete read_db.front();	
	// close all pending bt_io_cache_write_t
	DBGNET_ASSERT( write_db.empty() );
	while( !write_db.empty() )	nipmem_delete write_db.front();
	// close all pending bt_io_cache_stopping_t
	DBGNET_ASSERT( stopping_db.empty() );
	while( !stopping_db.empty() )	nipmem_delete stopping_db.front();

	// unlink this object from the bt_io_cache_pool_t if needed
	if( m_cache_pool )	m_cache_pool->cache_unlink(this);

// TODO bug ?
// - if there are still from blkwr_db, how the io_cache_t gonna be notified
//   of its completions ?!?!?
// - it may be left dangling for ever ?

	// delete the blkwr_db
	while( !blkwr_db.empty() ){
		// get the first element
		bt_io_cache_blkwr_t *	cache_blkwr	= blkwr_db.front();
		// remove the first element from the database
		blkwr_db.pop_front();
		// delete the element itself
		nipmem_zdelete	cache_blkwr;
	}

	// delete all bt_io_cache_block_t
	while( !block_db.empty() )	nipmem_delete block_db.begin()->second;
	// delete the subio_vapi if needed
	nipmem_zdelete	m_subio_vapi;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
bt_io_cache_t & bt_io_cache_t::profile(const bt_io_cache_profile_t &p_profile)	throw()
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
bt_err_t	bt_io_cache_t::start(bt_io_cache_pool_t *m_cache_pool
				, bt_io_vapi_t *m_subio_vapi)	throw()
{
	// copy the parameter
	this->m_cache_pool	= m_cache_pool;
	this->m_subio_vapi	= m_subio_vapi;

	// link this object from the bt_io_cache_pool_t
	m_cache_pool->cache_dolink(this);

	// start the cleaning_timeout
	cleaning_timeout.start(profile().cleaning_max_delay(), this, NULL );
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the bt_mfile_t on which this bt_io_vapi_t operate
 */
const bt_mfile_t &	bt_io_cache_t::bt_mfile()	const throw()
{
	return subio_vapi()->bt_mfile();	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_io_cache_blkwr_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start a bt_io_cache_blkwr_t on the asked_off
 */
void	bt_io_cache_t::start_blkwr(const file_size_t &asked_off)	throw()
{
	bt_io_cache_blkwr_t *	cache_blkwr;
	bt_err_t		bt_err;
	// start the bt_io_cache_blkwr_t
	cache_blkwr	= nipmem_new bt_io_cache_blkwr_t();
	bt_err		= cache_blkwr->start(asked_off, this, this, NULL);
	// handle the error case
	if( bt_err.failed() ){
		// set the delayed_write_err in the bt_io_cache_t of this block
		m_delayed_write_err	= bt_err;
		// delete the cache_blkwr
		nipmem_zdelete	cache_blkwr;
		// TODO not sure what to do here ?
		// - will this error be reported?
		// - how will the caller be notified of the completion
		return;		
	}
	// put the cache_blkwr in the blkwr_db
	blkwr_db.push_back(cache_blkwr);
}

/** \brief Start a bt_io_cache_blkwr_t on any dirty block
 */
void	bt_io_cache_t::start_blkwr_all()				throw()
{
	// loop to launch a bt_io_cache_blkwr_t on ALL dirty block
	while( true ){
		block_db_t::iterator	iter;
		// go thru the whole block_db
		for(iter = block_db.begin(); iter != block_db.end(); iter++){
			bt_io_cache_block_t *	cache_block	= iter->second;
			// if this cache_block IS NOT dirty, goto the next
			if( cache_block->state().is_not_dirty() )	continue;
			// launch a bt_io_cache_blkwr_t on it
			start_blkwr( cache_block->beg() );
			// leave the loop
			break;
		}
		// if no new bt_io_cache_blkwr_t has been launched, leave the loop
		if( iter == block_db.end() )	break;
	}
}


/** \brief callback called when bt_io_cache_blkwr_t is completed
 */
bool	bt_io_cache_t::neoip_bt_io_cache_blkwr_cb(void *cb_userptr
		, bt_io_cache_blkwr_t &cb_cache_blkwr, const bt_err_t &bt_err)	throw()
{
	bt_io_cache_blkwr_t *	cache_blkwr	= &cb_cache_blkwr;
	// remove the object from the blkwr_db
	blkwr_db.remove(cache_blkwr);
	// delete the object itself
	nipmem_zdelete	cache_blkwr;

	// if cache_blkwr notifyed an error, set the m_delayed_write_err
	if( bt_err.failed() )	m_delayed_write_err	= bt_err;
	
	// notify the io_cache_pool of the bt_io_cache_blkwr_t completion
	cache_pool()->notify_blkwr_completion();
	

	// notify the stopping_db if present
	if( !stopping_db.empty() ){
		bt_io_cache_stopping_t * cache_stopping	= stopping_db.front();
		// sanity check - stopping_db MUST be 1
		DBG_ASSERT( stopping_db.size() == 1 );
		// notify the cache_stopping of the completion
		cache_stopping->notify_blkwr_completion();
		// NOTE: nothing must be done after in case the stopping callback deleted
		//       bt_io_cache_t itself	
		return false;
	}
		
	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			cleaning stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	bt_io_cache_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_ERR("enter");

	// start a bt_io_cache_blkwr_t on any dirty block
	start_blkwr_all();	
	
	// NOTE: let the cleaning_timeout runs
	// - even if it is retriggered before all the just launched bt_io_cache_blkwr_t
	//   are completed, it wont collide.
	//   - because start_blkwr_all only uses DIRTY blocks and bt_io_cache_blkwr_t
	//     put the blocks it operates on in CLEANING
	
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			block_db stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief DoLink a bt_io_cache_block_t
 */
void 	bt_io_cache_t::block_dolink(bt_io_cache_block_t *block)		throw()
{
	// sanity check - the cache_block MUST NOT be in the block_db
	DBG_ASSERT( block_db.find(block->end()) == block_db.end() );
	// dolink the bt_io_cache_block_t
	bool	succeed	= block_db.insert( std::make_pair(block->end(), block) ).second;
	DBG_ASSERT( succeed );
}

/** \brief UnLink a bt_io_cache_block_t
 */
void	bt_io_cache_t::block_unlink(bt_io_cache_block_t *block)		throw()
{
	// sanity check - the cache_block MUST be in the block_db
	DBG_ASSERT( block_db.find(block->end()) != block_db.end() );
	// unlink the bt_io_cache_block_t	
	block_db.erase( block->end() );
}

/** \brief Return the block_range for this asked_off
 */
file_range_t	bt_io_cache_t::block_range_for(const file_size_t &asked_off)	const throw()
{
	const bt_io_cache_pool_profile_t & pool_profile	= cache_pool()->profile();
	// compute the block_beg - the asked_off aligned by profile.block_align_size()
	file_size_t	block_beg	= asked_off - (asked_off % pool_profile.block_align_size());
	// compute the block_end - block_beg + profile.block_maxlen() -1
	file_size_t	block_end	= block_beg + pool_profile.block_maxlen() - 1;
	// handle the last block case - clamp block_end with bt_mfile.totfile_size() 
	block_end	= std::min(block_end, bt_mfile().totfile_size()-1);
	// return the block_range from it
	return file_range_t(block_beg, block_end);
}


/** \brief return a bt_io_cache_block_t containing this offset, or null is none does 
 */
bt_io_cache_block_t *	bt_io_cache_t::block_find(const file_size_t &asked_off)	const throw()
{
	// log to debug
	KLOG_DBG("block_db.size()=" <<block_db.size());
	// find the first bt_io_cache_block_t::end() >= to the asked_off
	block_db_t::const_iterator	iter	= block_db.lower_bound(asked_off);
	// if no block has been found, return NULL
	if( iter == block_db.end() )			return NULL;
	// set an alias to the found block
	bt_io_cache_block_t *	cache_block	= iter->second;
	// if the found block DO NOT contains the asked_off, return NULL
	if( cache_block->dont_contain(asked_off) )	return NULL;	
	// else return the found block
	return cache_block;
}

/** \brief Return a list of pointer on all the bt_io_cache_block_t fully included in the range
 */
std::list<bt_io_cache_block_t *> bt_io_cache_t::block_fully_included_in(
					const file_range_t &totfile_range)	throw()
{
	std::list<bt_io_cache_block_t *>	result_db;

	// find the first bt_io_cache_block_t::end() >= to the asked_off
	block_db_t::iterator	iter	= block_db.lower_bound(totfile_range.beg());

	// loop until either the end of block_db or first block fully after totfile_range
	for( ; iter != block_db.end(); iter++){
		// set an alias to the found block
		bt_io_cache_block_t *	cache_block	= iter->second;
		// if this bt_io_cache_block_t is fully included, put is in result_db 
		if( cache_block->range().fully_included_in(totfile_range) )
			result_db.push_back(cache_block);		
		// if this cache_block is after the totfile_range, leave the loop
		if( cache_block->fully_after(totfile_range) )	break;
	}
	// return the result
	return result_db;
}

/** \brief Return a list of bt_io_cache_block_t with at least one containing asked_off
 *         and all being contiguous and dirty
 */
std::list<bt_io_cache_block_t *> 
bt_io_cache_t::block_dirty_contiguous(const file_size_t &asked_off)		const throw()
{
	std::list<bt_io_cache_block_t *>	result_db;

// find the bt_io_cache_block_t which contain asked_off	
	block_db_t::const_iterator	prev_iter;
	block_db_t::const_iterator	curr_iter;
	// find the first bt_io_cache_block_t::end() >= to the asked_off
	curr_iter	= block_db.lower_bound(asked_off);
	// if no block has been found, return NULL
	if( curr_iter == block_db.end() )			return result_db;
	// if found block DO NOT contains the asked_off, return NULL
	if( curr_iter->second->dont_contain(asked_off) )	return result_db;
	// if found block IS NOT dirty, return NULL
	if( curr_iter->second->state().is_not_dirty() )		return result_db;

// find the first block which is contiguous and dirty - first as in with the lowest offsets
	while(true){
		// copy the curr_iter to the prev_iter
		// - prev_iter is the always pointing on a dirty block contiguous to asked_off
		prev_iter	= curr_iter;
		// if the curr_iter is already the first element of block_db, leave the loop
		if( curr_iter == block_db.begin() )	break;
		// decrease curr_iter to go on the block with lower offset
		curr_iter--;
		// if curr_iter range is not contiguous to prev_iter, leave the loop
		const file_range_t &	prev_range	= prev_iter->second->range();
		const file_range_t &	curr_range	= curr_iter->second->range();
		if( curr_range.is_not_contiguous(prev_range) )		break;
		// if curr_iter doesnt point on a non-dirty block, leave the loop  
		if( curr_iter->second->state().is_not_dirty() )		break;
	}

// populate the result_db - by going forward from prev_iter
	curr_iter	= prev_iter;
	while(true){
		// put the curr_iter block into the result
		result_db.push_back( curr_iter->second );
		// copy the curr_iter to the prev_iter
		prev_iter	= curr_iter;
		// increase curr_iter
		curr_iter++;
		// if the curr_iter is the end element of block_db, leave the loop
		if( curr_iter == block_db.end() )	break;
		// if curr_iter bt_io_cache_block_t is not contiguous to prev_iter, leave the loop
		const file_range_t &	prev_range	= prev_iter->second->range();
		const file_range_t &	curr_range	= curr_iter->second->range();
		if( curr_range.is_not_contiguous(prev_range) )		break;
		// if curr_iter doesnt point on a non-dirty block, leave the loop  
		if( curr_iter->second->state().is_not_dirty() )		break;
	}

#if 1
	// sanity check - all blocks of result_db MUST be dirty and contiguous
	// - additionnaly it MUST contains asked_off
	if( !result_db.empty() ){
		std::list<bt_io_cache_block_t *>::iterator	curr_iter;
		std::list<bt_io_cache_block_t *>::iterator	prev_iter;
		// initialize the curr_iter
		curr_iter	= result_db.begin();
		DBG_ASSERT( (*curr_iter)->state().is_dirty() );
		// init the result_range
		file_range_t	result_range	= (*curr_iter)->range();
		while(true){
			// copy the curr_iter to the prev_iter
			prev_iter	= curr_iter;
			// increase curr_iter
			curr_iter++;
			// if the curr_iter is the end element of result_db, leave the loop
			if( curr_iter == result_db.end() )	break;
			// if curr_iter MUST be contiguous to prev_iter
			DBG_ASSERT( (*curr_iter)->range().is_contiguous((*prev_iter)->range()) );
			// if curr_iter MUST be dirty  
			DBG_ASSERT( (*curr_iter)->state().is_dirty() );
			// update the result range end
			result_range.end( (*curr_iter)->range().end() );
		}
		// sanity check - the result range MUST contains the asked_off
		DBG_ASSERT( result_range.contain(asked_off) );
	}
#endif
	
	// return the result
	return result_db;
}

/** \brief Remove all bt_io_cache_block_t which are fully included in totfile_range
 */
void bt_io_cache_t::block_remove_included_in(const file_range_t &totfile_range)	throw()
{
	// NOTE: todel_db is a temporary list containing all the bt_io_cache_block_t
	// - this is required to avoid a 'walk and delete'
	std::list<bt_io_cache_block_t *>	todel_db;
	todel_db	= block_fully_included_in(totfile_range);
	
	// delete all the blocks in todel_db
	while( !todel_db.empty() ){
		// dequeue the front element of todel_db
		bt_io_cache_block_t *	cache_block	= todel_db.front();
		todel_db.pop_front();
		// delete the bt_io_cache_block_t
		nipmem_zdelete	cache_block;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       bt_io_vapi_t function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Construct a bt_io_read_t request
 */
bt_io_read_t *	bt_io_cache_t::read_ctor(const file_range_t &totfile_range
					, bt_io_read_cb_t *callback, void *userptr)	throw()
{
	return nipmem_new bt_io_cache_read_t(this, totfile_range, callback, userptr);
}

/** \brief Construct a bt_io_write_t request
 */
bt_io_write_t *	bt_io_cache_t::write_ctor(const file_range_t &totfile_range, const datum_t &data2write
					, bt_io_write_cb_t *callback, void *userptr)	throw()
{
	// if write-thru, simply forward to the subio_vapi
	if( profile().write_thru() ){
		// remove all bt_io_cache_block_t contained in this totfile_range
		// - NOTE: this ensure that previously-read data will not remain 
		block_remove_included_in(totfile_range);
		// simply return a write on the subio_vapi
		return subio_vapi()->write_ctor(totfile_range, data2write, callback, userptr);
	}
	// else it is write-delayed, and so use the bt_io_cache_write_t
	return nipmem_new bt_io_cache_write_t(this, totfile_range, data2write, callback, userptr);
}

/** \brief remove a totfile_range in sync
 */
bt_err_t	bt_io_cache_t::remove(const file_range_t &totfile_range)	throw()
{
	// remove all bt_io_cache_block_t which are fully included in totfile_range
	block_remove_included_in(totfile_range);

#if 0
	// forward the remove to the subio_vapi	
	return subio_vapi()->remove(totfile_range);
#else
	// TODO to improve this...
	// - how to handle the subio_vapi remove ? error
	// - i mean the remove is ok IIF has been written before
	// - but how can i konw if a block has been written before ?
	bt_err_t	bt_err = subio_vapi()->remove(totfile_range);
	if( bt_err.failed() )	KLOG_ERR("removing " << totfile_range << " failed due to " << bt_err);
	return bt_err_t::OK;
#endif
}

/** \brief contruct a bt_io_stopping_t
 */
bt_io_stopping_t *	bt_io_cache_t::stopping_ctor(bt_io_stopping_cb_t *callback
							, void *userptr)	throw()
{
	bt_io_cache_stopping_t*	cache_stopping;
	bt_err_t		bt_err;
	// start a bt_io_cache_stopping_t
	cache_stopping	= nipmem_new bt_io_cache_stopping_t();
	bt_err		= cache_stopping->start(this, callback, userptr);
	if( bt_err.failed() ){
		// log the event 
		KLOG_ERR("Cant start a bt_io_cache_stopping_t due to " << bt_err);
		// delete the object and return NULL
		nipmem_zdelete	cache_stopping;
		return NULL;
	}
	// return a pointer on the start bt_io_cache_stopping_t
	return cache_stopping;
}

NEOIP_NAMESPACE_END





