/*! \file
    \brief Class to handle the bt_http_ecnx_pool_t

*/

/* system include */
/* local include */
#include "neoip_bt_http_ecnx_pool.hpp"
#include "neoip_bt_http_ecnx_cnx.hpp"
#include "neoip_bt_http_ecnx_herr.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_http_client_pool.hpp"
#include "neoip_bt_swarm_sched.hpp"		// only to get the callback for bt_http_ecnx_cnx_t
#include "neoip_bt_swarm_sched_ecnx.hpp"	// only to get the callback for bt_http_ecnx_cnx_t#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_http_ecnx_pool_t::bt_http_ecnx_pool_t()		throw()
{
	// zero some fields
	bt_swarm	= NULL;
	http_client_pool= NULL;
}

/** \brief Destructor
 */
bt_http_ecnx_pool_t::~bt_http_ecnx_pool_t()		throw()
{
	// delete the http_client_pool_t if needed
	nipmem_zdelete	http_client_pool;
	// close all pending bt_http_ecnx_herr_t
	while( !ecnx_herr_db.empty() )	nipmem_delete ecnx_herr_db.begin()->second;	
	// close all pending bt_http_ecnx_cnx_t
	while( !ecnx_cnx_db.empty() )	nipmem_delete ecnx_cnx_db.front();		
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
bt_http_ecnx_pool_t &bt_http_ecnx_pool_t::set_profile(const bt_http_ecnx_pool_profile_t &profile) throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == bt_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
bt_err_t	bt_http_ecnx_pool_t::start(bt_swarm_t *bt_swarm)	throw()
{
	bt_err_t	bt_err;
	// copy the parameter
	this->bt_swarm	= bt_swarm;
	// sanity check - the bt_http_ecnx_cnx_profile_t::req_queue_minlen MUST 
	//                BE >= than bt_swarm_profile_t::xmit_req_maxlen
	DBG_ASSERT( profile.ecnx_cnx().req_queue_minlen() >= bt_swarm->profile().xmit_req_maxlen() );

	// start the http_client_pool_t
	http_err_t	http_err;
	http_client_pool= nipmem_new http_client_pool_t();
	http_err	= http_client_pool->start();
	if( http_err.failed() )	return bt_err_from_http(http_err);

	// build the m_hostport_db
	// - aka a set of all distinct hostport_str present in all the bt_mfile_subfile_t::uri_arr()
	const bt_mfile_t &		bt_mfile	= bt_swarm->get_mfile();
	const bt_mfile_subfile_arr_t &	subfile_arr	= bt_mfile.subfile_arr();
	for(size_t subfile_idx = 0; subfile_idx < subfile_arr.size(); subfile_idx++){
		const http_uri_arr_t &	uri_arr		= subfile_arr[subfile_idx].uri_arr();
		// build the filtered_arr containing only the http_uri_t with the same hostport_str
		for(size_t i = 0; i < uri_arr.size(); i++){
			std::string	hostport_str	= uri_arr[i].hostport_str();
			// if this hostport_str is already in the m_hostport_db, skip it
			if( m_hostport_db.find(hostport_str) != m_hostport_db.end() )	continue;
			// add it to the m_hostport_db
			m_hostport_db.insert(hostport_str);
		}
	}

	// start a bt_http_ecnx_cnx_t for each hostport_str of the m_hostport_db
	std::set<std::string>::iterator	iter;
	for(iter = m_hostport_db.begin(); iter != m_hostport_db.end(); iter++){
		const std::string &	hostport_str	= *iter;
		bt_err	= launch_ecnx_cnx_if_needed(hostport_str);
		if( bt_err.failed() )	return bt_err;
	}

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch bt_http_ecnx_cnx_t for hostport_str until profile.concurrent_max_cnx() are up 
 */
bt_err_t bt_http_ecnx_pool_t::launch_ecnx_cnx_if_needed(const std::string &hostport_str)throw()
{
	size_t	nb_cnx_tolaunch	= profile.concurrent_cnx_max();
	
	// decrease nb_cnx_tolanuch for each current bt_http_ecnx_cnx_t for this hostport_str
	std::list<bt_http_ecnx_cnx_t *>::iterator	iter;
	for(iter = ecnx_cnx_db.begin(); iter != ecnx_cnx_db.end(); iter++){
		bt_http_ecnx_cnx_t *	ecnx_cnx	= *iter;
		// if this bt_http_ecnx_cnx_t doesnt match hostport_str, goto the next
		if( ecnx_cnx->hostport_str() != hostport_str )	continue;
		// sanity check - the counter MUST NEVER reach a negative value 
		DBG_ASSERT( nb_cnx_tolaunch > 0 );
		// update the counter
		nb_cnx_tolaunch--;
	}
	
	// start a bt_http_ecnx_cnx_t for each hostport_str of the m_hostport_db
	for(size_t i = 0; i < nb_cnx_tolaunch; i++){
		bt_ecnx_vapi_cb_t *	bt_ecnx_vapi_cb	= bt_swarm->swarm_sched()->sched_ecnx();
		bt_http_ecnx_cnx_t *	ecnx_cnx;
		bt_err_t		bt_err;
		// create a bt_http_ecnx_cnx_t for this hostport_str
		ecnx_cnx	= nipmem_new bt_http_ecnx_cnx_t();
		bt_err		= ecnx_cnx->set_profile(profile.ecnx_cnx())
					.start(this, hostport_str, bt_ecnx_vapi_cb, NULL);
		if( bt_err.failed() )	return bt_err;
	}

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    management of bt_http_ecnx_herr_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief DoLink this bt_http_ecnx_herr_t to the pool
 */
void	bt_http_ecnx_pool_t::ecnx_herr_dolink(bt_http_ecnx_herr_t *ecnx_herr)	throw()
{
	// add it to the ecnx_herr_db
	bool succeed = ecnx_herr_db.insert(std::make_pair(ecnx_herr->get_hostport_str(), ecnx_herr)).second;
	DBG_ASSERT( succeed );	
}

/** \brief UnLink this bt_http_ecnx_herr_t to the pool
 */
void	bt_http_ecnx_pool_t::ecnx_herr_unlink(bt_http_ecnx_herr_t *ecnx_herr)	throw()
{
	// sanity check - the bt_http_ecnx_herr_t MUST be present in the ecnx_herr_db
	DBG_ASSERT( ecnx_herr_db.find(ecnx_herr->get_hostport_str()) != ecnx_herr_db.end() );
	// remove it from the ecnx_herr_db
	ecnx_herr_db.erase(ecnx_herr->get_hostport_str());
}

/** \brief Return a bt_http_ecnx_herr_t pointer matching the hostport_str, or NULL if none matches
 */
bt_http_ecnx_herr_t *	bt_http_ecnx_pool_t::ecnx_herr_by_hostport_str(const std::string &hostport_str) throw()
{
	std::map<std::string, bt_http_ecnx_herr_t *>::iterator	iter;
	// try to find this hostport_str in the ecnx_herr_db
	iter	= ecnx_herr_db.find(hostport_str);
	// if none has been found, return NULL
	if( iter == ecnx_herr_db.end() )	return NULL;
	// else return its bt_http_ecnx_herr_t pointer
	return iter->second;
}


NEOIP_NAMESPACE_END





