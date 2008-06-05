/*! \file
    \brief Declaration of the slotpool_t



*/

/* system include */
/* local include */
#include "neoip_slotpool.hpp"
#include "neoip_rand.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
slotpool_t::slotpool_t()	throw()
{
#if 1
	slot_id_t	offset	= 1;
#else
	// choose the offset at a random point in the space
	// - this is a defensive measure to detect error faster
	// - e.g. if the caller put a slot_id value from another slotpool, it is more likely
	//   to be detected with this random initial offset that with a static one
	slot_id_t	offset	= (slot_id_t)neoip_rand(std::numeric_limits<slot_id_t>::max() - 1);
#endif
	// sanity check - the offset MUST be less than std::numeric_limits<slot_id_t>::max()
	DBG_ASSERT( offset > NONE && offset < std::numeric_limits<slot_id_t>::max() );
	// init next_slot to NONE + offset
	next_slot	= NONE + offset;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       UTILITY function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief allocate a new slot for this pointer
 */
slot_id_t	slotpool_t::allocate(const void *ptr)	throw()
{
	// loop until the next_slot points on a free slot
	// - if the current next_slot is used, increase next_slot and test again
	// - NOTE: if all slot are used, it fails and enter in a infinite loop.
	//   but it would require 4billions=2^sizeof(slot_id_t)-1 slot used. so this 
	//   case won't happen :)
	for( ; exist(next_slot) && next_slot != NONE; next_slot++ );

	// insert the pointer in the database
	bool succeed = slot_db.insert(std::make_pair(next_slot, const_cast<void *>(ptr))).second;
	DBG_ASSERT( succeed );

	// backup the allocated slot to return it
	slot_id_t	allocated_slot = next_slot;
	// update the next_slot
	next_slot++;
	// if the next_slot wrap around and reach NONE, go over NONE
	if( next_slot == NONE )	next_slot++;
	// return the allocated_slot
	return allocated_slot;
}

/** \brief release a given slot
 */
void	slotpool_t::release(slot_id_t slot)	throw()
{
	// sanity check - the slot MUST be used
	DBG_ASSERT( exist(slot) );
	// remove it from the database
	slot_db.erase(slot);
}

/** \brief return true if this slot is used, false otherwise
 */
bool	slotpool_t::exist(slot_id_t slot)	const throw()
{
	std::map<slot_id_t, void *>::const_iterator	iter = slot_db.find(slot);
	if( iter == slot_db.end() )	return false;
	return true;	
}

/** \brief return the pointer matching with slot, or NULL if none is associated
 */
void *	slotpool_t::find(slot_id_t slot)	const throw()
{
	std::map<slot_id_t, void *>::const_iterator	iter = slot_db.find(slot);
	if( iter == slot_db.end() )	return NULL;
	return iter->second;	
}


NEOIP_NAMESPACE_END







