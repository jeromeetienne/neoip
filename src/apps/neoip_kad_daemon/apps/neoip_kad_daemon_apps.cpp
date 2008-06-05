/*! \file
    \brief Definition of the \ref kad_daemon_apps_t class

*/

/* system include */
/* local include */
#include "neoip_kad_daemon_apps.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_kad_rpcresp.hpp"
#include "neoip_clineopt_arr.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_lib_apps.hpp"
#include "neoip_lib_apps_helper.hpp"
#include "neoip_strvar_db.hpp"
#include "neoip_strvar_helper.hpp"
#include "neoip_file_path.hpp"
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
kad_daemon_apps_t::kad_daemon_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some fields
	kad_rpcresp	= NULL;
}
	
/** \brief Destructor
 */
kad_daemon_apps_t::~kad_daemon_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the pidfile and urlfile if needed
	if( kad_rpcresp ){
		lib_apps_helper_t::pidfile_remove();
		lib_apps_helper_t::urlfile_remove();
	}
	// delete kad_rpcresp_t if needed
	nipmem_zdelete	kad_rpcresp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
kad_err_t	kad_daemon_apps_t::start()	throw()
{
	lib_session_t *		lib_session	= lib_session_get();
	lib_apps_t *		lib_apps	= lib_session->lib_apps();
	const strvar_db_t &	arg_option	= lib_apps->arg_option();
		
	// init and start kad_rpcresp_t
	kad_err_t	kad_err;
	kad_rpcresp	= nipmem_new kad_rpcresp_t();
	kad_err		= kad_rpcresp->start("/kadrpc");
	if( kad_err.failed() ){
		KLOG_ERR("Could not init neoip_routerd due to " << kad_err);
		return kad_err;
	}

	// if the arg_option DOES NOTcontains the "nodaemon" key, pass daemon
	if( !arg_option.contain_key("nodaemon") && lib_apps_helper_t::daemonize().failed() )
		return kad_err_t(kad_err_t::ERROR, "Can't daemon()");
	
	// create the pidfile
	// - NOTE: it MUST be done after the daemon() to get the proper pid
	lib_apps_helper_t::pidfile_create();
	
	// create the urlfile
	// - NOTE: it MUST be done after the daemon() to get the proper pid
	lib_apps_helper_t::urlfile_create();

	// return no error
	return kad_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the clineopt_arr_t for the cmdline option of the lib_session_t
 */
clineopt_arr_t	kad_daemon_apps_t::clineopt_arr()	throw()
{
	clineopt_arr_t	clineopt_arr;
	clineopt_t	clineopt;
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

