/*! \file
    \brief Definition of the \ref bt_io_pfile_asyncdel_profile_t

*/


/* system include */
/* local include */
#include "neoip_bt_io_pfile_asyncdel_profile.hpp"

NEOIP_NAMESPACE_BEGIN;


// definition of \ref bt_io_pfile_asyncdel_profile_t constant
#if 0
	const delaygen_arg_t	bt_io_pfile_asyncdel_profile_t::DELETOR_DELAYGEN	= delaygen_regular_arg_t()
								.first_delay(delay_t::from_sec(60))
								.period(delay_t::from_sec(60))
								.random_range(0.2);
#else	// debug values
	const delaygen_arg_t	bt_io_pfile_asyncdel_profile_t::DELETOR_DELAYGEN	= delaygen_regular_arg_t()
								.first_delay(delay_t::from_sec(10))
								.period(delay_t::from_sec(10))
								.random_range(0.2);
#endif
// end of constants definition


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_io_pfile_asyncdel_profile_t::bt_io_pfile_asyncdel_profile_t()	throw()
{
	// set the constant fields
	deletor_delaygen	(DELETOR_DELAYGEN);
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	bt_io_pfile_asyncdel_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

