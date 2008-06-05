/*! \file
    \brief Definition of the \ref bt_ezswarm_profile_t

- TODO to complete, it doesnt contains all the subprofile stuff
  - all the subprofile are required to maintain the configurability
*/


/* system include */
/* local include */
#include "neoip_bt_ezswarm_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_ezswarm_profile_t constant
// NOTE: use directly the strtype_* to avoid any static ctor race
// - this is ugly but it is not a local matter but a matter in NEOIP_STRTYPE define
// - e.g. bitflag_t stuff handles it this way
const bt_alloc_policy_t	bt_ezswarm_profile_t::ALLOC_POLICY	= strtype_bt_alloc_policy_t::HOLE;
const bt_check_policy_t	bt_ezswarm_profile_t::CHECK_POLICY	= strtype_bt_check_policy_t::SUBFILE_EXIST;
const bt_peerpick_mode_t bt_ezswarm_profile_t::PEERPICK_MODE	= strtype_bt_peerpick_mode_t::PLAIN;
const delay_t bt_ezswarm_profile_t::PEERSRC_KAD_STOPPING_TIMEOUT= delay_t::from_sec(10);



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_ezswarm_profile_t::bt_ezswarm_profile_t()	throw()
{
	alloc_policy			(ALLOC_POLICY);
	check_policy			(CHECK_POLICY);
	peerpick_mode			(PEERPICK_MODE);
	peersrc_kad_stopping_timeout	(PEERSRC_KAD_STOPPING_TIMEOUT);
	// peersrc_kad_peer is NULL by default - it it up to the caller to set it up if bt_peersrc_kad_t
	// - TODO why it is here ?!?!?! in a profile and not an bt_ezswarm_t parameter ?
	peersrc_kad_peer		(NULL);
}

/** \brief Destructor
 */
bt_ezswarm_profile_t::~bt_ezswarm_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_ezswarm_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

