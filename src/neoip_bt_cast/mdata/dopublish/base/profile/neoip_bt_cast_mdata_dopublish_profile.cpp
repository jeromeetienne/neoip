/*! \file
    \brief Definition of the \ref bt_cast_mdata_dopublish_profile_t

*/


/* system include */
/* local include */
#include "neoip_bt_cast_mdata_dopublish_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_cast_mdata_dopublish_profile_t constant
// TODO tune those values!!!
#if 0	// production value
const delay_t	bt_cast_mdata_dopublish_profile_t::PERIODIC_DELAY_PUSH	= delay_t::from_sec(60);
const delay_t	bt_cast_mdata_dopublish_profile_t::PERIODIC_DELAY_PULL	= delay_t::from_sec(60);
#else	// debug value
const delay_t	bt_cast_mdata_dopublish_profile_t::PERIODIC_DELAY_PUSH	= delay_t::from_sec(1);
const delay_t	bt_cast_mdata_dopublish_profile_t::PERIODIC_DELAY_PULL	= delay_t::from_sec(60);
#endif
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_cast_mdata_dopublish_profile_t::bt_cast_mdata_dopublish_profile_t()	throw()
{
	periodic_delay_push(PERIODIC_DELAY_PUSH);
	periodic_delay_pull(PERIODIC_DELAY_PULL);
}

/** \brief Destructor
 */
bt_cast_mdata_dopublish_profile_t::~bt_cast_mdata_dopublish_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_cast_mdata_dopublish_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

