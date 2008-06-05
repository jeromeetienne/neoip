/*! \file
    \brief Class to handle the bt_io_cache_stopping_t
 
\par Brief Description
bt_io_cache_stopping_t is made to be used within bt_ezswarm_stopping_t. It writes 
all the dirty blocks to disk and notify the caller only after that. Thus the disk
is in sync with the cache after this bt_io_cache_stopping_t.
- it use bt_io_cache_t::start_blkwr_all to write the dirty blocks.
  - it works because bt_ezswarm_t is garanteed not to do any bt_io_vapi_t operation
    during the stopping. except this bt_io_cache_stopping_t of course :)
  

*/

/* system include */
/* local include */
#include "neoip_bt_io_cache_stopping.hpp"
#include "neoip_bt_io_cache.hpp"
#include "neoip_bt_io_cache_block.hpp"
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
bt_io_cache_stopping_t::bt_io_cache_stopping_t()	throw()
{
	// zero some variables
	m_io_cache	= NULL;
}

/** \brief Destructor
 */
bt_io_cache_stopping_t::~bt_io_cache_stopping_t()		throw()
{
	// unlink this object from the bt_io_cache_t
	if( m_io_cache )	io_cache()->stopping_unlink(this);
}


/** \brief Start the operation
 */
bt_err_t	bt_io_cache_stopping_t::start(bt_io_cache_t *p_io_cache
				, bt_io_stopping_cb_t *callback, void *userptr)	throw()
{
	// copy the parameter
	this->m_io_cache	= p_io_cache;
	this->callback		= callback;
	this->userptr		= userptr;

	// link this object to the bt_io_cache_t
	io_cache()->stopping_dolink(this);

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Function called by bt_io_cache_t when a bt_io_cache_blkwr_t is completed
 */
void	bt_io_cache_stopping_t::notify_blkwr_completion()	throw()
{
	// if there are still some bt_io_cache_blkwr_t in bt_io_cache_t, do nothing
	if( io_cache()->blkwr_db.empty() == false )	return;

	// NOTE: the bt_io_cache_stopping_t caller may delete bt_io_cache_t itself
	// - it is not an issue because it is called last in from bt_io_cache_t and acts 
	//   as if bt_io_cache_t was deleted anyway

	// if io_cache()->delayed_write_err() is not succeed, notify that
	if( io_cache()->delayed_write_err().failed() ){
		notify_callback( io_cache()->delayed_write_err() );
		return;
	}
	
#if 1
	// sanity check - all the bt_io_cache_block_t MUST be clean
	bt_io_cache_t::block_db_t::iterator	iter;
	// go thru the whole block_db
	for(iter = io_cache()->block_db.begin(); iter != io_cache()->block_db.end(); iter++){
		bt_io_cache_block_t *	cache_block	= iter->second;
		// sanity check - this bt_io_cache_block_t MUST be clean
		DBG_ASSERT( cache_block->state().is_clean() );
	}
#endif

	// notify success to the caller
	notify_callback( bt_err_t::OK ); 
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback of the caller
 */
bool	bt_io_cache_stopping_t::notify_callback(const bt_err_t &bt_err)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_io_stopping_cb(userptr, *this, bt_err);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// sanity check - tokeep MUST be false as caller MUST always delete this object in the callback
	DBG_ASSERT( !tokeep );
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





