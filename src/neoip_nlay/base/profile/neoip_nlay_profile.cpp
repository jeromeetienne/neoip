/*! \file
    \brief Definition of the \ref nlay_profile_t

*/


/* system include */
/* local include */
#include "neoip_nlay_profile.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
nlay_profile_t::nlay_profile_t()	throw()
{
}

/** \brief Destructor
 */
nlay_profile_t::~nlay_profile_t()	throw()
{
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
nlay_err_t	nlay_profile_t::check()	const throw()
{
	nlay_err_t	nlay_err;
	// check the closure
	nlay_err			= closure_profile.check();
	if( nlay_err.failed() )		return nlay_err;
	// check the idletimeout
	nlay_err			= idletimeout_profile.check();
	if( nlay_err.failed() )		return nlay_err;
	// check the pktfrag
	pktfrag_err_t	pktfrag_err	= pktfrag_profile.check();
	if( pktfrag_err.failed() )	return nlay_err_from_pktfrag(pktfrag_err);
	// check the pktcomp
	pktcomp_err_t	pktcomp_err	= pktcomp_profile.check();
	if( pktcomp_err.failed() )	return nlay_err_from_pktcomp(pktcomp_err);
	// check the reachpeer
	reachpeer_err_t	reachpeer_err	= reachpeer_profile.check();
	if( reachpeer_err.failed() )	return nlay_err_from_reachpeer(reachpeer_err);
	// check the rate_limit
	nlay_err			= rate_limit_profile.check();
	if( nlay_err.failed() )		return nlay_err;
	// check the scnx
	scnx_err_t	scnx_err	= scnx_profile.check();
	if( scnx_err.failed() )		return nlay_err_from_scnx(scnx_err);
	// check the simuwan
	simuwan_err_t	simuwan_err	= simuwan_profile.check();
	if( simuwan_err.failed() )	return nlay_err_from_simuwan(simuwan_err);
		
	return nlay_err_t::OK;
}

NEOIP_NAMESPACE_END

