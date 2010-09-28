/*! \file
    \brief Definition of the \ref router_apps_t class

\par About file naming
- the files ending with _cert are the public one , the once ending wiht *_priv
  are the private one
- .rootca_cert/priv are the rootca files
- .selfsigned_cert/priv are the lident/rident file for the selfsigned
- .authsigned_cert is the lident for the authsigned

\par About --rootca_create command line option
- this is just a kludge to create a rootca with the main application
- it is a kludge as it is deemed to disapears and have a special tool
  to register name in case of authsigned identity.
  - this allow to get a autonomous software which ease the testing
- the external tool is not yet written
  - but it MUST be as currently nobody ensure the uniqueness of router_name_t
    for a given rootca domain but still it is assumed to be unique in the code
  - aka bad bad bouh :)

\par Possible improvement - more regular cmdline option to handle identity
- --create-selfsigned hostname-dnsname: to replace the current --lident_register for
  host-only-dnsname
- --import-selfsigned filename: copy the filename into router/authorized_peer.d
- --import-authsigned filename: copy the filename into router/local_identity/
  directory and make the router/current_identity.priv points to it
- --create-authsigned fully-qualified-dnsname: to replace the current --lident_register
  for fully-qualified-dnsname    

*/

/* system include */
/* local include */
#include "neoip_router_apps.hpp"
#include "neoip_router_apps_helper.hpp"
#include "neoip_router.hpp"
#include "neoip_clineopt_arr.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_libsess_err.hpp"
#include "neoip_lib_apps.hpp"
#include "neoip_lib_apps_helper.hpp"
#include "neoip_router_name.hpp"
#include "neoip_dns_helper.hpp"
#include "neoip_router_rootca_arr.hpp"
#include "neoip_ipport_aview_helper.hpp"
#include "neoip_file.hpp"
#include "neoip_strvar_db.hpp"
#include "neoip_strvar_helper.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
router_apps_t::router_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some fields
	router_peer	= NULL;
}
	
/** \brief Destructor
 */
router_apps_t::~router_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// save the router_peer_t acache - to be reused on next run
	if( router_peer ){
		router_err_t	router_err;
		router_err	= router_apps_helper_t::acache_save(lib_session_get()->cache_rootdir()
								, router_peer->acache());
		if( router_err.failed() )
			KLOG_ERR("Cant save router_acache_t due to " << router_err);
	}
	// delete the pidfile and urlfile if needed
	if( router_peer ){
		lib_apps_helper_t::pidfile_remove();
		lib_apps_helper_t::urlfile_remove();
	}
	// delete router_peer_t if needed
	nipmem_zdelete	router_peer;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
router_err_t	router_apps_t::start()	throw()
{
	lib_session_t *		lib_session	= lib_session_get();
	lib_apps_t *		lib_apps	= lib_session->lib_apps();
	const strvar_db_t &	arg_option	= lib_apps->arg_option();
	file_path_t		config_path	= lib_session->conf_rootdir() / "router";
	router_profile_t	router_profile;
	router_lident_t		router_lident;
	router_rident_arr_t	rident_arr;
	router_rootca_arr_t	rootca_arr;
	file_err_t		file_err;
	router_err_t		router_err;

	// load the configuration file for the router
	strvar_db_t	router_conf	= strvar_helper_t::from_file(config_path / "neoip_router.conf");

	// setup the ip_netaddr_t handled by the router_peer_t
	ip_netaddr_arr_t	ip_netaddr_arr;
	std::string	avail_netaddr_str= router_conf.get_first_value("avail_netaddr_str", "5.0.0.0/8");
	ip_netaddr_arr	+= avail_netaddr_str.c_str();

	/*************** handle the key creation option	***********************/
	// if the arg_option contains the "lident_prepare" key, call the proper cmdline_action function
	if( arg_option.contain_key("lident_prepare") )	return cmdline_action_lident_prepare();
	// if the arg_option contains the "lident_register" key, call the proper cmdline_action function
	if( arg_option.contain_key("lident_register") )	return cmdline_action_lident_register();
	// if the arg_option contains the "rootca_create" key, call the proper cmdline_action function
	if( arg_option.contain_key("rootca_create") )	return cmdline_action_rootca_create();
	
	/*************** handle all the info option	***********************/
	// if the arg_option contains the "info" key, call the proper cmdline_action function
	if( arg_option.contain_key("info") )		return cmdline_action_info();
	// if the arg_option contains the "info-local-identity" key, call the proper action
	if( arg_option.contain_key("info-local-identity") )
		return router_apps_helper_t::disp_info_lident();
	// if the arg_option contains the "info-remote-identity" key, call the proper action
	if( arg_option.contain_key("info-remote-identity") )
		return router_apps_helper_t::disp_info_rident_arr();
	// if the arg_option contains the "info-rootca" key, call the proper action
	if( arg_option.contain_key("info-rootca") )
		return router_apps_helper_t::disp_info_rootca_arr();
	// if the arg_option contains the "info-acl" key, call the proper action
	if( arg_option.contain_key("info-acl") )
		return router_apps_helper_t::disp_info_acl();
	// if the arg_option contains the "info-acache" key, call the proper action
	if( arg_option.contain_key("info-acache") )
		return router_apps_helper_t::disp_info_acl();
	

	/*************** start the normal starting	***********************/

	// set the profile tunnel_stub if it is defined in the router property file
	if( router_conf.contain_key("tunnel_stub") )	router_profile.tunnel_stub(true);

	// set the profile dnsgrab_prio if it is defined in the router property file
	if( router_conf.contain_key("dnsgrab_priority") ){
		std::string	dnsgrab_priority = router_conf.get_first_value("dnsgrab_priority");
		router_profile.dnsgrab_arg().reg_priority( atoi(dnsgrab_priority.c_str()) );
	}
	
	// as neoip_router is system wide, the dnsgrab MUST be system wide too
	router_profile.dnsgrab_arg().reg_location("system");

	// load the router_lident from the config_path
	router_err	= router_apps_helper_t::lident_load(config_path, router_lident);
	if( router_err.failed() )	return router_err;
	
	// init the rident from the files
	router_err	= router_apps_helper_t::rident_arr_load(config_path, rident_arr);
	if( router_err.failed() )	return router_err;

	// load the router_rootca_arr_t
	router_err	= router_apps_helper_t::rootca_arr_load(config_path, rootca_arr);
	if( router_err.failed() )	return router_err;
	
	// load the hostname_acl.conf file into the router_acl_t
	router_acl_t	router_acl;
	router_err	= router_acl.load_file(config_path / "hostname_acl.conf");
	if( router_err.failed() )	return router_err;

	// get the udp_listen_aview from the config
	ipport_aview_t	udp_listen_aview	= ipport_aview_helper_t::udp_listen_aview_from_conf();
	// if no udp_listen_aview have been found, return an error
	if( udp_listen_aview.is_null() )
		return router_err_t(router_err_t::ERROR, "Unable to find a udp_listen_aview");

	// init and start router_peer_t
	router_peer	= nipmem_new router_peer_t();
	router_err	= router_peer->set_profile(router_profile).start(udp_listen_aview, router_lident
					, rident_arr, rootca_arr, router_acl, ip_netaddr_arr);
	if( router_err.failed() ){
		KLOG_ERR("Could not init neoip_routerd due to " << router_err);
		return router_err;
	}

	// init the router_acache from the files
	router_acache_t	router_acache;
	router_err	= router_apps_helper_t::acache_load(lib_session->cache_rootdir(), router_acache);
	if( router_err.failed() )	return router_err;
	// set the just read router_acache_t in the router_peer_t
	router_peer->set_acache(router_acache);

	// if the arg_option DOES NOT contains the "nodaemon" key, pass daemon
	if( !arg_option.contain_key("nodaemon") && lib_apps_helper_t::daemonize().failed() )
		return router_err_t(router_err_t::ERROR, "Can't daemon()");
	
	// create the pidfile
	// - NOTE: it MUST be done after the daemon() to get the proper pid
	lib_apps_helper_t::pidfile_create();
	
	// create the urlfile
	// - NOTE: it MUST be done after the daemon() to get the proper pid
	lib_apps_helper_t::urlfile_create();

	// return no error
	return router_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			some cmdline actions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Handle the --lident_prepare cmdline actions
 */
router_err_t	router_apps_t::cmdline_action_lident_prepare()		throw()
{
	return router_err_t(router_err_t::ERROR, "not yet implemented");
}

/** \brief Handle the --lident_register cmdline actions
 */
router_err_t	router_apps_t::cmdline_action_lident_register()		throw()
{
	lib_session_t *		lib_session	= lib_session_get();
	lib_apps_t *		lib_apps	= lib_session->lib_apps();
	const strvar_db_t &	arg_option	= lib_apps->arg_option();
	file_path_t		config_path	= lib_session->conf_rootdir() / "router";
	const std::string &	reg_param	= arg_option.get_first_value("lident_register");
	router_name_t		dnsname		= router_name_t(dns_helper_t::idna_to_ascii(reg_param));
	router_err_t		router_err;
	file_err_t		file_err;
	x509_cert_t		ca_cert;
	x509_privkey_t		ca_privkey;
	// sanity check - the arg_option MUST contain the "lident_register" key
	DBG_ASSERT( arg_option.contain_key("lident_register") );
	
	KLOG_ERR("reg_param=" << reg_param);
	// display some info for the user
	KLOG_STDOUT("Registering host name " << dnsname << ". If the cryptographic key generation is too long, generate entropy by moving the mouse or using the disk\n");
	KLOG_STDOUT("Start registering peername " << dnsname << " ... ");

	// check that the dnsname is either is_fully_qualified() or is_host_only()
	if( !dnsname.is_fully_qualified() && !dnsname.is_host_only() ){
		std::string	reason = "dnsname " + dnsname.to_string() + " is invalid (only host-only and fully qualified are allowed)";
		return router_err_t(router_err_t::ERROR, reason);
	}

	// if dnsname to register is_authsigned_ok(), get cert/priv for the domain authority 
	if( dnsname.is_authsigned_ok() ){
		// try to get the ca_cert/ca_privkey for this dnsname
		router_err	= router_apps_helper_t::rootca_load_for_authsign(dnsname
							, ca_cert, ca_privkey);
		if( router_err.failed() )	return router_err;
		// sanity check - ca_cert and ca_privkey MUST NOT be null
		DBG_ASSERT( !ca_cert.is_null() );
		DBG_ASSERT( !ca_privkey.is_null() );
	}

	// log to debug
	KLOG_ERR("dnsname="		<< dnsname);
	KLOG_DBG("x509_cert="		<< ca_cert);
	KLOG_DBG("x509_privkey="	<< ca_privkey);
	
	// start generating the local identity
	router_err	= router_apps_helper_t::lident_register(config_path, dnsname, 1024
						, ca_cert, ca_privkey);
	if( router_err.failed() )	return router_err;

	// exit after a successfull action
	KLOG_STDOUT("OK\n");

	// return no error, but a specific reason to avoid launching the apps
	return router_err_t(router_err_t::OK, "DONTLAUNCHAPPS");
}

/** \brief Handle the --rootca_create cmdline actions
 */
router_err_t	router_apps_t::cmdline_action_rootca_create()	throw()
{
	const strvar_db_t &	arg_option	= lib_session_get()->lib_apps()->arg_option();
	file_path_t		config_path	= lib_session_get()->conf_rootdir() / "router";
	router_name_t		domain_name	= router_name_t(arg_option.get_first_value("rootca_create"));
	router_err_t		router_err;

	// sanity check - the arg_option MUST contain the "rootca_create" key
	DBG_ASSERT( arg_option.contain_key("rootca_create") );

	// if the domain_name IS NOT is_domain_only, return an error
	if( !domain_name.is_domain_only() )
		return router_err_t(router_err_t::ERROR, "Invalid domain name.");

	// display some info for the user
	KLOG_STDOUT("Creating rootca for domain " << domain_name << "...\n");
	KLOG_STDOUT("If the cryptographic key generation is too long, generate entropy by moving the mouse or using the disk\n");

	// build the router_rootca_t
	router_err	= router_apps_helper_t::rootca_create(config_path, domain_name, 1024);
	if( router_err.failed() )	return router_err;

	// exit after a successfull action
	KLOG_STDOUT("OK\n");
	// return no error, but a specific reason to avoid launching the apps
	return router_err_t(router_err_t::OK, "DONTLAUNCHAPPS");
}

/** \brief Handle the --info cmdline actions
 */
router_err_t	router_apps_t::cmdline_action_info()	throw()
{
	const strvar_db_t &	arg_option	= lib_session_get()->lib_apps()->arg_option();
	file_path_t		config_path	= lib_session_get()->conf_rootdir() / "router";
	router_err_t		router_err;

	// sanity check - the arg_option MUST contain the "info" key
	DBG_ASSERT( arg_option.contain_key("info") );

	// TODO may be nice to add some sanity check 
	// - e.g. is there any acl which hard no rootca/ *.rootca_cert file ?

	// display the info about the router_lident_t
	router_apps_helper_t::disp_info_lident();
	// display the info about the router_rident_arr_t
	router_apps_helper_t::disp_info_rident_arr();
	// display the info about the router_rootca_arr_t
	router_apps_helper_t::disp_info_rootca_arr();
	// display the info about the router_acl_t
	router_apps_helper_t::disp_info_acl();
	// display the info about the router_acache_t
	router_apps_helper_t::disp_info_acache();

	// return no error, but a specific reason to avoid launching the apps
	return router_err_t(router_err_t::OK, "DONTLAUNCHAPPS");
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the clineopt_arr_t for the cmdline option of the lib_session_t
 */
clineopt_arr_t	router_apps_t::clineopt_arr()	throw()
{
	clineopt_arr_t	clineopt_arr;
	clineopt_t	clineopt;
	// add the --lident_register cmdline option
	clineopt	= clineopt_t("lident_prepare", clineopt_mode_t::REQUIRED)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("To prepare the local identity peername (authsign only)");
	clineopt.alias_name_db().append("p");
	clineopt_arr	+= clineopt;
	// add the --lident_register cmdline option
	clineopt	= clineopt_t("lident_register", clineopt_mode_t::REQUIRED)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("To register the local identity peername");
	clineopt.alias_name_db().append("r");
	clineopt_arr	+= clineopt;
	// add the --rootca_create cmdline option
	clineopt	= clineopt_t("rootca_create", clineopt_mode_t::REQUIRED)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("Create a router_rootca_t");
	clineopt_arr	+= clineopt;
	// add the --info cmdline option
	clineopt	= clineopt_t("info", clineopt_mode_t::NOTHING)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("Display information on the configuration");
	clineopt.alias_name_db().append("i");
	clineopt_arr	+= clineopt;
	// add the --info-local-identity cmdline option
	clineopt	= clineopt_t("info-local-identity", clineopt_mode_t::NOTHING)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("Display information on the local identity");
	clineopt_arr	+= clineopt;
	// add the --info-remote-identity cmdline option
	clineopt	= clineopt_t("info-remote-identity", clineopt_mode_t::NOTHING)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("Display information on the remote identities");
	clineopt_arr	+= clineopt;
	// add the --info-rootca cmdline option
	clineopt	= clineopt_t("info-rootca", clineopt_mode_t::NOTHING)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("Display information on the rootca");
	clineopt_arr	+= clineopt;
	// add the --info-acl cmdline option
	clineopt	= clineopt_t("info-acl", clineopt_mode_t::NOTHING)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("Display information on the access control list");
	clineopt_arr	+= clineopt;
	// add the --info-acache cmdline option
	clineopt	= clineopt_t("info-acache", clineopt_mode_t::NOTHING)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("Display information on the address cache");
	clineopt_arr	+= clineopt;
	// add the --verbose cmdline option
	clineopt	= clineopt_t("verbose", clineopt_mode_t::NOTHING)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("Add more verbose details for the display");
	clineopt.alias_name_db().append("v");
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

