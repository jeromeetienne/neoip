/*! \file
    \brief Definition of the \ref kad_profile_t

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_kad_profile.hpp"
#include "neoip_kad_pkttype.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref kad_rpc_profile_t constant
const pkttype_profile_t	kad_profile_t::PKTTYPE_PROFILE	= pkttype_profile_t(0, 16, pkttype_profile_t::UINT8);
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_profile_t::kad_profile_t()	throw()
{
	// set the default pkttype_profile_t
	pkttype		(PKTTYPE_PROFILE);	
	// set the profile for the local and remote database
	local_db	(kad_db_profile_t::build_local_db());
	remote_db	(kad_db_profile_t::build_remote_db());
	
	// sanity check - the resulting profile MUST be valid
	DBG_ASSERT( check().succeed() );
}

/** \brief Destructor
 */
kad_profile_t::~kad_profile_t()	throw()
{
}


//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     valid function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
kad_err_t	kad_profile_t::check()	const throw()
{
	kad_err_t	kad_err;
	// check the validity of all subprofile
	kad_err		= rpc().check();
	if( kad_err.succeed() )		kad_err	= bstrap().check();
	if( kad_err.succeed() )		kad_err	= kbucket().check();
	if( kad_err.succeed() )		kad_err	= nsearch().check();
	if( kad_err.succeed() )		kad_err	= local_db().check();
	if( kad_err.succeed() )		kad_err	= remote_db().check();
	if( kad_err.succeed() )		kad_err	= clicnx().check();
	if( kad_err.failed() )		return kad_err;
	
	// check that the pkttype_profile_t.nb_reserved() is <= kad_pkttype_t::size()
	if( pkttype().nb_reserved() < kad_pkttype_t::size() ){
		std::string	reason = "the pkttype_profile_t::nb_reserved() should be equal to or "
					"greater than kad_pkttype_t.";
		return kad_err_t(kad_err_t::ERROR, reason);
	}
	
	// return no error
	return kad_err_t::OK;
}


NEOIP_NAMESPACE_END

