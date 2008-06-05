/*! \file
    \brief Definition of the \ref bt_peerpick_plain_profile_t

*/


/* system include */
/* local include */
#include "neoip_bt_peerpick_plain_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_peerpick_plain_profile_t constant
const delay_t	bt_peerpick_plain_profile_t::REQAUTH_COOP_TIMEOUT	= delay_t::from_sec(10);
const size_t	bt_peerpick_plain_profile_t::REQAUTH_COOP_MAX		= 4;
const double	bt_peerpick_plain_profile_t::REQAUTH_COOP_PREC		= 0.5;
const delay_t	bt_peerpick_plain_profile_t::REQAUTH_HOPE_TIMEOUT	= delay_t::from_sec(30);
const size_t	bt_peerpick_plain_profile_t::REQAUTH_HOPE_MAX		= 1;
const double	bt_peerpick_plain_profile_t::REQAUTH_HOPE_PREC		= 0.1;
const delay_t	bt_peerpick_plain_profile_t::REQAUTH_GIVE_TIMEOUT	= delay_t::from_sec(40);
const size_t	bt_peerpick_plain_profile_t::REQAUTH_GIVE_MAX		= 4;
const double	bt_peerpick_plain_profile_t::REQAUTH_GIVE_PREC		= 1.0;
// end of constants definition



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_peerpick_plain_profile_t::bt_peerpick_plain_profile_t()	throw()
{
	reqauth_coop_timeout	( REQAUTH_COOP_TIMEOUT	);
	reqauth_coop_max	( REQAUTH_COOP_MAX	);
	reqauth_coop_prec	( REQAUTH_COOP_PREC	);
	reqauth_hope_timeout	( REQAUTH_HOPE_TIMEOUT	);
	reqauth_hope_max	( REQAUTH_HOPE_MAX	);
	reqauth_hope_prec	( REQAUTH_HOPE_PREC	);
	reqauth_give_timeout	( REQAUTH_GIVE_TIMEOUT	);
	reqauth_give_max	( REQAUTH_GIVE_MAX	);
	reqauth_give_prec	( REQAUTH_GIVE_PREC	);
}

/** \brief Destructor
 */
bt_peerpick_plain_profile_t::~bt_peerpick_plain_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_peerpick_plain_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

