/*! \file
    \brief Definition of the \ref kad_bstrap_profile_t

*/


/* system include */
/* local include */
#include "neoip_kad_bstrap_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref kad_bstrap_profile_t constant
const size_t		kad_bstrap_profile_t::DFILE_MAX_NB_ADDR		= 200;
const delay_t		kad_bstrap_profile_t::NSLAN_PEER_RECORD_TTL	= delay_t::from_sec(30);
const delay_t		kad_bstrap_profile_t::CAND_PING_RPC_TIMEOUT	= delay_t::from_sec(30);
const delay_t		kad_bstrap_profile_t::CAND_CLOSESTNODE_TIMEOUT	= delay_t::from_sec(3*60);
const size_t		kad_bstrap_profile_t::CAND_NB_CONCURRENT	= 5;
const delay_t		kad_bstrap_profile_t::STOP_MULTPUB_DELAY	= delay_t::from_sec(60);
const size_t		kad_bstrap_profile_t::STOP_MULTPUB_NPEER	= 3;
const delay_t		kad_bstrap_profile_t::STOP_MULTANY_DELAY	= delay_t::from_sec(90);
const size_t		kad_bstrap_profile_t::STOP_MULTANY_NPEER	= 10;
const delay_t		kad_bstrap_profile_t::STOP_SINGANY_DELAY	= delay_t::from_min(1440);
const size_t		kad_bstrap_profile_t::STOP_SINGANY_NPEER	= 1;
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_bstrap_profile_t::kad_bstrap_profile_t()	throw()
{
	dfile_max_nb_addr		(DFILE_MAX_NB_ADDR);
	nslan_peer_record_ttl		(NSLAN_PEER_RECORD_TTL);
	cand_ping_rpc_timeout		(CAND_PING_RPC_TIMEOUT);
	cand_closestnode_timeout	(CAND_CLOSESTNODE_TIMEOUT);
	cand_nb_concurrent		(CAND_NB_CONCURRENT);
	stop_multpub_delay		(STOP_MULTPUB_DELAY);
	stop_multpub_npeer		(STOP_MULTPUB_NPEER);
	stop_multany_delay		(STOP_MULTANY_DELAY);
	stop_multany_npeer		(STOP_MULTANY_NPEER);
	stop_singany_delay		(STOP_SINGANY_DELAY);
	stop_singany_npeer		(STOP_SINGANY_NPEER);
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
kad_err_t	kad_bstrap_profile_t::check()	const throw()
{
	// check the stop_*_delay being in order. multpub >= multany >= singany
	if( stop_multpub_delay() > stop_multany_delay()	)	return kad_err_t(kad_err_t::ERROR, "stop_multpub_delay > stop_multany_delay()");
	if( stop_multany_delay() > stop_singany_delay()	)	return kad_err_t(kad_err_t::ERROR, "stop_multany_delay > stop_singany_delay()");
	// check the nlay_profile
	kad_err_t	kad_err	 = src().check();
	if( !kad_err.succeed() )	return kad_err;
	// return no error
	return kad_err_t::OK;
}

NEOIP_NAMESPACE_END

