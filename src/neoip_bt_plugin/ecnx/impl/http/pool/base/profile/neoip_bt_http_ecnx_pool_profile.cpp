/*! \file
    \brief Definition of the \ref bt_http_ecnx_pool_profile_t

*/


/* system include */
/* local include */
#include "neoip_bt_http_ecnx_pool_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref bt_http_ecnx_pool_profile_t constant
#if 1
	const delaygen_arg_t	bt_http_ecnx_pool_profile_t::HERR_DELAYGEN	= delaygen_expboff_arg_t()
								.min_delay(delay_t::from_sec(1))
								.max_delay(delay_t::from_sec(3))
								.random_range(0.2);
	const size_t		bt_http_ecnx_pool_profile_t::CONCURRENT_CNX_MAX	= 1;
#else
	const delaygen_arg_t	bt_http_ecnx_pool_profile_t::HERR_DELAYGEN	= delaygen_expboff_arg_t()
								.min_delay(delay_t::from_sec(10))
								.max_delay(delay_t::from_sec(5*60))
								.random_range(0.2);
	const size_t		bt_http_ecnx_pool_profile_t::CONCURRENT_CNX_MAX	= 2;
#endif
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_http_ecnx_pool_profile_t::bt_http_ecnx_pool_profile_t()	throw()
{
	// init each plain field with its default value
	herr_delaygen		(HERR_DELAYGEN		);
	concurrent_cnx_max	(CONCURRENT_CNX_MAX	);
}

/** \brief Destructor
 */
bt_http_ecnx_pool_profile_t::~bt_http_ecnx_pool_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_http_ecnx_pool_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

