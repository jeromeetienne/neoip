/*! \file
    \brief Definition of the \ref kad_kbucket_profile_t

*/


/* system include */
/* local include */
#include "neoip_kad_kbucket_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref kad_kbucket_profile_t constant
#if 0
	const delay_t	kad_kbucket_profile_t::PING_RPC_TIMEOUT			= delay_t::from_sec(60);
	const delay_t	kad_kbucket_profile_t::REFRESHING_FINDNODE_TIMEOUT	= delay_t::from_sec(60);
	const delay_t	kad_kbucket_profile_t::REFRESH_IDLE_BUCKLIST_TIMEOUT	= delay_t::from_sec(60*60);
	const size_t	kad_kbucket_profile_t::NB_REPLICATION			= 20;
	const size_t	kad_kbucket_profile_t::KBUCKET_WIDTH			= 5;
	const delay_t	kad_kbucket_profile_t::DELETE_TIMEDOUT_RPC_MIN_DELAY	= delay_t::from_sec(60);	
#else
	const delay_t	kad_kbucket_profile_t::PING_RPC_TIMEOUT			= delay_t::from_sec(5);
	const delay_t	kad_kbucket_profile_t::REFRESHING_FINDNODE_TIMEOUT	= delay_t::from_sec(60);
	const delay_t	kad_kbucket_profile_t::REFRESH_IDLE_BUCKLIST_TIMEOUT	= delay_t::from_sec(60*60);
	const size_t	kad_kbucket_profile_t::NB_REPLICATION			= 20;
	const size_t	kad_kbucket_profile_t::KBUCKET_WIDTH			= 5;
	const delay_t	kad_kbucket_profile_t::DELETE_TIMEDOUT_RPC_MIN_DELAY	= delay_t::from_sec(60);
#endif
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_kbucket_profile_t::kad_kbucket_profile_t()	throw()
{
	ping_rpc_timeout		(PING_RPC_TIMEOUT);
	refreshing_findnode_timeout	(REFRESHING_FINDNODE_TIMEOUT);
	refresh_idle_bucklist_timeout	(REFRESH_IDLE_BUCKLIST_TIMEOUT);
	nb_replication			(NB_REPLICATION);
	kbucket_width			(KBUCKET_WIDTH);	
	delete_timedout_rpc_min_delay	(DELETE_TIMEDOUT_RPC_MIN_DELAY);
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
kad_err_t	kad_kbucket_profile_t::check()	const throw()
{
	// return no error
	return kad_err_t::OK;
}

NEOIP_NAMESPACE_END

