/*! \file
    \brief Definition of the \ref oload_apps_t


- rb(at)actech-innovation(dot)com
  - gars faisant un "magnetoscope sur IP" apparement vers nantes
  
\par Possible Improvement
- support the outter uri variables about max rate and buffer_len
  - e.g. fill the buffer as fast as possible, then deliver to httpo_full_t
    at a maximum of max_rate
  - it may be usefull when delivering audio/video to poorly coded players.
  - e.g. im not sure it is possible to limit the flash buffer

\par Possible Improvement
- To be able to seed once downloaded
  - build the hash of the whole file while downloading
  - find back the file when doaloded
    - based on location (possibly hinted by the downloader user-agent)
    - based on the ctime/mtime/file_size
  - how to handle partial download (aka partial http request)
  - then seed it
    - issue with very large seeding, as it may have MANY publishers and MANY files
      - in relation with the not-yet-implemented meshns
- To support http server without partial request
  - do something similar to bt_httpi_t 
  - aka download from http the begining of the file
  - and try to download the end of the file via bt_swarm_t
  - stop when the whole file is available
- have the path of the local file specified in the nested uri ?
  - what to do with it ?
  - to determine the amount of file already downloaded ? well the caller
    can already do it itself and issue a range request
  - to store directly the data in the file ? what is the point of this ?
    to be able to have a much larger bt_pselect_slide_cur_t ?
  - ok the interest is not clear for all this... but i had this idea and
    wanted to note it.

*/

/* system include */
/* local include */
#include "neoip_oload_apps.hpp"
#include "neoip_oload_info.hpp"		// NOTE: needed here for neoip-webok
#include "neoip_oload_itor.hpp"
#include "neoip_oload_swarm.hpp"
#include "neoip_oload_helper.hpp"


#include "neoip_oload_mod_vapi.hpp"
#include "neoip_oload_mod_raw.hpp"
#include "neoip_oload_mod_flv.hpp"
#include "neoip_oload_httpo_ctrl.hpp"
#include "neoip_oload_flash_xdom.hpp"

#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_ezsession_opt.hpp"
#include "neoip_bt_id.hpp"
#include "neoip_bt_mfile.hpp"

#include "neoip_bt_cast_prefetch.hpp"

#include "neoip_http_listener.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_http_nested_uri.hpp"

#if 0
#include "neoip_http_sresp.hpp"		// TODO to remove
#include "neoip_http_sresp_ctx.hpp"
#include "neoip_http_resp_mode.hpp"
#endif

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
oload_apps_t::oload_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	m_http_listener		= NULL;
	m_httpo_listener	= NULL;
	m_apps_httpdetect	= NULL;
	m_cast_prefetch		= NULL;
	m_httpo_ctrl		= NULL;
	m_flash_xdom		= NULL;
	m_bt_ezsession		= NULL;
}

/** \brief Destructor
 */
oload_apps_t::~oload_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete all the itor_db if needed
	while( !itor_db.empty() )	nipmem_delete itor_db.front();	
	// delete all the swarm_db if needed
	while( !swarm_db.empty() )	nipmem_delete swarm_db.front();	
	// delete the bt_ezsession_t if needed
	nipmem_zdelete	m_bt_ezsession;
	// delete all the mod_db if needed
	while( !mod_db.empty() )	nipmem_delete mod_db.front();
	
	// delete the pidfile and urlfile if needed
	if( m_http_listener ){
		lib_apps_helper_t::pidfile_remove();
		lib_apps_helper_t::urlfile_remove();
	}
	// delete the whole io_pfile_dirpath if needed
	if( !m_io_pfile_dirpath.is_null() ){
		file_utils_t::remove_directory(io_pfile_dirpath(), file_utils_t::DO_RECURSION);
	}
	// delete the bt_httpo_t if needed
	nipmem_zdelete	m_httpo_listener;
	// delete the m_flash_xdom if needed
	nipmem_zdelete	m_flash_xdom;
	// delete the m_httpo_ctrl if needed
	nipmem_zdelete	m_httpo_ctrl;
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
bt_err_t	oload_apps_t::start()						throw()
{
	lib_session_t *		lib_session	= lib_session_get();
	lib_apps_t *		lib_apps	= lib_session->lib_apps();
	const strvar_db_t &	arg_option	= lib_apps->arg_option();
	file_path_t		config_path	= lib_session->conf_rootdir() / "oload";
	bt_err_t		bt_err;
	// log to debug
	KLOG_WARN("enter");

	// init the static link http - to ease access while debugging
	wikidbg_obj_add_static_page("/oload_apps_" + OSTREAMSTR(this));
	
	// load the configuration file for this apps
	strvar_db_t oload_conf	= strvar_helper_t::from_file(config_path / "neoip_oload.conf");

	// start the bt_httpo_t
	bt_err		= launch_httpo(oload_conf);
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

	// Launch the oload_httpo_ctrl_t
	m_httpo_ctrl	= nipmem_new oload_httpo_ctrl_t();
	bt_err		= m_httpo_ctrl->start(this);
	if( bt_err.failed() )		return bt_err;
	
	// Launch the oload_flash_xdom_t
	m_flash_xdom	= nipmem_new oload_flash_xdom_t();
	bt_err		= m_flash_xdom->start(http_listener());
	if( bt_err.failed() )		return bt_err;
	
	// start the oload_mod_vapi_t
	bt_err		= launch_mod();
	if( bt_err.failed() )		return bt_err;
	
	// start the bt_httpo_t
	bt_err		= launch_ezsession(oload_conf);
	if( bt_err.failed() )		return bt_err;

	// if the arg_option DOES NOT contains the "nodaemon" key, pass daemon
	// - NOTE: be carefull this change the pid
	if( !arg_option.contain_key("nodaemon") && lib_apps_helper_t::daemonize().failed() ){
		DBG_ASSERT( 0 );
		return bt_err_t(bt_err_t::ERROR, "Can't daemon()");
	}

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
	// - there is the deletion of this directory in the oload_apps_t dtor
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

/** \brief Constructor the bt_httpo_t and http_listener_t
 */
bt_err_t	oload_apps_t::launch_httpo(const strvar_db_t &oload_conf)	throw()
{
	bt_err_t	bt_err;
	// log to debug
	KLOG_WARN("enter");

	// get the listen_ipport from the oload_conf
	std::string	port_min_str	= oload_conf.get_first_value("http_listener_port_min", "4242");
	std::string	port_max_str	= oload_conf.get_first_value("http_listener_port_max", "4242");
	ip_addr_t	listener_addr	= oload_conf.get_first_value("http_listener_addr", "127.0.0.1");
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
	
	// start the bt_httpo_t
	m_httpo_listener= nipmem_new bt_httpo_listener_t();
	bt_err		= m_httpo_listener->start(http_listener());
	if( bt_err.failed() )		return bt_err;
	
	// return no error
	return bt_err_t::OK;
}

/** \brief Launch all the oload_mod_vapi_t
 */
bt_err_t	oload_apps_t::launch_mod()	throw()
{
	bt_err_t	bt_err;
	// start the oload_mode_raw_t
	oload_mod_raw_t	* mod_raw;
	mod_raw		= nipmem_new oload_mod_raw_t();
	bt_err		= mod_raw->start(this);
	if( bt_err.failed() )	return bt_err;

	// start the oload_mode_flv_t
	oload_mod_flv_t	* mod_flv;
	mod_flv		= nipmem_new oload_mod_flv_t();
	bt_err		= mod_flv->start(this);
	if( bt_err.failed() )	return bt_err;

	// return no error
	return bt_err_t::OK;
}



/** \brief Launch the bt_ezsession_t
 */
bt_err_t	oload_apps_t::launch_ezsession(const strvar_db_t &oload_conf)	throw()
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

	// set bt_session_profile_t::resp_jamrc4_type_arr() support bt_jamrc4_type_t::DOJAM
	// and support bt_jamrc4_type_t::NOJAM
	// - thus neoip-oload can do both type. bt_swarm_t which doesnt explicitly demand
	//   bt_jamrc4_type_t::NOJAM, will default in bt_jamrc4_type_t::DOJA  
	ezsession_profile.session().resp_jamrc4_type_arr( bt_jamrc4_type_arr_t()
								.append(bt_jamrc4_type_t::NOJAM)
								.append(bt_jamrc4_type_t::DOJAM)
							);
	// Set the bt_io_cache_t size from oload_conf if any
	std::string	io_cache_size_s	= oload_conf.get_first_value("io_cache_size", "0");
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
					, bt_id_t::build_peerid("azureus", "1.2.3.4"));
	if( bt_err.failed() )	return bt_err;

	// return no error
	return bt_err_t::OK;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a pointer to a oload_itor_t matching the inner_uri, or NULL if none matches
 */
oload_itor_t *	oload_apps_t::itor_by_inner_uri(const http_uri_t &inner_uri) const throw()
{
	std::list<oload_itor_t *>::const_iterator	iter;
	// go thru the whole itor_db
	for(iter = itor_db.begin(); iter != itor_db.end(); iter++){
		oload_itor_t *	itor	= *iter;
		// if this oload_itor_t matches this inner_uri, return its pointer
		if( itor->inner_uri() == inner_uri )	return itor;
	}
	// if this point is reached, no matches have been found
	return NULL;
}


/** \brief Return a pointer to a oload_swarm_t matching the inner_uri, or NULL if none matches
 */
oload_swarm_t *	oload_apps_t::swarm_by_inner_uri(const http_uri_t &inner_uri) const throw()
{
	std::list<oload_swarm_t *>::const_iterator	iter;
	// go thru the whole swarm_db
	for(iter = swarm_db.begin(); iter != swarm_db.end(); iter++){
		oload_swarm_t *	swarm	= *iter;
		// if this oload_swarm_t matches this inner_uri, return its pointer
		if( swarm->inner_uri() == inner_uri )	return swarm;
	}
	// if this point is reached, no matches have been found
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_httpo_full_t post establishement processing
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
			
/** \brief post processing of every bt_httpo_full_t post establishement
 * 
 * - this transfert the ownership of bt_httpo_full_t to oload_apps_t
 */
void	oload_apps_t::httpo_full_post_esta(bt_httpo_full_t *httpo_full)		throw()
{
	bt_err_t	bt_err;
	// some alias variable to ease readability
	const http_reqhd_t &	http_reqhd	= httpo_full->http_reqhd();
	const http_uri_t &	nested_uri	= http_reqhd.uri();

	// log to debug
	KLOG_ERR("nested_uri="	<< nested_uri);
	KLOG_ERR("http_reqhd="	<< http_reqhd);
	
	// if the nested_uri is invalid, delete the bt_httpo_full_t and exit
	if( !http_nested_uri_t::is_valid_nested(nested_uri) ){
		// reply the http error 400 "Bad request" - rfc2616.10.4.1
		oload_helper_t::reply_err_httpo_full(httpo_full, http_status_t::BAD_REQUEST
							, "Invalid nested url");
		nipmem_delete	httpo_full;
		return;
	}
	
	// honor the oload_mod_vapi_t::pre_itor_hook
	oload_mod_vapi_t *	mod_vapi	= oload_helper_t::mod_vapi_from(httpo_full);
	bt_err	= mod_vapi->pre_itor_hook(httpo_full);
	if( bt_err.failed() ){
		// reply the http error 401 "Unauthorized" - rfc2616.10.4.2
		oload_helper_t::reply_err_httpo_full(httpo_full, http_status_t::UNAUTHORIZED
							, bt_err.reason());
		nipmem_delete	httpo_full;
		return;
	}

	// parse the inner_uri from the nested_uri
	http_uri_t	inner_uri	= oload_helper_t::parse_inner_uri(nested_uri);

	// try to find bt_oload_swarm_t matchin the inner_uri
	oload_swarm_t *	oload_swarm;
	oload_swarm	= swarm_by_inner_uri(inner_uri);
	// if there is a matching bt_oload_swarm_t, add this bt_httpo_full_t to it
	if( oload_swarm ){
		oload_swarm->httpo_full_push(httpo_full);
		return;
	}

	// try to find bt_oload_itor_t matchin the inner_uri
	oload_itor_t *	oload_itor;
	oload_itor	= itor_by_inner_uri(inner_uri);
	// if there is a matching bt_oload_itor_t, add this bt_httpo_full_t to it
	if( oload_itor ){
		oload_itor->httpo_full_push(httpo_full);
		return;
	}
	
	// if no swarm or itor match this nested_uri, create a bt_oload_itor_t for it
	oload_itor	= nipmem_new oload_itor_t();
	bt_err		= oload_itor->start(this, nested_uri, httpo_full, this, NULL);
	if( bt_err.failed() )	nipmem_zdelete oload_itor;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			oload_itor_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref oload_itor_t when it is completed
 */
bool oload_apps_t::neoip_oload_itor_cb(void *cb_userptr, oload_itor_t &cb_oload_itor
			, const bt_err_t &cb_bt_err, const bt_mfile_t &bt_mfile)	throw()
{
	oload_itor_t *	oload_itor	= &cb_oload_itor;
	bt_err_t	bt_err		= cb_bt_err;
	// log to debug
	KLOG_ERR("enter bt_err=" << bt_err << " bt_mfile.name()=" << bt_mfile.name() );

	// if bt_err.failed(), just delete oload_itor and return
	if( bt_err.failed() ){
		nipmem_zdelete oload_itor;
		return false;
	}

	// sanity check - at this point, bt_err.succeed() MUST be true
	DBG_ASSERT( bt_err.succeed() );

	// create oload_swarm_t
	oload_swarm_t *	oload_swarm;
	oload_swarm	= nipmem_new oload_swarm_t();
	// start the oload_swarm_t
	bt_err		= oload_swarm->start(this, oload_itor->nested_uri(), bt_mfile);
	if( bt_err.failed() )	nipmem_zdelete oload_swarm;

	// pass all the httpo_full from itor and push them in swarm
	if( bt_err.succeed() ){
		while(true){
			bt_httpo_full_t * httpo_full; 
			httpo_full	= oload_itor->httpo_full_pop();
			if( !httpo_full ) break;
			oload_swarm->httpo_full_push(httpo_full);
		};
	}
	
	// delete the oload_itor
	nipmem_zdelete oload_itor;
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
clineopt_arr_t	oload_apps_t::clineopt_arr()	throw()
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




