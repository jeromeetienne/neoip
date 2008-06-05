/*! \file
    \brief Declaration of the object_slotid_t
*/

/* system include */
/* local include */
#include "neoip_object_slotid.hpp"
#include "neoip_slotpool.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

static slotpool_t *	object_slotid_slotpool	= NULL;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
object_slotid_t::object_slotid_t()	throw()
{
	internal_ctor();
}

/** \brief Constructor
 */
object_slotid_t::~object_slotid_t()	throw()
{
	internal_dtor();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      copy operator and assignement
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Contructor by copy
 */
object_slotid_t::object_slotid_t(const object_slotid_t &other)	throw()
{
	internal_ctor();	
}

/** \brief redefinition of the operator =
 */
object_slotid_t &	object_slotid_t::operator=(const object_slotid_t& other)
{
	if( this == &other )	return *this;   // Gracefully handle self assignment
	// release the old slot_id_t for this object
	internal_dtor();
	// create a new one
	internal_ctor();
	// return the object itself
	return *this;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                   internal ctor to centralize all ctor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Private function which is called by all the ctor
 */
void	object_slotid_t::internal_ctor()	throw()
{
	// build the database if not already done
	if( !object_slotid_slotpool )	object_slotid_slotpool = new slotpool_t();
	// allocate a slot_id_t for this object
	slot_id	= object_slotid_slotpool->allocate(this);
}


/** \brief Private function which is called by all the dtor
 */
void	object_slotid_t::internal_dtor()	throw()
{
	// sanity check - the global slotpool MUST be allocated
	DBG_ASSERT( object_slotid_slotpool );
	// release slot_id_t for this object
	object_slotid_slotpool->release(slot_id);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      simple function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief return a true if the slotid exists, false otherwise
 * 
 * @return true if the slot_id_t is present in the global database, false otherwise
 */
bool	object_slotid_exist(slot_id_t &slotid)	throw()
{
	// if there are no database, return false
	if( !object_slotid_slotpool )			return false;	
	// if the slotid is NOT found, return false
	if( !object_slotid_slotpool->find(slotid) )	return false;
	// else return true
	return true;
}

NEOIP_NAMESPACE_END







