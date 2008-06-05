/*! \file
    \brief Definition of the \ref bt_peerpick_casti_profile_t

*/


/* system include */
/* local include */
#include "neoip_bt_peerpick_casti_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_peerpick_casti_profile_t constant
#if 0
	const delay_t	bt_peerpick_casti_profile_t::REQAUTH_HOPE_TIMEOUT	= delay_t::from_sec(20);
	const size_t	bt_peerpick_casti_profile_t::REQAUTH_HOPE_MAX		= 4;
#else	// debug only value
	const delay_t	bt_peerpick_casti_profile_t::REQAUTH_HOPE_TIMEOUT	= delay_t::from_sec(20);
	const size_t	bt_peerpick_casti_profile_t::REQAUTH_HOPE_MAX		= 10;
#endif
// end of constants definition



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_peerpick_casti_profile_t::bt_peerpick_casti_profile_t()	throw()
{
	reqauth_hope_timeout	(REQAUTH_HOPE_TIMEOUT);
	reqauth_hope_max	(REQAUTH_HOPE_MAX);
}

/** \brief Destructor
 */
bt_peerpick_casti_profile_t::~bt_peerpick_casti_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_peerpick_casti_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

