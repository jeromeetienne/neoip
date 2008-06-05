/*! \file
    \brief Definition of the \ref bstrapntudp_apps_t class

*/

/* system include */
#include <cmath>
/* local include */
#include "neoip_bstrapntudp_apps.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_clineopt_arr.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_libsess_err.hpp"
#include "neoip_lib_apps.hpp"
#include "neoip_lib_apps_helper.hpp"
#include "neoip_ipport_aview_helper.hpp"
#include "neoip_strvar_db.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
bstrapntudp_apps_t::bstrapntudp_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
}
	
/** \brief Destructor
 */
bstrapntudp_apps_t::~bstrapntudp_apps_t()	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// delete the pidfile and urlfile if needed
	if( !peer_db.empty() ){
		lib_apps_helper_t::pidfile_remove();
		lib_apps_helper_t::urlfile_remove();
	}
	// delete router_peer_t if needed
	while( !peer_db.empty() ){
		ntudp_peer_t *	ntudp_peer	= peer_db.front();
		nipmem_delete	ntudp_peer;
		peer_db.remove(ntudp_peer);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
ntudp_err_t	bstrapntudp_apps_t::start()	throw()
{
	lib_session_t *		lib_session	= lib_session_get();
	lib_apps_t *		lib_apps	= lib_session->lib_apps();
	const strvar_db_t &	arg_option	= lib_apps->arg_option();
	
	// get the number of ntudp_peer_t to launch
	DBG_ASSERT( arg_option.contain_key("nb_ntudp_peer") );
	size_t	nb_peer	= atoi(arg_option.get_first_value("nb_ntudp_peer").c_str());

	// create the ntudp_profile_t to use
	ntudp_profile_t		ntudp_profile;
	// set no_npos_watch to avoid the ntudp_npos_eval_t chicken-egg
	// - aka needing ntudp_pserver_t to become ntudp_pserver_t
	ntudp_profile.peer().no_npos_watch( true );

	// create all the needed ntudp_peer_t
	for(size_t i = 0; i < nb_peer; i++ ){
		ipport_aview_t		ipport_listen;
		ntudp_peer_t *		ntudp_peer;
		ntudp_npos_res_t	npos_res;
		ntudp_err_t		ntudp_err;
		// get the ipport_listen from the config
		ipport_listen	= ipport_aview_helper_t::udp_listen_aview_from_conf();
		// if no udp_listen_aview have been found, return an error
		if( ipport_listen.is_null() )
			return ntudp_err_t(ntudp_err_t::ERROR, "Unable to find a udp_listen_aview");		
		// deduced the ntudp_npos_res_t from the ipport_listen
		npos_res	= ntudp_npos_res_t::from_ipport_aview(ipport_listen);
		DBG_ASSERT( npos_res.completed() );
		// Start the ntudp_peer_t
		ntudp_peer	= nipmem_new ntudp_peer_t();
		ntudp_err	= ntudp_peer->set_profile(ntudp_profile)
					.start(ipport_listen, npos_res, ntudp_peerid_t::build_random());	
		if( ntudp_err.failed() )	return ntudp_err;
		// put it in the peer_db
		peer_db.push_back(ntudp_peer);
	}

	// if the arg_option DOES NOTcontains the "nodaemon" key, pass daemon
	if( !arg_option.contain_key("nodaemon") && lib_apps_helper_t::daemonize().failed() )
		return ntudp_err_t(ntudp_err_t::ERROR, "Can't daemon()");
	
	// create the pidfile
	// - NOTE: it MUST be done after the daemon() to get the proper pid
	lib_apps_helper_t::pidfile_create();
	
	// create the urlfile
	// - NOTE: it MUST be done after the daemon() to get the proper pid
	lib_apps_helper_t::urlfile_create();

	// return no error
	return ntudp_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the clineopt_arr_t for the cmdline option of the lib_session_t
 */
clineopt_arr_t	bstrapntudp_apps_t::clineopt_arr()	throw()
{
	clineopt_arr_t	clineopt_arr;
	clineopt_t	clineopt;
	// add the --nb_ntudp_peer cmdline option
	clineopt	= clineopt_t("nb_ntudp_peer", clineopt_mode_t::REQUIRED)
				.option_mode(clineopt_mode_t::REQUIRED)
				.help_string("tune the number of ntudp_peer_t to launch");
	clineopt.alias_name_db().append("n");
	clineopt_arr	+= clineopt;
	// add the --nodamon cmdline option
	clineopt	= clineopt_t("nodaemon", clineopt_mode_t::NOTHING)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("To keep the daemon in front");
	clineopt.alias_name_db().append("d");
	clineopt_arr	+= clineopt;
	// return the just built clineopt_arr_t
	return clineopt_arr;
}

NEOIP_NAMESPACE_END

