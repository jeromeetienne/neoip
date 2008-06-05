/*! \file
    \brief Definition of the \ref bt_peerpick_casto_profile_t

*/


/* system include */
/* local include */
#include "neoip_bt_peerpick_casto_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_peerpick_casto_profile_t constant
const delay_t	bt_peerpick_casto_profile_t::REQAUTH_COOP_TIMEOUT	= delay_t::from_sec(10);
const size_t	bt_peerpick_casto_profile_t::REQAUTH_COOP_MAX		= 4;
const double	bt_peerpick_casto_profile_t::REQAUTH_COOP_PREC		= 1.0;
const delay_t	bt_peerpick_casto_profile_t::REQAUTH_HOPE_TIMEOUT	= delay_t::from_sec(20);
const size_t	bt_peerpick_casto_profile_t::REQAUTH_HOPE_MAX		= 1;
const double	bt_peerpick_casto_profile_t::REQAUTH_HOPE_PREC		= 1.0;
const delay_t	bt_peerpick_casto_profile_t::REQAUTH_FSTART_TIMEOUT	= delay_t::from_sec(20);
const size_t	bt_peerpick_casto_profile_t::REQAUTH_FSTART_MAX		= 1;
const double	bt_peerpick_casto_profile_t::REQAUTH_FSTART_PREC	= 0.25;
const delay_t	bt_peerpick_casto_profile_t::REQAUTH_IDLE_TIMEOUT	= delay_t::from_sec(20);
const size_t	bt_peerpick_casto_profile_t::REQAUTH_IDLE_MAX		= 3;
const double	bt_peerpick_casto_profile_t::REQAUTH_IDLE_PREC		= 0.1;
// end of constants definition



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_peerpick_casto_profile_t::bt_peerpick_casto_profile_t()	throw()
{
	reqauth_coop_timeout	( REQAUTH_COOP_TIMEOUT	);
	reqauth_coop_max	( REQAUTH_COOP_MAX	);
	reqauth_coop_prec	( REQAUTH_COOP_PREC	);
	reqauth_hope_timeout	( REQAUTH_HOPE_TIMEOUT	);
	reqauth_hope_max	( REQAUTH_HOPE_MAX	);
	reqauth_hope_prec	( REQAUTH_HOPE_PREC	);
	reqauth_fstart_timeout	( REQAUTH_FSTART_TIMEOUT);
	reqauth_fstart_max	( REQAUTH_FSTART_MAX	);
	reqauth_fstart_prec	( REQAUTH_FSTART_PREC	);
	reqauth_idle_timeout	( REQAUTH_IDLE_TIMEOUT	);
	reqauth_idle_max	( REQAUTH_IDLE_MAX	);
	reqauth_idle_prec	( REQAUTH_IDLE_PREC	);
}

/** \brief Destructor
 */
bt_peerpick_casto_profile_t::~bt_peerpick_casto_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_peerpick_casto_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

