/*! \file
    \brief Definition of the \ref casto_apps_t


\par measuring latency of a video stream
- use a video stream from a webcam
-# get a digital clock on the screen
-# put the output of neoip-casto close to it
-# point the camera to get both and read the webcam with neoip-casti
-# get a snapshot
- reading the snapshot give you the digital clock at 2 level. just a 
  subtraction between them give the latency
- the "digital clock" in bash
  - while true; do date +"%M.%S.%N"; sleep 0.1;done

\par Possible Improvement
- to include peersrc in the bt_cast_mdata_t
  - this would allows casto to have a faster establishement as it would
    remove the latency of the first peersrc requests
  - moreover one could choose to put the peersrc which are tuned to help
    the newly joined peers.
    - e.g. bt_relay_t peers tuned for that

*/

/* system include */
/* local include */
#include "neoip_casto_apps.hpp"
#include "neoip_casto_info.hpp"		// NOTE: needed here for neoip-webok
#include "neoip_casto_itor.hpp"
#include "neoip_casto_swarm.hpp"

#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_ezsession_opt.hpp"
#include "neoip_bt_id.hpp"
#include "neoip_bt_mfile.hpp"

#include "neoip_bt_cast_prefetch.hpp"

#include "neoip_http_listener.hpp"
#include "neoip_http_resp_mode.hpp"
#include "neoip_http_uri.hpp"

#include "neoip_apps_httpdetect.hpp"

#include "neoip_bt_httpo_listener.hpp"
#include "neoip_bt_httpo_resp.hpp"
#include "neoip_bt_httpo_full.hpp"
#include "neoip_bt_httpo_event.hpp"

#include "neoip_ipport_addr.hpp"
#include "neoip_ipport_addr_helper.hpp"
#include "neoip_ipport_aview_helper.hpp"

#include "neoip_lib_apps.hpp"
#include "neoip_lib_apps_helper.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_strvar_db.hpp"
#include "neoip_strvar_helper.hpp"
#include "neoip_clineopt_arr.hpp"
#include "neoip_file.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
casto_apps_t::casto_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	m_http_listener		= NULL;
	m_httpo_listener	= NULL;
	m_httpo_resp		= NULL;
	m_apps_httpdetect	= NULL;
	m_cast_prefetch		= NULL;
	m_bt_ezsession		= NULL;
}

/** \brief Destructor
 */
casto_apps_t::~casto_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete all the itor_db if needed
	while( !itor_db.empty() )	nipmem_delete itor_db.front();	
	// delete all the swarm_db if needed
	while( !swarm_db.empty() )	nipmem_delete swarm_db.front();	
	// delete the bt_ezsession_t if needed
	nipmem_zdelete	m_bt_ezsession;
	// delete the pidfile and urlfile if needed
	if( m_http_listener ){
		lib_apps_helper_t::pidfile_remove();
		lib_apps_helper_t::urlfile_remove();
		file_utils_t::remove_directory(io_pfile_dirpath(), file_utils_t::DO_RECURSION);
	}
	// delete the bt_httpo_resp_t if needed
	nipmem_zdelete	m_httpo_resp;
	// delete the bt_httpo_listener_t if needed
	nipmem_zdelete	m_httpo_listener;
	// delete the m_cast_prefetch if needed
	nipmem_zdelete	m_cast_prefetch;
	// delete the m_apps_httpdetect if needed
	nipmem_zdelete	m_apps_httpdetect;
	// delete the http_listener if needed
	nipmem_zdelete	m_http_listener;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	casto_apps_t::start()						throw()
{
	lib_session_t *		lib_session	= lib_session_get();
	lib_apps_t *		lib_apps	= lib_session->lib_apps();
	const strvar_db_t &	arg_option	= lib_apps->arg_option();
	file_path_t		config_path	= lib_session->conf_rootdir() / "casto";
	bt_err_t		bt_err;
	// log to debug
	KLOG_WARN("enter");

	// init the static link http - to ease access while debugging
	wikidbg_obj_add_static_page("/casto_apps_" + OSTREAMSTR(this));
	
	// load the configuration file for the router
	strvar_db_t casto_conf	= strvar_helper_t::from_file(config_path / "neoip_casto.conf");
	
	// read some defaults value fromt the casto_conf - for future reference
	m_dfl_mdata_srv_uri	= casto_conf.get_first_value("dfl_mdata_srv_uri");
	
	// start the bt_httpo_listener_t
	bt_err		= launch_httpo(casto_conf);
	if( bt_err.failed() )		return bt_err;

	// Launch the apps_httpdetect_t - use directly neoip_*_info.hpp #define 
	// - cant use libapps->canon_name() because it would be wrong for neoip-webok
	libsess_err_t	libsess_err;
	m_apps_httpdetect = nipmem_new apps_httpdetect_t();
	libsess_err	= m_apps_httpdetect->start(http_listener(), NEOIP_APPS_CANON_NAME
							, NEOIP_APPS_VERSION);
	if( libsess_err.failed() )	return bt_err_t(bt_err_t::ERROR, libsess_err.to_string());
	
	// Launch the bt_cast_prefetch_t
	m_cast_prefetch	= nipmem_new bt_cast_prefetch_t();
	bt_err		= m_cast_prefetch->start(http_listener());
	if( bt_err.failed() )		return bt_err;
	
	// start the bt_ezsession_t
	bt_err		= launch_ezsession(casto_conf);
	if( bt_err.failed() )		return bt_err;
	
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

	// set the io_pfile_dirpath name
	m_io_pfile_dirpath	= lib_session->temp_rootdir() / (lib_apps->canon_name()+"_pfile.");
	m_io_pfile_dirpath	= file_utils_t::get_temp_path(m_io_pfile_dirpath);
	// create the temporary directory io_pfile_dirpath()
	// - TODO this should be created by bt_alloc_t?
	// - all this stuff about bt_alloc_t and bt_io_pfile_t is unclear
	// - there is the deletion of this directory in the casto_apps_t dtor
	file_err_t	file_err;
	file_err	= file_utils_t::create_directory(io_pfile_dirpath(), file_utils_t::NO_RECURSION);
	if( file_err.failed() )	return bt_err_from_file(file_err);
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			subpart launcher
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor the bt_httpo_listener_t and http_listener_t
 */
bt_err_t	casto_apps_t::launch_httpo(const strvar_db_t &casto_conf)	throw()
{
	bt_err_t	bt_err;
	// log to debug
	KLOG_WARN("enter");

	// get the listen_ipport from the casto_conf
	std::string	port_min_str	= casto_conf.get_first_value("http_listener_port_min", "4242");
	std::string	port_max_str	= casto_conf.get_first_value("http_listener_port_max", "4242");
	ip_addr_t	listener_addr	= casto_conf.get_first_value("http_listener_addr", "127.0.0.1");
	uint16_t	listener_portmin= string_t::to_uint16(port_min_str);
	uint16_t	listener_portmax= string_t::to_uint16(port_max_str);
	// try to find a free one within the configured range
	ipport_addr_t	listener_ipport	= ipport_addr_helper_t::find_avail_listen_addr(listener_addr
						, listener_portmin, listener_portmax);
	if( listener_ipport.is_null() )	return bt_err_t(bt_err_t::ERROR, "no free port for http_listener");

	// build the resp_arg for the http_listener_t 
	socket_resp_arg_t	resp_arg;
	resp_arg	= socket_resp_arg_t().profile(socket_profile_t(socket_domain_t::TCP))
					.domain(socket_domain_t::TCP).type(socket_type_t::STREAM)
					.listen_addr("tcp://" + listener_ipport.to_string());
	// start the http_listener_t
	http_err_t	http_err;
	m_http_listener	= nipmem_new http_listener_t();
	http_err	= m_http_listener->start(resp_arg);
	if( http_err.failed() )		return bt_err_from_http(http_err);
	
	// start the bt_httpo_listener_t
	m_httpo_listener	= nipmem_new bt_httpo_listener_t();
	bt_err		= m_httpo_listener->start(http_listener());
	if( bt_err.failed() )		return bt_err;

	// start the bt_httpo_resp_t
	m_httpo_resp	= nipmem_new bt_httpo_resp_t();
	bt_err		= m_httpo_resp->start(httpo_listener(), "http://0.0.0.0"
					, http_resp_mode_t::ACCEPT_SUBPATH, this, NULL);
	if( bt_err.failed() )	return bt_err;

	// return no error
	return bt_err_t::OK;
}


/** \brief Launch the bt_ezsession_t
 */
bt_err_t	casto_apps_t::launch_ezsession(const strvar_db_t &casto_conf)	throw()
{
	const strvar_db_t &	session_conf	= lib_session_get()->session_conf();
	lib_apps_t *		lib_apps	= lib_session_get()->lib_apps();
	const strvar_db_t &	arg_option	= lib_apps->arg_option();
	bt_err_t		bt_err;

	// get the tcp_listen_aview from the config
	ipport_aview_t	tcp_listen_aview;
	tcp_listen_aview= ipport_aview_helper_t::tcp_listen_aview_from_conf();
	// sanity check - the tcp_listen_aview local view MUST NOT be null
	DBG_ASSERT( !tcp_listen_aview.is_null() );

	// get the udp_listen_aview from the config
	ipport_aview_t	udp_listen_aview;
	udp_listen_aview= ipport_aview_helper_t::udp_listen_aview_from_conf();
	// sanity check - the udp_listen_aview local view MUST NOT be null
	DBG_ASSERT( !udp_listen_aview.is_null() );
	// log to debug
	KLOG_ERR("tcp_listen_aview=" << tcp_listen_aview << " udp_listen_aview=" << udp_listen_aview);

	// get the nslan_addr from the config
	ipport_addr_t	nslan_addr = session_conf.get_first_value("nslan_ipport", "255.255.255.255:7777");

	// set the bt_ezswarm_opt_t
	bt_ezsession_opt_t	ezsession_opt;
	ezsession_opt	|= bt_ezsession_opt_t::NSLAN_PEER;
	// NOTE: disable bt_ezswarm_opt_t::PEERSRC_KAD as kad not stable enougth to be usefull 
	//ezsession_opt	|= bt_ezsession_opt_t::KAD_PEER;

	// build the bt_ezsession_profile_t
	bt_ezsession_profile_t	ezsession_profile;
	ezsession_profile.nslan_addr	(nslan_addr);
	ezsession_profile.nslan_realmid	("nslan_realm_neoip_bt");
	ezsession_profile.kad_realmid	("kad_realm_neoip_bt");

	// set bt_session_profile_t::resp_jamrc4_type_arr() support ONLY bt_jamrc4_type_t::DOJAM
	ezsession_profile.session().resp_jamrc4_type_arr( bt_jamrc4_type_arr_t()
								.append(bt_jamrc4_type_t::DOJAM)
							);

	// Set the bt_io_cache_t size from casto_conf if any
	std::string	io_cache_size_s	= casto_conf.get_first_value("io_cache_size", "0");
	file_size_t	io_cache_size	= string_t::to_uint64(io_cache_size_s);
	if( io_cache_size > 0 ){
		// enable the bt_io_cache_t usage in ezsession_opt
		ezsession_opt	|= bt_ezsession_opt_t::USE_IO_CACHE;
		// set the io_cache_size in the bt_io_cache_pool_profile_t
		ezsession_profile.io_cache_pool().pool_maxlen( io_cache_size );
	}
	
	// if arg_option contain xmit-maxrate, set it up
	if( arg_option.contain_key("xmit-maxrate") ){
		size_t maxrate	= string_t::to_uint32(arg_option.get_first_value("xmit-maxrate"));
		ezsession_opt	|= bt_ezsession_opt_t::XMIT_RSCHED;
		ezsession_profile.xmit_maxrate( maxrate );
	}
	// if arg_option contain recv-maxrate, set it up
	if( arg_option.contain_key("recv-maxrate") ){
		size_t maxrate	= string_t::to_uint32(arg_option.get_first_value("recv-maxrate"));
		ezsession_opt	|= bt_ezsession_opt_t::RECV_RSCHED;
		ezsession_profile.recv_maxrate( maxrate );
	}

	// start the bt_ezsession_t;
	m_bt_ezsession	= nipmem_new bt_ezsession_t();
	bt_err		= m_bt_ezsession->profile(ezsession_profile).start(ezsession_opt
					, tcp_listen_aview, udp_listen_aview
					, kad_peerid_t::build_random()
					, bt_id_t::build_peerid("azureus", "0.0.0.2"));
	if( bt_err.failed() )	return bt_err;

	// return no error
	return bt_err_t::OK;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a pointer to a casto_itor_t matching the function params, or NULL if none matches
 */
casto_itor_t *	casto_apps_t::itor_by(const http_uri_t &mdata_srv_uri, const std::string &cast_name
					, const std::string &cast_privhash)	const throw()

{
	std::list<casto_itor_t *>::const_iterator	iter;
	// go thru the whole itor_db
	for(iter = itor_db.begin(); iter != itor_db.end(); iter++){
		casto_itor_t *	itor	= *iter;
		// if any parameters dont match the one of this casto_itor_t, goto the next
		if( itor->mdata_srv_uri() != mdata_srv_uri )	continue;
		if( itor->cast_name()     != cast_name )	continue;
		if( itor->cast_privhash() != cast_privhash )	continue;
		// if all parameters match, return this casto_itor_t
		return itor;
	}
	// if this point is reached, no matches have been found
	return NULL;
}

/** \brief Return a pointer to a casto_swarm_t matching the function params, or NULL if none matches
 */
casto_swarm_t *	casto_apps_t::swarm_by(const http_uri_t &mdata_srv_uri, const std::string &cast_name
					, const std::string &cast_privhash)	const throw()

{
	std::list<casto_swarm_t *>::const_iterator	iter;
	// go thru the whole swarm_db
	for(iter = swarm_db.begin(); iter != swarm_db.end(); iter++){
		casto_swarm_t *	swarm	= *iter;
		// if any parameters dont match the one of this casto_swarm_t, goto the next
		if( swarm->mdata_srv_uri() != mdata_srv_uri )	continue;
		if( swarm->cast_name()     != cast_name )	continue;
		if( swarm->cast_privhash() != cast_privhash )	continue;
		// if all parameters match, return this casto_swarm_t
		return swarm;
	}
	// if this point is reached, no matches have been found
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    bt_httpo_resp_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
			
/** \brief callback notified by \ref bt_httpo_resp_t when to notify an event
 */
bool	casto_apps_t::neoip_bt_httpo_resp_cb(void *cb_userptr, bt_httpo_resp_t &cb_bt_httpo_resp
						, const bt_httpo_event_t &httpo_event)	throw()
{
	// log to debug
	KLOG_ERR("enter httpo_event=" << httpo_event);

	// sanity check - the bt_httpo_event_t MUST BE is_resp_ok()
	DBG_ASSERT( httpo_event.is_resp_ok() );
	// sanity check - the bt_httpo_event_t MUST BE a CNX_ESTABLISHED
	DBG_ASSERT( httpo_event.is_cnx_established() );

	// get variable from the bt_httpo_full_t
	bt_httpo_full_t *	httpo_full	= httpo_event.get_cnx_established();
	const http_reqhd_t &	http_reqhd	= httpo_full->http_reqhd();
	const http_uri_t &	reqhd_uri	= http_reqhd.uri();


	// get the cast_mdata server uri from the reqhd_uri
	http_uri_t	mdata_srv_uri	= dfl_mdata_srv_uri();
	if( reqhd_uri.var().contain_key("mdata_srv_uri") ){
		std::string str	= reqhd_uri.var().get_first_value("mdata_srv_uri");
		mdata_srv_uri	= string_t::unescape(str);
	}
	// if any parameter are invalid, return a 400 error and delete the httpo_full
	if( mdata_srv_uri.is_null() || reqhd_uri.path().size() != 3 ){
		httpo_full->start_reply_error(http_status_t::BAD_REQUEST, "Invalid url");
		nipmem_zdelete	httpo_full;
		return true;
	}
	// get the cast_name from the reqhd_uri
	DBG_ASSERT( reqhd_uri.path().is_absolute() );
	std::string	cast_privhash	= reqhd_uri.path()[1].to_string();
	std::string	cast_name	= reqhd_uri.path()[2].to_string();
	
	// log to debug
	KLOG_ERR("reqhd_uri="<< reqhd_uri);
	KLOG_ERR("mdata_srv_uri=" << mdata_srv_uri);
	KLOG_ERR("cast_name=" << cast_name << " cast_privhash=" << cast_privhash);

	// try to find bt_casto_swarm_t matching the cast described in reqhd_uri
	casto_swarm_t *	casto_swarm;
	casto_swarm	= swarm_by(mdata_srv_uri, cast_name, cast_privhash);
	// if there is a matching bt_casto_swarm_t, add this bt_httpo_full_t to it
	if( casto_swarm ){
		casto_swarm->httpo_full_push(httpo_full);
		return true;
	}

	// try to find bt_casto_itor_t matching the cast described in reqhd_uri
	casto_itor_t *	casto_itor;
	casto_itor	= itor_by(mdata_srv_uri, cast_name, cast_privhash);
	// if there is a matching bt_casto_itor_t, add this bt_httpo_full_t to it
	if( casto_itor ){
		casto_itor->httpo_full_push(httpo_full);
		return true;
	}
	
	// if no swarm or itor match the cast described in reqhd_uri, create a bt_casto_itor_t for it
	bt_err_t	bt_err;
	casto_itor	= nipmem_new casto_itor_t();
	bt_err		= casto_itor->start(this, mdata_srv_uri, cast_name, cast_privhash
								, httpo_full, this, NULL);
	if( bt_err.failed() )	nipmem_zdelete casto_itor;

	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			casto_itor_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref casto_itor_t when it is completed
 */
bool casto_apps_t::neoip_casto_itor_cb(void *cb_userptr, casto_itor_t &cb_casto_itor
		, const bt_err_t &cb_bt_err, const bt_cast_mdata_t &cast_mdata)	throw()
{
	casto_itor_t *	casto_itor	= &cb_casto_itor;
	bt_err_t	bt_err		= cb_bt_err;
	// log to debug
	KLOG_ERR("enter bt_err=" << bt_err << " cast_mdata=" << cast_mdata );

	// if bt_err.failed(), just delete casto_itor and return
	if( bt_err.failed() ){
		nipmem_zdelete casto_itor;
		return false;
	}

	// sanity check - at this point, bt_err.succeed() MUST be true
	DBG_ASSERT( bt_err.succeed() );

	// start casto_swarm_t
	casto_swarm_t *	casto_swarm;
	casto_swarm	= nipmem_new casto_swarm_t();
	bt_err		= casto_swarm->start(this, casto_itor->mdata_srv_uri()
					, casto_itor->cast_name(), casto_itor->cast_privhash()
					, cast_mdata);
	if( bt_err.failed() )	KLOG_ERR("cant start casto_swarm_t due to " << bt_err);
	if( bt_err.failed() )	nipmem_zdelete casto_swarm;

	// pass all the httpo_full from itor and push them in swarm
	if( bt_err.succeed() ){
		while(true){
			bt_httpo_full_t * httpo_full; 
			httpo_full	= casto_itor->httpo_full_pop();
			if( !httpo_full ) break;
			casto_swarm->httpo_full_push(httpo_full);
		}
	}
	
	// delete the casto_itor
	nipmem_zdelete casto_itor;
	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the clineopt_arr_t for the cmdline option of the lib_session_t
 */
clineopt_arr_t	casto_apps_t::clineopt_arr()	throw()
{
	clineopt_arr_t	clineopt_arr;
	clineopt_t	clineopt;
	// add the --xmit-maxrate cmdline option
	clineopt	= clineopt_t("xmit-maxrate", clineopt_mode_t::REQUIRED)
						.option_mode(clineopt_mode_t::OPTIONAL)
						.help_string("Specify the maximum rate in xmit"
							"\n\t\tIf this option is not specified, there are no limit.");
	clineopt.alias_name_db().append("x");
	clineopt_arr	+= clineopt;
	// add the --recv-maxrate cmdline option
	clineopt	= clineopt_t("recv-maxrate", clineopt_mode_t::REQUIRED)
						.option_mode(clineopt_mode_t::OPTIONAL)
						.help_string("Specify the maximum rate in recv"
							"\n\t\tIf this option is not specified, there are no limit.");
	clineopt.alias_name_db().append("r");
	clineopt_arr	+= clineopt;
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




