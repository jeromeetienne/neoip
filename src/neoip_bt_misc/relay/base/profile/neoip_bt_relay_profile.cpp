/*! \file
    \brief Definition of the \ref bt_relay_profile_t

*/


/* system include */
/* local include */
#include "neoip_bt_relay_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_relay_profile_t constant
#if 0
	const delaygen_arg_t	bt_relay_profile_t::EVAL_DELAYGEN	= delaygen_expboff_arg_t()
							.min_delay(delay_t::from_sec(30))
							.max_delay(delay_t::from_sec(60))
							.random_range(0.2);
	const file_size_t	bt_relay_profile_t::MAX_CACHE_SIZE	= 10*1024*1024;
#else
	const delaygen_arg_t	bt_relay_profile_t::EVAL_DELAYGEN	= delaygen_expboff_arg_t()
							.min_delay(delay_t::from_sec(30))
							.max_delay(delay_t::from_sec(2*60))
							.random_range(0.2);
	const file_size_t	bt_relay_profile_t::MAX_CACHE_SIZE	= 30*1024*1024;
#endif
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_relay_profile_t::bt_relay_profile_t()	throw()
{
	// init each plain field with its default value
	eval_delaygen	(EVAL_DELAYGEN);
	max_cache_size	(MAX_CACHE_SIZE);
}

/** \brief Destructor
 */
bt_relay_profile_t::~bt_relay_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_relay_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

