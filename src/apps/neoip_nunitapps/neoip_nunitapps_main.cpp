/*! \file
    \brief main() for neoip_nunitapps
    
*/

/* system include */
/* local include */
#include "neoip_nunitapps_info.hpp"
#include "neoip_nunitapps_runner.hpp"
#include "neoip_nunit.hpp"
#include "neoip_clineopt_arr.hpp"
#include "neoip_lib_apps.hpp"
#include "neoip_lib_apps_ezplugin.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_libsess_err.hpp"
#include "neoip_clineopt_arr.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

// list of nunit_gsuite include
#include "neoip_base_nunit_gsuite.hpp"
#include "neoip_eloop_nunit_gsuite.hpp"
#include "neoip_libsess_nunit_gsuite.hpp"
#include "neoip_crypto_nunit_gsuite.hpp"
#include "neoip_compress_nunit_gsuite.hpp"
#include "neoip_file_nunit_gsuite.hpp"
#include "neoip_kad_nunit_gsuite.hpp"
#include "neoip_kad_rpc_nunit_gsuite.hpp"
#include "neoip_netutils_nunit_gsuite.hpp"
#include "neoip_xml_nunit_gsuite.hpp"
#include "neoip_xmlrpc_nunit_gsuite.hpp"
#include "neoip_xmlrpc_net_nunit_gsuite.hpp"
#include "neoip_netif_nunit_gsuite.hpp"
#include "neoip_ntudp_nunit_gsuite.hpp"
#include "neoip_inet_nunit_gsuite.hpp"
#include "neoip_upnp_nunit_gsuite.hpp"
#include "neoip_ndiag_nunit_gsuite.hpp"
#include "neoip_slay_nunit_gsuite.hpp"
#include "neoip_socket_nunit_gsuite.hpp"
#include "neoip_proto_nunit_gsuite.hpp"
#include "neoip_dnsgrab_nunit_gsuite.hpp"
#include "neoip_router_nunit_gsuite.hpp"
#include "neoip_http_nunit_gsuite.hpp"
#include "neoip_bt_core_nunit_gsuite.hpp"
#include "neoip_bt_ez_nunit_gsuite.hpp"
#include "neoip_bt_misc_nunit_gsuite.hpp"
#include "neoip_bt_plugin_nunit_gsuite.hpp"
#include "neoip_bt_cast_nunit_gsuite.hpp"
#include "neoip_flv_nunit_gsuite.hpp"

using namespace neoip;

NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   nunitapps gsuite
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void nunitapps_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// log to debug
	KLOG_DBG("enter");
	base_nunit_gsuite->populate(nunit_suite);
	eloop_nunit_gsuite->populate(nunit_suite);
	libsess_nunit_gsuite->populate(nunit_suite);
	netutils_nunit_gsuite->populate(nunit_suite);
	inet_nunit_gsuite->populate(nunit_suite);
	upnp_nunit_gsuite->populate(nunit_suite);
	ndiag_nunit_gsuite->populate(nunit_suite);
#ifdef __linux__
	dnsgrab_nunit_gsuite->populate(nunit_suite);
	router_nunit_gsuite->populate(nunit_suite);
	netif_nunit_gsuite->populate(nunit_suite);
#endif
	kad_nunit_gsuite->populate(nunit_suite);
	kad_rpc_nunit_gsuite->populate(nunit_suite);
	file_nunit_gsuite->populate(nunit_suite);
	xml_nunit_gsuite->populate(nunit_suite);
	xmlrpc_nunit_gsuite->populate(nunit_suite);
	xmlrpc_net_nunit_gsuite->populate(nunit_suite);
	crypto_nunit_gsuite->populate(nunit_suite);
	compress_nunit_gsuite->populate(nunit_suite);
	ntudp_nunit_gsuite->populate(nunit_suite);
	slay_nunit_gsuite->populate(nunit_suite);
	socket_nunit_gsuite->populate(nunit_suite);
	proto_nunit_gsuite->populate(nunit_suite);
	http_nunit_gsuite->populate(nunit_suite);
	bt_core_nunit_gsuite->populate(nunit_suite);
	bt_ez_nunit_gsuite->populate(nunit_suite);
	bt_misc_nunit_gsuite->populate(nunit_suite);
	bt_plugin_nunit_gsuite->populate(nunit_suite);
	bt_cast_nunit_gsuite->populate(nunit_suite);
	flv_nunit_gsuite->populate(nunit_suite);
}

// definition of the nunit_gsuite_t for nunitapps
NUNIT_GSUITE_DEFINITION(nunitapps_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(nunitapps_nunit_gsuite, 0, nunitapps_gsuite_fct);
NEOIP_NAMESPACE_END

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   main function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief the Main function
 */
int main(int argc, char **argv)
{
	// insert all the available plugins - MUST be done before anything else
	lib_apps_ezplugin_t::insert_all_available();
	// set the lib_apps_profile_t
	lib_apps_profile_t	profile;
	profile.lib_session().ndiag_watch().enable_upnp_watch	( false );

	// standard init
	lib_apps_t *	lib_apps	= nipmem_new lib_apps_t();
	lib_apps->set_profile(profile);
	libsess_err_t	libsess_err	= lib_apps->start(argc, argv, nunitapps_runner_t::clineopt_arr()
						, NEOIP_APPS_CANON_NAME, NEOIP_APPS_HUMAN_NAME
						, NEOIP_APPS_VERSION, NEOIP_APPS_SUMMARY
						, NEOIP_APPS_LONGDESC, NEOIP_APPS_TYPE);
	if( libsess_err.failed() ){
		KLOG_ERR("cant start lib_apps_t due to " << libsess_err);
		return -1;
	}
	if( libsess_err.reason() == "DONTLAUNCHAPPS" )	return 0;

	// build the pattern the nunit_path_t to run
	nunit_path_t	path_pattern;
	// get the pattern from the command
	if( lib_apps->arg_remain().size() > 0 )	path_pattern	= lib_apps->arg_remain()[0];

//	path_pattern	= "all/eloop";
//	path_pattern	= "all/base";
//	path_pattern	= "all/base/slotpool_t";
//	path_pattern	= "all/base/tokeep_check_t";
//	path_pattern	= "all/base/event_hook_t";
//	path_pattern	= "all/base/expireset";
//	path_pattern	= "all/base/item_arr";
//	path_pattern	= "all/base/bitfield";
//	path_pattern	= "all/base/dvar";
//	path_pattern	= "all/base/object_slotid";
//	path_pattern	= "all/base/delaygen";
//	path_pattern	= "all/base/interval";
//	path_pattern	= "all/base/pkttype";
//	path_pattern	= "all/base/ptr";
//	path_pattern	= "all/base/profile";
//	path_pattern	= "all/base/strtype";
//	path_pattern	= "all/base/errtype";
//	path_pattern	= "all/base/idleop";
//	path_pattern	= "all/base/strvar_db";
//	path_pattern	= "all/base/bytearray2";
//	path_pattern	= "all/base/datum2";
//	path_pattern	= "all/base/serial";
//	path_pattern	= "all/base/serial2";
//	path_pattern	= "all/base/serial2/inheritance";
//	path_pattern	= "all/base/delay2";
//	path_pattern	= "all/base/bitflag";
//	path_pattern	= "all/base/string";
//	path_pattern	= "all/libsess/clineopt";
//	path_pattern	= "all/compress";
//	path_pattern	= "all/kad";
//	path_pattern	= "all/kad/kad_bstrap_src_t";
//	path_pattern	= "all/kad/kad_rec_src_t";
//	path_pattern	= "all/kad/kad2_helper";
//	path_pattern	= "all/kad/kad_client_rpc";
//	path_pattern	= "all/kad/cmd";
//	path_pattern	= "all/crypto";
//	path_pattern	= "all/crypto/x509";
//	path_pattern	= "all/crypto/skey_auth";
//	path_pattern	= "all/netutils";
//	path_pattern	= "all/netutils/bfilter_t";
//	path_pattern	= "all/netutils/gen_id";
//	path_pattern	= "all/netutils/bencode";
//	path_pattern	= "all/netutils/mimediag";
//	path_pattern	= "all/netutils/rate_sched";
//	path_pattern	= "all/netutils/rate_estim";
//	path_pattern	= "all/ntudp";
//	path_pattern	= "all/ntudp/socket_direct";
//	path_pattern	= "all/ntudp/socket_estarelay";
//	path_pattern	= "all/ntudp/socket_reverse";
//	path_pattern	= "all/ntudp/socket_error";
//	path_pattern	= "all/ntudp/aview_pool";
//	path_pattern	= "all/ntudp/npos_eval";
//	path_pattern	= "all/ntudp/pserver_pool";
//	path_pattern	= "all/ntudp/itor_retry";
//	path_pattern	= "all/ntudp/client";
//	path_pattern	= "all/netif";
//	path_pattern	= "all/netif/netif_vdev";
//	path_pattern	= "all/netif/netif_stat";
//	path_pattern	= "all/netif/netif_stat_watch";
//	path_pattern	= "all/inet";
//	path_pattern	= "all/inet/ipport_strlist_src";
//	path_pattern	= "all/inet/nslan";
//	path_pattern	= "all/inet/nslan_rec_src";
//	path_pattern	= "all/inet/ipport_addr";
//	path_pattern	= "all/inet/ip_netaddr";
//	path_pattern	= "all/inet/tcp_client";
//	path_pattern	= "all/neoip_socket";
//	path_pattern	= "all/neoip_socket/discard/general";
//	path_pattern	= "all/proto";
//	path_pattern	= "all/dnsgrab";
//	path_pattern	= "all/router";
//	path_pattern	= "all/router/peer";
//	path_pattern	= "all/bt";
//	path_pattern	= "all/bt/mfile";
//	path_pattern	= "all/bt/swarm";
//	path_pattern	= "all/bt/ezswarm";
//	path_pattern	= "all/bt/oload";
//	path_pattern	= "all/bt/alloc";
//	path_pattern	= "all/bt/check";
//	path_pattern	= "all/bt/mfile_cpuhash";
//	path_pattern	= "all/bt/peerid_parser";
//	path_pattern	= "all/bt/offpiece";
//	path_pattern	= "all/bt/pieceavail";
//	path_pattern	= "all/bt/globavail";
//	path_pattern	= "all/bt/pselect";
//	path_pattern	= "all/bt/pieceprec";
//	path_pattern	= "all/bt/mfile";
//	path_pattern	= "all/bt/tracker_client";
//	path_pattern	= "all/bt/tracket_server";
//	path_pattern	= "all/file";
//	path_pattern	= "all/file/path";
//	path_pattern	= "all/file/sio";
//	path_pattern	= "all/file/aio";
//	path_pattern	= "all/file/aio/awrite";
//	path_pattern	= "all/file/utils";
//	path_pattern	= "all/file/range";
//	path_pattern	= "all/xml";
//	path_pattern	= "all/xml/parse";
//	path_pattern	= "all/http";
//	path_pattern	= "all/http/uri";
//	path_pattern	= "all/http/reqhd";
//	path_pattern	= "all/http/rephd";
//	path_pattern	= "all/http/listener";
//	path_pattern	= "all/http/client";
//	path_pattern	= "all/http/sclient";
//	path_pattern	= "all/http/nested_uri";

	// append wildcard to allow any subtest
	// - NOTE: this kind of stuff could be directly included in nunit_path_t
	path_pattern	/= "*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*/*";
	KLOG_DBG("path_pattern="<< path_pattern);
	
	// create the nunit_utest
	nunitapps_runner_t *	nunitapps_runner;	
	nunitapps_runner	= nipmem_new nunitapps_runner_t(path_pattern, nunitapps_nunit_gsuite);

	// log the event
	KLOG_ERR("NeoIP Unit Testing Application Initialized");
	// Start the event loop - exit only when the apps goes off
	lib_session_get()->loop_run();

	// Destroy the nunitapps_runner
	if( nunitapps_runner )	nipmem_delete	nunitapps_runner;

	// standard deinit
	nipmem_zdelete	lib_apps;
	
	// return no error
	return 0;
}


