/*! \file
    \brief Definition of the \ref kad_db_profile_t

*/


/* system include */
/* local include */
#include "neoip_kad_db_profile.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref kad_db_profile_t constant
const bool	kad_db_profile_t::KEYID_NODUPS_OK		= false;
const delay_t	kad_db_profile_t::REPLICATE_FINDNODE_RPC_TIMEOUT= delay_t::from_sec(60);
const delay_t	kad_db_profile_t::REPLICATE_STORE_RPC_TIMEOUT	= delay_t::from_sec(60);
const delay_t	kad_db_profile_t::REPUBLISH_STORE_TIMEOUT	= delay_t::from_sec(4*60);
const size_t	kad_db_profile_t::LOCAL_DB_MAX_LEN		= 0;
const size_t	kad_db_profile_t::LOCAL_REC_MAX_LEN		= 0;
const delay_t	kad_db_profile_t::LOCAL_REC_MAX_TTL		= delay_t::from_sec(24*60*60);
const delay_t	kad_db_profile_t::LOCAL_REC_REPUBLISH_PERIOD	= delay_t(delay_t::INFINITE_VAL);
const size_t	kad_db_profile_t::REMOTE_DB_MAX_LEN		= 0;
const size_t	kad_db_profile_t::REMOTE_REC_MAX_LEN		= 0;
const delay_t	kad_db_profile_t::REMOTE_REC_MAX_TTL		= delay_t::from_sec(24*60*60);
const delay_t	kad_db_profile_t::REMOTE_REC_REPUBLISH_PERIOD	= delay_t::from_sec(60*60);	
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_db_profile_t::kad_db_profile_t()	throw()
{
	// NOTE: here the value are not initialized at all as only the values
	// from build_local_db/build_remote_db should be used
	// - would be nice to put some sanity check on this behaviour
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                              build default profile
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return the default profile for the local database
 */
kad_db_profile_t kad_db_profile_t::build_local_db()	throw()
{
	kad_db_profile_t	profile;
	// put the value specific to the local_db
	profile.keyid_nodups_ok			(KEYID_NODUPS_OK);
	profile.replicate_findnode_rpc_timeout	(REPLICATE_FINDNODE_RPC_TIMEOUT);
	profile.replicate_store_rpc_timeout	(REPLICATE_STORE_RPC_TIMEOUT);
	profile.republish_store_timeout		(REPUBLISH_STORE_TIMEOUT);
	profile.db_max_len			(LOCAL_DB_MAX_LEN);
	profile.rec_max_len			(LOCAL_REC_MAX_LEN);
	profile.rec_max_ttl			(LOCAL_REC_MAX_TTL);
	profile.rec_republish_period		(LOCAL_REC_REPUBLISH_PERIOD);
	// return the default profile for the local database
	return profile;
}

/** \brief return the default profile for the remote database
 */
kad_db_profile_t kad_db_profile_t::build_remote_db()	throw()
{
	kad_db_profile_t	profile;
	// put the value specific to the remote_db
	profile.keyid_nodups_ok			(KEYID_NODUPS_OK);
	profile.replicate_findnode_rpc_timeout	(REPLICATE_FINDNODE_RPC_TIMEOUT);
	profile.replicate_store_rpc_timeout	(REPLICATE_STORE_RPC_TIMEOUT);
	profile.republish_store_timeout		(REPUBLISH_STORE_TIMEOUT);
	profile.db_max_len			(REMOTE_DB_MAX_LEN);
	profile.rec_max_len			(REMOTE_REC_MAX_LEN);
	profile.rec_max_ttl			(REMOTE_REC_MAX_TTL);
	profile.rec_republish_period		(REMOTE_REC_REPUBLISH_PERIOD);
	// return the default profile for the remote database
	return profile;
}

//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	
//                     check function
//////////////////////////////////////////////////////////////////////////////	
//////////////////////////////////////////////////////////////////////////////	

/** \brief Check the validity of the profile
 */
kad_err_t	kad_db_profile_t::check()	const throw()
{
	// return no error
	return kad_err_t::OK;
}

NEOIP_NAMESPACE_END

