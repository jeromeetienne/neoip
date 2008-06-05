/*! \file
    \brief Definition of the \ref btcli_resp_t class

\par Brief Description
\ref btcli_resp_t handles the xmlrpc for a btcli_apps_t

\par About delete/archive action on swarm
- do a poststopped_action in the btcli_swarm
  - when a btcli_swarm reach stopped, it does the action 
  - TODO what about if the bt_ezswarm_t is rerouted before reaching stopped ?
    - at each state change, read the destination state of bt_ezswarm_t and if 
      it is not STOPPED, reset the action
- delete: remove the bt_ezswarm and remove the files associated with it
- archive: it is only for bt_ezswarm_t which are fullavail
  - it is supposed to be in completed_dir
  - it simply remove the .resumedata from the completed dir
  - if there is a .resumedata in the completed dir, the bt_ezswarm_t is launched
    else it is not
  - so when a bt_ezswarm_t reaches fullavail, it should be moved to completed_dir
    - how to do that ?
    - it is not possible to change the location of the bt_mfile_subfile_t::local_path()
      while running the bt_swarm_t
    - so it require to stop the bt_ezswarm_t, move it to completed, and restart it
      from there.
    - TODO not very good but it all i can come up with for now 
*/

/* system include */
/* local include */
#include "neoip_btcli_resp.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_btcli_ezsession_info.hpp"
#include "neoip_btcli_ezswarmid_arr.hpp"
#include "neoip_btcli_ezswarm_info_arr.hpp"
#include "neoip_xmlrpc_net.hpp"
#include "neoip_xmlrpc.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

#include "neoip_rate_sched.hpp"
#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_ezswarm.hpp"
#include "neoip_bt_ezswarm_state.hpp"
#include "neoip_btcli_swarm.hpp"

NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Declaration of the xmlrpc_resp_sfct_db_t containing the functions for nunit
 * 
 * - it is typically put in a .hpp
 */
NEOIP_XMLRPC_RESP0_SFCT_DECLARATION(btcli_rpc_sfct_db);

/** \brief Definition of the xmlrpc_resp_sfct_db_t containing the functions for nunit
 * 
 * - it is typically put in a .cpp
 */
NEOIP_XMLRPC_RESP0_SFCT_DEFINITION(btcli_rpc_sfct_db);

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			example of function exported in xmlrpc
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief get the btcli_ezswarmid_arr_t from the bt_ezsession
 */
static btcli_ezswarmid_arr_t	get_ezswarmid_arr(void *userptr)	throw()
{
	btcli_resp_t *		btcli_resp	= (btcli_resp_t*)userptr;
	bt_ezsession_t *	bt_ezsession	= btcli_resp->bt_ezsession();
	btcli_ezswarmid_arr_t	ezswarmid_arr;
	KLOG_ERR("btcli_resp=" << btcli_resp);
	// build the btcli_ezswarmid_arr_t
	ezswarmid_arr	= btcli_ezswarmid_arr_t::from_bt_ezsession(bt_ezsession);
	// return the just built btcli_ezswarmid_arr_t
	return ezswarmid_arr;
}
NEOIP_XMLRPC_RESP0_SFCT_ITEM(get_ezswarmid_arr, 0, btcli_rpc_sfct_db
				, btcli_ezswarmid_arr_t, get_ezswarmid_arr, ());


/** \brief get the btcli_ezswarm_info_t for each btcli_ezswarmid_t passed in parameter
 */
static btcli_ezswarm_info_arr_t	get_ezswarm_info_arr(void *userptr, const btcli_ezswarmid_arr_t &ezswarmid_arr)	throw()
{
	btcli_resp_t *		btcli_resp	= (btcli_resp_t*)userptr;
	bt_ezsession_t *	bt_ezsession	= btcli_resp->bt_ezsession();
	btcli_ezswarm_info_arr_t	ezswarm_info_arr;
	// go thru each btcli_ezswarmid_t
	for(size_t i = 0; i < ezswarmid_arr.size(); i++){
		const btcli_ezswarmid_t &	ezswarmid	= ezswarmid_arr[i];
		bt_ezswarm_t *			bt_ezswarm	= ezswarmid.to_bt_ezswarm(bt_ezsession);
		DBG_ASSERT( bt_ezswarm );	// TODO poor error management.. what to do here?
						// - what about having exception ? 
		// add the btcli_ezswarm_info_t for this bt_ezswarm_t in the array
		ezswarm_info_arr	+= btcli_ezswarm_info_t(bt_ezswarm);
	}
	// return the just built btcli_ezswarm_info_arr_t
	return ezswarm_info_arr;
}

NEOIP_XMLRPC_RESP0_SFCT_ITEM(get_ezswarm_info_arr, 1, btcli_rpc_sfct_db
		, btcli_ezswarm_info_arr_t, get_ezswarm_info_arr, (btcli_ezswarmid_arr_t));


/** \brief get the btcli_ezsession_t from the bt_ezsession
 */
static btcli_ezsession_info_t	get_ezsession_info(void *userptr)	throw()
{
	btcli_resp_t *		btcli_resp	= (btcli_resp_t*)userptr;
	bt_ezsession_t *	bt_ezsession	= btcli_resp->bt_ezsession();
	btcli_ezsession_info_t	ezsession_info(bt_ezsession);
	// return the just built btcli_ezsession_info_t
	return ezsession_info;
}
NEOIP_XMLRPC_RESP0_SFCT_ITEM(get_ezsession_info, 0, btcli_rpc_sfct_db
		, btcli_ezsession_info_t, get_ezsession_info, ());
		
/** \brief Set the bt_ezsession_t recv_maxrate/xmit_maxrate
 */
static bool	set_ezsession_maxrate(void *userptr, const std::string &direction
			, const std::string &rate_str)	throw()
{
	btcli_resp_t *		btcli_resp	= (btcli_resp_t*)userptr;
	bt_ezsession_t *	bt_ezsession	= btcli_resp->bt_ezsession();
	// log to debug
	KLOG_ERR("direction=" << direction << " newrate=" << rate_str);	
	// get the rate_sched_t depending on the direction
	rate_sched_t *		rate_sched	= NULL;
	if( direction == "recv")	rate_sched = bt_ezsession->recv_rsched();
	if( direction == "xmit")	rate_sched = bt_ezsession->xmit_rsched();
	
	// TODO if rate_sched is null here, notify an error to the caller with 
	// "xmit rate sched is not tunable"
	DBG_ASSERT( rate_sched );
	
	// convert the new_rate_str into a double
	double	new_rate	= double(string_t::to_uint64(rate_str));
	// set the new maxrate
	rate_sched->max_rate(new_rate);
	
	// return true all the time
	// TODO currently void is not supported
	// - i need to have a way to report the error to the xmlrpc caller
	// - how do i do that ?
	return true;
}
NEOIP_XMLRPC_RESP0_SFCT_ITEM(set_ezsession_maxrate, 2, btcli_rpc_sfct_db
		, bool, set_ezsession_maxrate, (std::string, std::string));

/** \brief Change the bt_ezswarm_t state
 * 
 * - TODO this whole stuff is bad from the bone
 *   - there is likely race and stuff in this
 *   - if there is an error, there is nothing to handle it
 * - TODO how to handle the bt_ezswarm_state_t better than that
 *   - i think having a destination state would be better
 *   - aka there is a final destination state (with likely a info string for error)
 *   - and the state moving from one state to another is done by a routing table
 *   - it is moving from state to state in a controlled fashion and thus
 *   - the possible race may be easier to control
 * - im just coming back to neoip_bt so i wont start something that big now
 *   but im just noting the issue and the idea about the potential solution 
 * - TODO moreover the goto stat is not correct
 *   - this is more for a command
 *   - e.g. if i would like to remove this ezswarm all together, like a delete
 *   - what if i want to 'archive it' e.g. put it in another directory as it is 
 *     completly downloaded
 */
static bool	swarm_command(void *userptr, const btcli_ezswarmid_arr_t &ezswarmid_arr
						, const std::string &command)	throw()
{
	btcli_resp_t *	btcli_resp	= (btcli_resp_t*)userptr;
	btcli_apps_t *	btcli_apps	= btcli_resp->btcli_apps();
	// log to debug
	KLOG_DBG("ezswarm_id_arr=" 	<< ezswarmid_arr);
	KLOG_DBG("command="		<< command);
	
	// go thru each btcli_ezswarmid_t
	for(size_t i = 0; i < ezswarmid_arr.size(); i++){
		const bt_id_t &	infohash	= ezswarmid_arr[i].infohash();
		btcli_swarm_t *	btcli_swarm	= btcli_apps->swarm_by_infohash(infohash);
		DBG_ASSERT( btcli_swarm );	// TODO poor error management.. what to do here?
		// ask the btcli_swarm to do the command
		btcli_swarm->do_command(command);
	}

	// return true all the time
	// TODO currently void is not supported
	// - i need to have a way to report the error to the xmlrpc caller
	// - how do i do that ? exception and void should be supported
	return true;
}

NEOIP_XMLRPC_RESP0_SFCT_ITEM(swarm_command, 2, btcli_rpc_sfct_db
		, bool, swarm_command, (btcli_ezswarmid_arr_t, std::string));

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
btcli_resp_t::btcli_resp_t()		throw()
{
	xmlrpc_resp	= NULL;
}

/** \brief Destructor
 */
btcli_resp_t::~btcli_resp_t()		throw()
{
	// delete the xmlrpc_resp_t if needed
	nipmem_zdelete	xmlrpc_resp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operations
 * 
 */
bt_err_t	btcli_resp_t::start(btcli_apps_t *m_btcli_apps)	throw()
{
	// copy the parameter
	this->m_btcli_apps	= m_btcli_apps;

	// start the xmlrpc_resp_t
	xmlrpc_err_t	xmlrpc_err;
	xmlrpc_resp	= nipmem_new xmlrpc_resp0_t();
	xmlrpc_err	= xmlrpc_resp->start("/btcli");
	if( xmlrpc_err.failed() )	return bt_err_from_xmlrpc(xmlrpc_err);

	// just add the list of exported function to the xmlrpc_resp_t
	// - TODO the userptr in the static is rather shitty - to refactor
	btcli_rpc_sfct_db->userptr	= this;
	xmlrpc_resp->sfct_db_arr()	+= btcli_rpc_sfct_db;

	KLOG_ERR("this=" << this);

	// return no error
	return bt_err_t::OK;
}


NEOIP_NAMESPACE_END





