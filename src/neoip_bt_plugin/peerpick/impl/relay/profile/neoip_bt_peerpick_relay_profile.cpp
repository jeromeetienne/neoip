/*! \file
    \brief Definition of the \ref bt_peerpick_relay_profile_t

*/


/* system include */
#include <float.h>
/* local include */
#include "neoip_bt_peerpick_relay_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_peerpick_relay_profile_t constant
const delay_t	bt_peerpick_relay_profile_t::REQAUTH_GIVE_TIMEOUT	= delay_t::from_sec(20);
const size_t	bt_peerpick_relay_profile_t::REQAUTH_GIVE_MAX		= 4;
const double	bt_peerpick_relay_profile_t::REQAUTH_GIVE_PREC		= 1.0;
const delay_t	bt_peerpick_relay_profile_t::REQAUTH_FSTART_TIMEOUT	= delay_t::from_sec(20);
const size_t	bt_peerpick_relay_profile_t::REQAUTH_FSTART_MAX		= 4;
const double	bt_peerpick_relay_profile_t::REQAUTH_FSTART_PREC	= 0.66;
const delay_t	bt_peerpick_relay_profile_t::REQAUTH_IDLE_TIMEOUT	= delay_t::from_sec(20);
const size_t	bt_peerpick_relay_profile_t::REQAUTH_IDLE_MAX		= std::numeric_limits<size_t>::max();
const double	bt_peerpick_relay_profile_t::REQAUTH_IDLE_PREC		= 0.33;
// end of constants definition



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_peerpick_relay_profile_t::bt_peerpick_relay_profile_t()	throw()
{
	reqauth_give_timeout	( REQAUTH_GIVE_TIMEOUT	);
	reqauth_give_max	( REQAUTH_GIVE_MAX	);
	reqauth_give_prec	( REQAUTH_GIVE_PREC	);
	reqauth_fstart_timeout	( REQAUTH_FSTART_TIMEOUT);
	reqauth_fstart_max	( REQAUTH_FSTART_MAX	);
	reqauth_fstart_prec	( REQAUTH_FSTART_PREC	);
	reqauth_idle_timeout	( REQAUTH_IDLE_TIMEOUT	);
	reqauth_idle_max	( REQAUTH_IDLE_MAX	);
	reqauth_idle_prec	( REQAUTH_IDLE_PREC	);
}

/** \brief Destructor
 */
bt_peerpick_relay_profile_t::~bt_peerpick_relay_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_peerpick_relay_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

