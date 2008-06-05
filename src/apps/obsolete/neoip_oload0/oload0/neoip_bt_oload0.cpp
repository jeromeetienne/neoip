/*! \file
    \brief Definition of the \ref bt_oload0_t

\par BUG
- if i point a vlc toward a video in http static, there is a core dump in neoip-oload0

\par Note about complete refactoring
- TODO i think i should completly reimplement this
  - the bt_httpo_t is half backed
  - bt_oload0_t doesnt uses the bt_ezsession_t
    - but its owns implementatoin
    - is it possible for bt_oload0 to use bt_ezsession_t and bt_ezswarm?
    - it would be nice to... as it would avoid a special case
    - if it is not possible right now, try to do the modification for that
  - it doesnt uses bt_lnk2mfile_t
    - but its own implementation
  - in order there is some issue with the pid too
- in order to be full usefull the neoip-oload0 should keep the datas after they
  have been downloaded
  - so the data available locally should be shared even after the download
  - currently it is only sharing data *during* the download.
    - not completly useless but not optimal
  - unfortunatly how to do this ? as it suppose for other downloader to be able
    to find out that the local peer has data which may interest them
    - always this bootstrap matter
    - this implies to be able to publish a large number of file so the publication
      overhead should be low
  - the publication overhead of http tracker or dht tracker are not negligible 
    at all, as we talk here about a large number of file
  - the best would be a nsmesh, but it is currently not implemented
  - a extreme usecase would be each installed ubuntu sharing its 
    /var/cache/apt/archives (where all the .deb are cached) and have apt-get
    to go thru the neoip-oload0 to take advantage of it.
    - this would implies at least 100files per ubuntu install
    
\par TODO about userid
- neoip-oload0 has no requirement to be root in itself
  - but currently it is handled as a system daemon and will run as root
  - writing in /var/run/neoip-oload0.pid requires root, but this is really not important
  - being root may expose some security vulnerability too
  - running as a system daemon is easy for now
  - TODO to fix this

*/

/* system include */
/* local include */
#include "neoip_bt_oload0.hpp"
#include "neoip_bt_oload0_herr.hpp"
#include "neoip_bt_oload0_swarm.hpp"
#include "neoip_bt_oload0_mlink.hpp"
#include "neoip_bt_oload0_stat.hpp"
#include "neoip_bt_oload0_torr.hpp"

#include "neoip_bt_session.hpp"
#include "neoip_bt_httpo.hpp"
#include "neoip_http_listener.hpp"
#include "neoip_kad.hpp"
#include "neoip_kad_publish.hpp"
#include "neoip_udp_vresp.hpp"

#include "neoip_clineopt_arr.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_libsess_err.hpp"
#include "neoip_lib_apps.hpp"
#include "neoip_lib_apps_helper.hpp"
#include "neoip_strvar_db.hpp"
#include "neoip_strvar_helper.hpp"
#include "neoip_ipport_aview_helper.hpp"
#include "neoip_file.hpp"
#include "neoip_file_sio.hpp"

#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_oload0_t::bt_oload0_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	http_listener	= NULL;
	bt_session	= NULL;
	bt_httpo	= NULL;
	// zero stuff for peersrc_kad
	udp_vresp	= NULL;
	kad_listener	= NULL;
	kad_peer	= NULL;	

	oload0_herr	= NULL;

	oload0_mlink	= NULL;
	oload0_stat	= NULL;
	oload0_torr	= NULL;
}

/** \brief Destructor
 */
bt_oload0_t::~bt_oload0_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the pidfile and urlfile if needed
	if( http_listener ){
		lib_apps_helper_t::pidfile_remove();
		lib_apps_helper_t::urlfile_remove();
		file_utils_t::remove_directory(io_pfile_dirpath(), file_utils_t::DO_RECURSION);
	}

	// delete all the oload0_swarm_db if needed
	while( !oload0_swarm_db.empty() )	nipmem_delete oload0_swarm_db.front();	

	// delete the bt_oload0_mlink_t if needed
	nipmem_zdelete oload0_mlink;
	// delete the bt_oload0_torr_t if needed
	nipmem_zdelete oload0_torr;
	// delete the bt_oload0_stat_t if needed
	nipmem_zdelete oload0_stat;

	// delete the bt_oload0_herr_t if needed
	nipmem_zdelete oload0_herr;
	
	// delete the kad_peer_t if needed
	nipmem_zdelete	kad_peer;
	// delete the kad_listener_t if needed
	nipmem_zdelete	kad_listener;	
	// delete the udp_vresp_t if needed
	nipmem_zdelete	udp_vresp;

	// delete the bt_httpo_t if needed
	nipmem_zdelete bt_httpo;
	// delete the bt_session_t if needed
	nipmem_zdelete bt_session;
	// delete the http_listener if needed
	nipmem_zdelete http_listener;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_oload0_t::start()						throw()
{
	lib_session_t *		lib_session	= lib_session_get();
	lib_apps_t *		lib_apps	= lib_session->lib_apps();
	const strvar_db_t &	arg_option	= lib_apps->arg_option();
	file_path_t		config_path	= lib_session->conf_rootdir() / "oload0";
	const strvar_db_t &	session_conf	= lib_session_get()->session_conf();
	bt_err_t		bt_err;
	// log to debug
	KLOG_WARN("enter");
	
	// load the configuration file for the router
	strvar_db_t	oload0_conf	= strvar_helper_t::from_file(config_path / "neoip_oload0.conf");

	// get the oload0_listen_ipport
	ipport_addr_t	listener_ipport	= oload0_conf.get_first_value("http_listener_ipport", "127.0.0.1:4242");

	// build the resp_arg for the http_listener_t 
	socket_resp_arg_t	resp_arg;
	resp_arg	= socket_resp_arg_t().profile(socket_profile_t(socket_domain_t::TCP))
					.domain(socket_domain_t::TCP).type(socket_type_t::STREAM)
					.listen_addr("tcp://" + listener_ipport.to_string());
	// start the http_listener_t
	http_err_t	http_err;
	http_listener	= nipmem_new http_listener_t();
	http_err	= http_listener->start(resp_arg);
	if( http_err.failed() )		return bt_err_from_http(http_err);


	// get the tcp_listen_aview from the config
	ipport_aview_t	tcp_listen_aview;
	tcp_listen_aview= ipport_aview_helper_t::tcp_listen_aview_from_conf();
	// sanity check - the tcp_listen_aview local view MUST NOT be null
	DBG_ASSERT( !tcp_listen_aview.is_null() );
	
	// start the bt_session_t
	bt_session	= nipmem_new bt_session_t();
	bt_err		= bt_session->start(tcp_listen_aview);
	if( bt_err.failed() )		return bt_err;

	// start the bt_httpo_t
	bt_httpo	= nipmem_new bt_httpo_t();
	bt_err		= bt_httpo->start(http_listener);
	if( bt_err.failed() )		return bt_err;

	// get the udp_listen_aview from the config
	ipport_aview_t	udp_listen_aview;
	udp_listen_aview= ipport_aview_helper_t::udp_listen_aview_from_conf();
	// sanity check - the udp_listen_aview MUST NOT be null
	DBG_ASSERT( !udp_listen_aview.is_null() );
	
// init stuff for bt_peersrc_kad_t
	// create the udp_vresp
	inet_err_t	inet_err;
	udp_vresp	= nipmem_new udp_vresp_t();
	inet_err 	= udp_vresp->start(udp_listen_aview.lview());
	// if the udp_vresp_t failed, report the error
	if( inet_err.failed() )		return bt_err_from_inet(inet_err);
	// init kad_listener
	ipport_addr_t	nslan_addr	= session_conf.get_first_value("nslan_ipport", "255.255.255.255:7777");
	kad_err_t	kad_err;
	kad_listener	= nipmem_new kad_listener_t();
	kad_err 	= kad_listener->start(nslan_addr, udp_vresp, udp_listen_aview);
	if( kad_err.failed() )		return bt_err_from_kad(kad_err);
	// set some variables for kad_peer_t
	kad_realmid_t	kad_realmid	= "bt_oload0_t kad realmid";
	kad_peerid_t	kad_peerid	= kad_peerid_t::build_random();
	kad_peer	= nipmem_new kad_peer_t();
	kad_err		= kad_peer->start(kad_listener, kad_realmid, kad_peerid);
	if( kad_err.failed() )		return bt_err_from_kad(kad_err);

	// start the bt_oload0_herr_t 
	oload0_herr	= nipmem_new bt_oload0_herr_t();
	bt_err		= oload0_herr->start(this);
	if( bt_err.failed() )		return bt_err;	

	// start the bt_oload0_mlink_t
	oload0_mlink	= nipmem_new bt_oload0_mlink_t();
	bt_err		= oload0_mlink->start(this);
	if( bt_err.failed() )		return bt_err;

	// start the bt_oload0_stat_t
	oload0_stat	= nipmem_new bt_oload0_stat_t();
	bt_err		= oload0_stat->start(this);
	if( bt_err.failed() )		return bt_err;

	// start the bt_oload0_torr_t
	oload0_torr	= nipmem_new bt_oload0_torr_t();
	bt_err		= oload0_torr->start(this);
	if( bt_err.failed() )		return bt_err;

	// init the static link http - to ease access while debugging
	wikidbg_obj_add_static_page("/bt_oload0_" + bt_session->local_peerid().to_string());

	// if the arg_option DOES NOT contains the "nodaemon" key, pass daemon
	// - NOTE: be carefull this change the pid
	if( !arg_option.contain_key("nodaemon") && lib_apps_helper_t::daemonize().failed() )
		return bt_err_t(bt_err_t::ERROR, "Can't daemon()");

	// create the pidfile
	// - NOTE: it MUST be done after the daemon() to get the proper pid
	lib_apps_helper_t::pidfile_create();

	// create the urlfile
	// - NOTE: it MUST be done after the daemon() to get the proper pid
	lib_apps_helper_t::urlfile_create();

	// create the temporary directory io_pfile_dirpath()
	// - this should be created by bt_alloc_t
	file_err_t	file_err;
	file_err	= file_utils_t::create_directory(io_pfile_dirpath(), file_utils_t::NO_RECURSION);
	if( file_err.failed() )	return bt_err_from_file(file_err);

	// return no error
	return bt_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a pointer to a bt_oload0_swarm_t matching the nested_uri, or NULL if none matches
 */
bt_oload0_swarm_t *	bt_oload0_t::swarm_by_nested_uri(const http_uri_t &nested_uri) const throw()
{
	std::list<bt_oload0_swarm_t *>::const_iterator	iter;
	// go thru the whole oload0_swarm_db
	for(iter = oload0_swarm_db.begin(); iter != oload0_swarm_db.end(); iter++){
		bt_oload0_swarm_t *	oload0_swarm	= *iter;
		// if this bt_oload0_swarm_t matches this nested_uri, return its pointer
		if( oload0_swarm->get_nested_uri() == nested_uri )	return oload0_swarm;
	}
	// if this point is reached, no matches have been found
	return NULL;
}

/** \brief Return the io_pfile_dirpath
 * 
 * - aka the directory path used by bt_io_pfile_t to store the temporary files
 */
file_path_t	bt_oload0_t::io_pfile_dirpath()	const throw()
{
	file_path_t	dirpath;
	// build the dirpath
	dirpath		= "/tmp/.neoip-oload0." + OSTREAMSTR(lib_apps_helper_t::current_pid()) + ".temp";
	// return the just built dirpath
	return dirpath;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the clineopt_arr_t for the cmdline option of the lib_session_t
 */
clineopt_arr_t	bt_oload0_t::clineopt_arr()	throw()
{
	clineopt_arr_t	clineopt_arr;
	clineopt_t	clineopt;
	// add the --nodaemon cmdline option
	clineopt	= clineopt_t("nodaemon", clineopt_mode_t::NOTHING)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("To keep the daemon in front");
	clineopt.alias_name_db().append("d");
	clineopt_arr	+= clineopt;
	// return the just built clineopt_arr_t
	return clineopt_arr;
}


NEOIP_NAMESPACE_END;




