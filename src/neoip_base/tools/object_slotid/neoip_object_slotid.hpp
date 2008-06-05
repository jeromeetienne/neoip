/*! \file
    \brief Declaration of the object_slotid_t

*/


#ifndef __NEOIP_OBJECT_SLOTID_HPP__ 
#define __NEOIP_OBJECT_SLOTID_HPP__ 
/* system include */
/* local include */
#include "neoip_slotpool.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief object_slotid_t assigned a unique slotid to each instance of the 
 *         its derived class.
 * 
 * - it allow to determine easily if a given object still exists or not. This 
 *   is usefull when passing object to a callback without knowing if it get deleted
 *   or not, but still needing to return a tokeep value for this object
 * - the uniqueness of the slotid is 'garanteed' as it is taken from slotpool_t and 
 *   slotpool_t except if the uint32_t counter warp around, so one would have to be
 *   pretty unlucky to have a collision aka to allocate exactly 2^32 slot (around 4billions) 
 *   on this slotpool between the slot_id_t backup and its existance
 * 
 * - Usage Description:
 *   class foo : public object_slotid_t {};
 *   
 *   (in foo callback)
 *   slot_id_t	foo_slotid = foo.get_object_slotid();
 *   ....
 *   passing foo to a callback without knowing if foo will be deleted or not
 *   ....
 *   return object_slotid_tokeep( foo_slotid );
 * 
 */ 
class object_slotid_t : NEOIP_COPY_CTOR_ALLOW {
private:
	slot_id_t	slot_id;	//!< the slot_id of this object
	void		internal_dtor()	throw();
	void		internal_ctor()	throw();
public:
	/*************** ctor/dtor	***************************************/
	object_slotid_t()				throw();
	~object_slotid_t()				throw();
	object_slotid_t(const object_slotid_t &other)	throw();
	object_slotid_t &	operator=(const object_slotid_t& other);

	/*************** query function	***************************************/
	const slot_id_t	get_object_slotid()	const throw()	{ return slot_id;	}
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//       single function to 'convert' a object_slotid into a 'tokeep'
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool	object_slotid_exist(slot_id_t &slotid)	throw();


/** \brief return a 'tokeep' kindof thing for the slotid
 * 
 * - it automatically 'compute' the tokeep value depending on the slotid
 * 
 * @return true if the slot_id_t is present in the global database, false otherwise
 */
inline bool object_slotid_tokeep(slot_id_t &slotid)	throw()
{
	return object_slotid_exist(slotid);
}

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_OBJECT_SLOTID_HPP__  */



