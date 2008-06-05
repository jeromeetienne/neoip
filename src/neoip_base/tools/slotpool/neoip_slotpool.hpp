/*! \file
    \brief Declaration of the slotpool_t

*/


#ifndef __NEOIP_SLOTPOOL_HPP__ 
#define __NEOIP_SLOTPOOL_HPP__ 
/* system include */
#include <map>
/* local include */
#include "neoip_slotpool_id.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief Handle a pool of unique slot associated with a pointer
 * 
 * - the slot 0 is NEVER allocated on purpose so it may be used as a special value
 * - it makes it best not to reused a previous allocated slot
 * - the allocation is similar to the PID process identificator of UNIX
 * - the first slot_id_t value allocated with a slotpool_t is chosen randomly
 *   to detect usage error faster.
 */
class slotpool_t : NEOIP_COPY_CTOR_DENY {
public:	enum dummy {
		NONE	= 0
	};
private:
	slot_id_t			next_slot;	//!< the next slot to allocate
	std::map<slot_id_t, void *>	slot_db;	//!< the database of the currently used slots
public:
	/*************** ctor/dtor	***************************************/
	slotpool_t()	throw();
	
	/*************** Utility function	*******************************/
	slot_id_t	allocate(const void *ptr)	throw();
	void		release(slot_id_t slot)		throw();
	bool		exist(slot_id_t slot)		const throw();
	void *		find(slot_id_t slot)		const throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SLOTPOOL_HPP__  */



