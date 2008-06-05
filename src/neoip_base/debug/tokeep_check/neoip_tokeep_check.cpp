/*! \file
    \brief Declaration of the tokeep_check_t

\par TODO
- to rename tokeep 
  - cbret ?
  - cblive ?
  - beendeleted ? this is very clear and descriptive
    - true = not been deleted ?
    - false = been delete ?
    - well not that obvious after all :)
  - or i change the whole thing to avoid the 'true/false' and have a explicit constant to make it more 
    clear ?
    - yep this seems like the good thing to do.
    - obviously do that in a compatible way to allow smooth transition.
    - return cbret_t::NOT_BEEN_DELETED
    - return cbret_t::BEEN_DELETED
    - with cbret_t being compatible with bool... like maybe simply a typedef at first
      - #define CBRET_NOT_BEEN_DELETED	true
      - #define CBRET_BEEN_DELETED	false

================================================================================
class cbret_t : public bool {
public:	here define constant ?
	bool	has_been_deleted()	return value in bool :)
}

	cbret_t	cbret	= notify_callback();
	if( cbret.has_been_deleted() )	return cbret_t::BEEN_DELETED;
	

*/

/* system include */
/* local include */
#include "neoip_tokeep_check.hpp"
#include "neoip_slotpool.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

slotpool_t *	tokeep_check_slotpool	= NULL;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
tokeep_check_t::tokeep_check_t()	throw()
{
	internal_ctor();
}

/** \brief Constructor
 */
tokeep_check_t::~tokeep_check_t()	throw()
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
tokeep_check_t::tokeep_check_t(const tokeep_check_t &other)	throw()
{
	internal_ctor();	
}

/** \brief redefinition of the operator =
 */
tokeep_check_t &	tokeep_check_t::operator=(const tokeep_check_t& other)
{
	if( this == &other )	return *this;   // Gracefully handle self assignment
	// release the old slot_id_t for this object
	internal_dtor();
	// allocate a new slot_id_t 
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
void	tokeep_check_t::internal_ctor()	throw()
{
	// build the database if not already done
	if( !tokeep_check_slotpool )	tokeep_check_slotpool = new slotpool_t();
	// allocate a slot_id_t for this object
	slot_id	= tokeep_check_slotpool->allocate(this);
}

/** \brief Private function which is called by all the dtor
 */
void	tokeep_check_t::internal_dtor()	throw()
{
	// sanity check - the global slotpool MUST be allocated
	DBG_ASSERT( tokeep_check_slotpool );
	// release slot_id_t for this object
	tokeep_check_slotpool->release(slot_id);
}

NEOIP_NAMESPACE_END







