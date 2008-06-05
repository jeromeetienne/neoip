/*! \file
    \brief Definition of the \ref bt_utmsg_byteacct_profile_t

*/


/* system include */
/* local include */
#include "neoip_bt_utmsg_byteacct_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_utmsg_byteacct_profile_t constant
#if 0
	const delaygen_arg_t	bt_utmsg_byteacct_profile_t::XMIT_DELAYGEN	= delaygen_regular_arg_t()
							.period(delay_t::from_sec(60))
							.random_range(0.2);
#else
	const delaygen_arg_t	bt_utmsg_byteacct_profile_t::XMIT_DELAYGEN	= delaygen_regular_arg_t()
							.period(delay_t::from_sec(5))
							.random_range(0.2);
#endif

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_utmsg_byteacct_profile_t::bt_utmsg_byteacct_profile_t()	throw()
{
	xmit_delaygen	(XMIT_DELAYGEN);
}

/** \brief Destructor
 */
bt_utmsg_byteacct_profile_t::~bt_utmsg_byteacct_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_utmsg_byteacct_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

