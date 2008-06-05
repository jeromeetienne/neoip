/*! \file
    \brief Definition of the \ref get_profile_t

*/


/* system include */
/* local include */
#include "neoip_get_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref get_profile_t constant
#if 0
	const delay_t	get_profile_t::DISPLAY_PERIOD			= delay_t::from_msec(500);
	const delay_t	get_profile_t::RESUMEDATA_AUTOSAVE_PERIOD	= delay_t::from_sec(60);
#else	// debug value
	const delay_t	get_profile_t::DISPLAY_PERIOD			= delay_t::from_msec(500);
	const delay_t	get_profile_t::RESUMEDATA_AUTOSAVE_PERIOD	= delay_t::from_sec(60);
#endif
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
get_profile_t::get_profile_t()	throw()
{
	display_period			(DISPLAY_PERIOD);
	resumedata_autosave_period	(RESUMEDATA_AUTOSAVE_PERIOD);
}

/** \brief Destructor
 */
get_profile_t::~get_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
bt_err_t	get_profile_t::check()	const throw()
{
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END

