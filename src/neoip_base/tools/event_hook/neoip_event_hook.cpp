/*! \file
    \brief Implementation of \ref event_hook_t to hook event

\par Implementation Notes
\ref event_hook_t seems like a lame way to implement 'signal'.

\par TODO
- may be a good idea to implement a good code for 'signal'
  - look at libsigc++ or similar stuff in boost
  - yeah may be good :) :) :) especially better than this :)

*/

/* system include */
/* local include */
#include "neoip_event_hook.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                            CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief constructor
 */
event_hook_t::event_hook_t(size_t nb_level)	throw()
{
	for( size_t i = 0; i < nb_level; i++ )
		level_arr.push_back( hook_level() );
}

/** \brief Destructor
 */
event_hook_t::~event_hook_t()			throw()
{
	// sanity check - all the event_hook MUST have been removed
	for( size_t i = 0; i < level_arr.size(); i++ )
		DBG_ASSERT(level_arr[i].item_list.size() == 0);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                            HOOK notify
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief notify a given level
 * 
 * @return a tokeep for the event_hook_t object
 */
bool	event_hook_t::notify(size_t level_no)	throw()
{
	// sanity check 
	DBG_ASSERT( level_no < level_arr.size() );
	// do a copy of the list to allow modification during the callback
	std::list<hook_item>		tmp_list	= level_arr[level_no].item_list;
	std::list<hook_item>::iterator	iter;
	for( iter = tmp_list.begin(); iter != tmp_list.end(); iter++ ){
		hook_item	&item	= *iter;
		// backup the tokey_check_t context to check after the callback notification
		TOKEEP_CHECK_BACKUP_DFL(*item.callback);
		// notify the callback
		bool tokeep = item.callback->neoip_event_hook_notify_cb(item.userptr, this, level_no);
		// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
		TOKEEP_CHECK_MATCH_DFL(tokeep);
		// if the callback returned 'dontkeep', return false now
		if( !tokeep )	return false;
	}
	return true;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                            HOOK add/del
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief add a hook
 */
void	event_hook_t::append(size_t level_no, event_hook_cb_t *callback, void *userptr)	throw()
{
	// sanity check 
	DBG_ASSERT( level_no < level_arr.size() );
	// push the new item on the proper level
	hook_item	item(callback, userptr);
	level_arr[level_no].item_list.push_back( item );
}

/** \brief del a hook
 */
void	event_hook_t::remove(size_t level_no, event_hook_cb_t *callback, void *userptr)	throw()
{
	// sanity check 
	DBG_ASSERT( level_no < level_arr.size() );
	// build the item
	hook_item	item(callback, userptr);
	// sanity check - the item MUST be contained by the list
	DBG_ASSERT( contain(level_no, callback, userptr) );
	// remove the item from the list
	level_arr[level_no].item_list.remove( item );
}

/** \brief return true if the hook is contained by the event_hook_t, false otherwise
 */
bool	event_hook_t::contain(size_t level_no, event_hook_cb_t *callback, void *userptr)	throw()
{
	// sanity check 
	DBG_ASSERT( level_no < level_arr.size() );
	
	std::list<hook_item>::const_iterator	iter;
	const std::list<hook_item> &		item_list = level_arr[level_no].item_list;
	hook_item				item(callback, userptr);
	// go thru the whole list
	for( iter = item_list.begin(); iter != item_list.end(); iter++ ){
		// if this item is the one looked for, return true
		if( *iter == item )	return true;
	}
	// if this point is reached, the item has not been found and return false
	return false;
}


NEOIP_NAMESPACE_END



