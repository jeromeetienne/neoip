/*! \file
    \brief Definition of the \ref bt_httpo_full_profile_t

*/


/* system include */
/* local include */
#include "neoip_bt_httpo_full_profile.hpp"
#include "neoip_rate_sched.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_httpo_full_profile_t constant
const double		bt_httpo_full_profile_t::XMIT_MAXRATE		= rate_sched_t::INFINITE_RATE_VAL;
const file_size_t	bt_httpo_full_profile_t::XMIT_MAXRATE_THRES	= 0;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_httpo_full_profile_t::bt_httpo_full_profile_t()	throw()
{
	xmit_maxrate		(XMIT_MAXRATE);
	xmit_maxrate_thres	(XMIT_MAXRATE_THRES);
}

/** \brief Destructor
 */
bt_httpo_full_profile_t::~bt_httpo_full_profile_t()	throw()
{
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_httpo_full_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

