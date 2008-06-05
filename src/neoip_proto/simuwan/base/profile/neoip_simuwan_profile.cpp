/*! \file
    \brief Definition of the \ref simuwan_profile_t

*/


/* system include */
/* local include */
#include "neoip_simuwan_profile.hpp"
#include "neoip_simuwan.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
simuwan_profile_t::simuwan_profile_t()	throw()
{
	// set the default parameter
	drop_rate_val		= simuwan_t::DROP_RATE_DFL;
	delayed_rate_val	= simuwan_t::DELAYED_RATE_DFL;
	delayed_min_val		= simuwan_t::DELAYED_MIN_DFL;
	delayed_max_val		= simuwan_t::DELAYED_MAX_DFL;
	duplicate_rate_val	= simuwan_t::DUPLICATE_RATE_DFL;
	duplicate_min_val	= simuwan_t::DUPLICATE_MIN_DFL;
	duplicate_max_val	= simuwan_t::DUPLICATE_MAX_DFL;
}

/** \brief Destructor
 */
simuwan_profile_t::~simuwan_profile_t()	throw()
{
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     to_string() function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Convert the object into a string
 */
std::string	simuwan_profile_t::to_string()	const throw()
{
	std::ostringstream oss;
	oss << "drop_rate="		<< drop_rate_val;
	oss << " delayed_rate="		<< delayed_rate_val;
	oss << " delayed_min="		<< delayed_min_val;
	oss << " delayed_max="		<< delayed_max_val;
	oss << " duplicate_rate="	<< duplicate_rate_val;
	oss << " duplicate_min="	<< duplicate_min_val;
	oss << " duplicate_max="	<< duplicate_max_val;
	return oss.str();
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
simuwan_err_t	simuwan_profile_t::check()	const throw()
{
	return simuwan_err_t::OK;
}

NEOIP_NAMESPACE_END

