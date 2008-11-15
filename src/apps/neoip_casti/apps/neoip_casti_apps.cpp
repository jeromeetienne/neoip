/*! \file
    \brief Definition of the \ref casti_apps_t


\par Idea to testing it
- CASE 1
  - one casti with -x 50k
  - a lot of casto without bandwidth limit
- CASE 2
  - one casti with -x 50k
  - many casto with -x 50k -r 50k
- CASE 3
  - same as case 2 + one relay without rate_limit_t

\par TODO better handling of multiple streams
- the multistream feature is very poorly handled
  - aka some part works only for a single casti_swarm_t
  - e.g. the bt_cast_mdata_t reply
  - e.g. the casti_mod_type_t from the command line
  - NOTE: not a big/urgent problem but noted to keep track of it

\par Idea about the licensing
- description: may be used for free if the content is under CC (or similar) BUT
  needs to be paid.
  - similar model than the GPL or pay in a way that "if your stuff is not open
    you have to pay for it"
- help seeding: this allows to seed the system by having all the CC content
  provider it to use/promote it
- negligible money cost: this doesnt reduce significantly the money made on it
  because CC content providers are unlikely to pay anyway

\par About the 2 type of casti_swarm_t initialization
- if the casti_swarm_t has been triggered by cmdline, it is attempted to stream
  all the time.
  - aka the casti_swarm_t wont be deleted by casti_apps_t (only stopping neoip-casti
    will stop it)
  - aka if the scasti_t is disconnected, casti_apps_t will periodicatlly retry to
    reconnect it
- if the casti_swarm_t has been triggered by webpage
  - stopping: can be done explicitly by the webpage
  - stopping: can be done if the webpage do not renew its requests

*/

/* system include */
/* local include */
#include "neoip_casti_apps.hpp"
#include "neoip_casti_info.hpp"		// NOTE: needed here for neoip-webok
#include "neoip_casti_swarm.hpp"
#include "neoip_casti_swarm_arg.hpp"
#include "neoip_casti_ctrl_cline.hpp"
#include "neoip_casti_ctrl_wpage.hpp"
#include "neoip_casti_inetreach_httpd.hpp"

#include "neoip_http_listener.hpp"
#include "neoip_http_resp_mode.hpp"

#include "neoip_apps_httpdetect.hpp"

#include "neoip_bt_cast_id.hpp"
#include "neoip_bt_cast_mdata_server.hpp"
#include "neoip_bt_cast_mdata.hpp"

#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_ezsession_opt.hpp"
#include "neoip_bt_id.hpp"

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
casti_apps_t::casti_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	m_http_listener		= NULL;
	m_inetreach_httpd	= NULL;
	m_apps_httpdetect	= NULL;
	m_ctrl_cline		= NULL;
	m_ctrl_wpage		= NULL;
	m_mdata_server		= NULL;
	m_bt_ezsession		= NULL;
}

/** \brief Destructor
 */
casti_apps_t::~casti_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the pidfile and urlfile if needed
	if( m_bt_ezsession ){
		lib_apps_helper_t::pidfile_remove();
		lib_apps_helper_t::urlfile_remove();
		file_utils_t::remove_directory(io_pfile_dirpath(), file_utils_t::DO_RECURSION);
	}
	// delete all the swarm_db if needed
	while( !swarm_db.empty() )	nipmem_delete swarm_db.front();
	// delete the bt_ezsession_t if needed
	nipmem_zdelete	m_bt_ezsession;
	// delete the bt_cast_mdata_server_t if needed
	nipmem_zdelete	m_mdata_server;
	// delete the casti_ctrl_cline_t if needed
	nipmem_zdelete	m_ctrl_cline;
	// delete the casti_ctrl_wpage_t if needed
	nipmem_zdelete	m_ctrl_wpage;
	// delete the m_apps_httpdetect if needed
	nipmem_zdelete	m_apps_httpdetect;
	// delete the casti_inetreach_httpd_t if needed
	nipmem_zdelete	m_inetreach_httpd;
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
bt_err_t	casti_apps_t::start()						throw()
{
	lib_session_t *		lib_session	= lib_session_get();
	lib_apps_t *		lib_apps	= lib_session->lib_apps();
	const strvar_db_t &	arg_option	= lib_apps->arg_option();
	file_path_t		config_path	= lib_session->conf_rootdir() / "casti";
	bt_err_t		bt_err;
	// log to debug
	KLOG_WARN("enter");

	// init the static link http - to ease access while debugging
	wikidbg_obj_add_static_page("/casti_apps_" + OSTREAMSTR(this));

	// load the configuration file for this apps
	strvar_db_t casti_conf	= strvar_helper_t::from_file(config_path / "neoip_casti.conf");

	// read some defaults value fromt the casti_conf - for future reference
	m_dfl_http_peersrc_uri	= casti_conf.get_first_value("dfl_http_peersrc_uri");
	m_dfl_mdata_srv_uri	= casti_conf.get_first_value("dfl_mdata_srv_uri");

	// start the bt_http_listener_t
	bt_err		= launch_http_listener(casti_conf);
	if( bt_err.failed() )		return bt_err;

	// start the inetreach_httpd
	m_inetreach_httpd= nipmem_new casti_inetreach_httpd_t();
	bt_err		= m_inetreach_httpd->start(this, NULL);
	if( bt_err.failed() )		return bt_err;

	// Launch the apps_httpdetect_t - use directly neoip_*_info.hpp #define
	// - cant use libapps->canon_name() because it would be wrong for neoip-webok
	libsess_err_t	libsess_err;
	m_apps_httpdetect = nipmem_new apps_httpdetect_t();
	libsess_err	= m_apps_httpdetect->start(http_listener(), NEOIP_APPS_CANON_NAME
							, NEOIP_APPS_VERSION);
	if( libsess_err.failed() )	return bt_err_t(bt_err_t::ERROR, libsess_err.to_string());

	// start the m_ctrl_cline
	m_ctrl_cline	= nipmem_new casti_ctrl_cline_t();
	bt_err		= m_ctrl_cline->start(this);
	if( bt_err.failed() )		return bt_err;

	// start the m_ctrl_wpage
	m_ctrl_wpage	= nipmem_new casti_ctrl_wpage_t();
	bt_err		= m_ctrl_wpage->start(this);
	if( bt_err.failed() )		return bt_err;

	// start the m_mdata_server
	m_mdata_server	= nipmem_new bt_cast_mdata_server_t();
	bt_err		= m_mdata_server->start(m_inetreach_httpd->http_listener(), this, NULL);
	if( bt_err.failed() )		return bt_err;

	// start the bt_ezsession_t
	bt_err		= launch_ezsession(casti_conf);
	if( bt_err.failed() )		return bt_err;

	// set the io_pfile_dirpath name
	m_io_pfile_dirpath	= lib_session->temp_rootdir() / (lib_apps->canon_name()+"_pfile.");
	m_io_pfile_dirpath	= file_utils_t::get_temp_path(m_io_pfile_dirpath);
	// create the temporary directory io_pfile_dirpath()
	// - TODO this should be created by bt_alloc_t?
	// - all this stuff about bt_alloc_t and bt_io_pfile_t is unclear
	// - there is the deletion of this directory in the casti_apps_t dtor
	file_err_t	file_err;
	file_err	= file_utils_t::create_directory(io_pfile_dirpath(), file_utils_t::NO_RECURSION);
	if( file_err.failed() )	return bt_err_from_file(file_err);

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

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			subpart launcher
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor the http_listener_t
 */
bt_err_t	casti_apps_t::launch_http_listener(const strvar_db_t &casti_conf)	throw()
{
	bt_err_t	bt_err;
	// log to debug
	KLOG_WARN("enter");

	// get the listen_ipport from the casti_conf
	std::string	port_min_str	= casti_conf.get_first_value("http_listener_port_min", "4242");
	std::string	port_max_str	= casti_conf.get_first_value("http_listener_port_max", "4242");
	ip_addr_t	listener_addr	= casti_conf.get_first_value("http_listener_addr", "127.0.0.1");
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

	// return no error
	return bt_err_t::OK;
}

/** \brief Launch the bt_ezsession_t
 */
bt_err_t	casti_apps_t::launch_ezsession(const strvar_db_t &oload_conf)	throw()
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

// TODO about special case in bt_httpi_t and bt_io_cache_t
// - bt_io_cache_write_t will read a full block (e.g. 16kbyte) from the disk
//   if the write is less than 16kbyte
// - but bt_httpi_t write data as they are received (may be 2k or 50k or another size)
// - this cause bt_io_cache_write_t to fails because it tries to write from a block
//   (in this case a file because the bt_io_cache_t subio_vapi is bt_io_pfile_t)
//   which doesnt exists, and so reject the write and return 'no such file or directory'
// - this is an issue in the handling of bt_io_cache_write_t
// - currently no brain to fix it
//   - out of the blue, what about 'if read failed, considere it is 0?'
//   - this issue seems close to the bt_alloc_t issue which is not done for
//     bt_io_pfile_t
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
					, bt_id_t::build_peerid("azureus", "0.0.0.1"));
	if( bt_err.failed() )	return bt_err;

	// return no error
	return bt_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a pointer on the casti_swarm_t matching this bt_cast_id_t
 */
casti_swarm_t *	casti_apps_t::swarm_by_cast_id(const bt_cast_id_t &cast_id)	const throw()
{
	std::list<casti_swarm_t *>::const_iterator	iter;
	// go throw the whole swarm_db
	for(iter = swarm_db.begin(); iter != swarm_db.end(); iter++){
		casti_swarm_t *	casti_swarm	= *iter;
		// if this casti_swarm_t bt_cast_id_t matches, return its pointer
		if( casti_swarm->cast_id() == cast_id )	return casti_swarm;
	}
	// if this point is reached, none matches, so return NULL
	return NULL;
}

/** \brief Return a pointer to a casti_swarm_t matching the function params, or NULL if none matches
 */
casti_swarm_t *	casti_apps_t::swarm_by(const http_uri_t &mdata_srv_uri, const std::string &cast_name
					, const std::string &cast_privtext)	const throw()

{
	std::list<casti_swarm_t *>::const_iterator	iter;
	// go thru the whole swarm_db
	for(iter = swarm_db.begin(); iter != swarm_db.end(); iter++){
		casti_swarm_t *	swarm	= *iter;
		// if any parameters dont match the one of this casto_swarm_t, goto the next
		if( swarm->mdata_srv_uri()	!= mdata_srv_uri )	continue;
		if( swarm->cast_name()		!= cast_name )		continue;
		if( swarm->cast_privtext()	!= cast_privtext )	continue;
		// if all parameters match, return this casto_swarm_t
		return swarm;
	}
	// if this point is reached, no matches have been found
	return NULL;
}

/** \brief Return a base casti_swarm_arg_t with some default values
 *
 * - NOTE: it is not fully initialized but
 */
casti_swarm_arg_t	casti_apps_t::swarm_arg_default()			throw()
{
	casti_swarm_arg_t	swarm_arg;
	// build the object
	swarm_arg.casti_apps		(this);
	swarm_arg.mdata_srv_uri		(dfl_mdata_srv_uri());
	swarm_arg.scasti_mod		(bt_scasti_mod_type_t::RAW);
	swarm_arg.http_peersrc_uri	(dfl_http_peersrc_uri());
	// return the just built object
	return swarm_arg;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     bt_cast_mdata_server_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_cast_mdata_server_t to provide event
 */
bool	casti_apps_t::neoip_bt_cast_mdata_server_cb(void *cb_userptr, bt_cast_mdata_server_t &cb_mdata_server
		, const bt_cast_id_t &cast_id, bt_cast_mdata_t *cast_mdata_out)	throw()
{
	// log to debug
	KLOG_ERR("enter bt_cast_mdata_server_cb_t cast_id=" << cast_id);
	// try to find a casti_swarm_t matching this cast_name
	casti_swarm_t *	casti_swarm;
	casti_swarm	= swarm_by_cast_id(cast_id);
	// if no casti_swarm_t matched, return now
	if( !casti_swarm )	return true;
	// copy the casti_swarm_t bt_cast_mdata_t
	*cast_mdata_out	= casti_swarm->current_mdata();
	// return tokeep
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    casti_inetreach_httpd_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref casti_inetreach_httpd_t
 */
bool	casti_apps_t::neoip_casti_inetreach_httpd_cb(void *cb_userptr
				, casti_inetreach_httpd_t &cb_inetreach_httpd
				, const ipport_addr_t &new_ipport_pview)	throw()
{
	std::list<casti_swarm_t *>::const_iterator	iter;
	// log to debug
	KLOG_ERR("enter new_ipport_pview=" << new_ipport_pview);

	// go throw the whole swarm_db
	for(iter = swarm_db.begin(); iter != swarm_db.end(); iter++){
		casti_swarm_t *	casti_swarm	= *iter;
		// warn this casti_swarm_t that a republish_required
		casti_swarm->notify_republish_required();
	}

	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			clineopt_arr
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the clineopt_arr_t for the cmdline option of the lib_session_t
 */
clineopt_arr_t	casti_apps_t::clineopt_arr()	throw()
{
	clineopt_arr_t	clineopt_arr;
	clineopt_t	clineopt;
	// add the casti_ctrl_cline_t options
	clineopt_arr	+= casti_ctrl_cline_t::clineopt_arr();
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




