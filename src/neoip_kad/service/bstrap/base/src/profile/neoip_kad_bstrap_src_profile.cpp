/*! \file
    \brief Definition of the \ref kad_bstrap_src_profile_t

*/


/* system include */
/* local include */
#include "neoip_kad_bstrap_src_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref kad_bstrap_src_profile_t constant
const delay_t	kad_bstrap_src_profile_t::SFILE_CREATION_DELAY	= delay_t::from_sec(3*60);
const delay_t	kad_bstrap_src_profile_t::NEGCACHE_TIMEOUT	= delay_t::from_sec(3*60);
const delay_t	kad_bstrap_src_profile_t::NSLAN_NOQUERY_TIMEOUT	= delay_t::from_sec(0);
const delay_t	kad_bstrap_src_profile_t::DFILE_NOQUERY_TIMEOUT	= delay_t::from_sec(3);
const delay_t	kad_bstrap_src_profile_t::SFILE_NOQUERY_TIMEOUT	= delay_t::from_sec(3);
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_bstrap_src_profile_t::kad_bstrap_src_profile_t()	throw()
{
	sfile_creation_delay	(SFILE_CREATION_DELAY);
	negcache_timeout	(NEGCACHE_TIMEOUT);
	nslan_noquery_timeout	(NSLAN_NOQUERY_TIMEOUT);
	dfile_noquery_timeout	(DFILE_NOQUERY_TIMEOUT);
	sfile_noquery_timeout	(SFILE_NOQUERY_TIMEOUT);
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
kad_err_t	kad_bstrap_src_profile_t::check()	const throw()
{
	// return no error
	return kad_err_t::OK;
}

NEOIP_NAMESPACE_END

