/*! \file
    \brief Definition of the \ref casti_ctrl_cline_t

\par Brief Description
- This object is made to allow control via cmdline.
- it allows a single casti_swarm_t to be launched
  - nothing specific to the neoip-casti. just it is hard to build a cmdline
    language to be able to run all the swarm access parameter for multiple
    casti_swarm_t

*/

/* system include */
/* local include */
#include "neoip_casti_ctrl_cline.hpp"
#include "neoip_casti_apps.hpp"
#include "neoip_casti_swarm.hpp"
#include "neoip_casti_swarm_arg.hpp"

#include "neoip_bt_ezswarm_state.hpp"

#include "neoip_lib_apps.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_clineopt_arr.hpp"

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
casti_ctrl_cline_t::casti_ctrl_cline_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
}

/** \brief Destructor
 */
casti_ctrl_cline_t::~casti_ctrl_cline_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	casti_ctrl_cline_t::start(casti_apps_t *m_casti_apps)		throw()
{
	lib_apps_t *		lib_apps	= lib_session_get()->lib_apps();
	const strvar_db_t &	arg_option	= lib_apps->arg_option();
	bt_err_t		bt_err;
	// copy the parameter
	this->m_casti_apps	= m_casti_apps;

	// if cmdline option scasti_uri is not present, do nothing
	if( !arg_option.contain_key("scasti_uri") )	return bt_err_t::OK;

	// check the validity of the parameters
	casti_swarm_arg_t	swarm_arg;
	swarm_arg	= build_swarm_arg();
	bt_err		= swarm_arg.check();
	KLOG_ERR("bt_err=" << bt_err);
	if( bt_err.failed() )	return bt_err;

	// start the relaunch_swarm
	refresh_timeout.start(delay_t(0), this, NULL);

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build the casti_swarm_arg_t based on the lib_apps->arg_option
 */
casti_swarm_arg_t	casti_ctrl_cline_t::build_swarm_arg()	throw()
{
	lib_apps_t *		lib_apps	= lib_session_get()->lib_apps();
	const strvar_db_t &	arg_option	= lib_apps->arg_option();
	casti_swarm_arg_t	swarm_arg	= m_casti_apps->swarm_arg_default();
	// populate the casti_swarm_arg_t witht the arg_option
	swarm_arg.cast_name	(arg_option.get_first_value("cast_name"));
	swarm_arg.cast_privtext	(arg_option.get_first_value("cast_privtext"));
	swarm_arg.scasti_uri	(arg_option.get_first_value("scasti_uri"));
	if( arg_option.contain_key("mdata_srv_uri") )
		swarm_arg.mdata_srv_uri		(arg_option.get_first_value("mdata_srv_uri"));
	if( arg_option.contain_key("scasti_mod") )
		swarm_arg.scasti_mod		(arg_option.get_first_value("scasti_mod"));
	if( arg_option.contain_key("http_peersrc_uri") )
		swarm_arg.http_peersrc_uri	(arg_option.get_first_value("http_peersrc_uri"));
	// return the resulting object
	return swarm_arg;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			timeout_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 *
 */
bool	casti_ctrl_cline_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	casti_swarm_arg_t	swarm_arg	= build_swarm_arg();
	casti_swarm_t *		casti_swarm;
	bt_err_t		bt_err;
	// log to debug
	KLOG_DBG("enter");

	// sanity check - the swarm_arg MUST BE check().succeed()
	DBG_ASSERT( swarm_arg.check().succeed() );

	// start the relaunch_swarm
	refresh_timeout.start(m_casti_apps->profile().relaunch_swarm_period(), this, NULL);

	// try to get the casti_swarm for this cast_name
	casti_swarm	= m_casti_apps->swarm_by(swarm_arg.mdata_srv_uri(), swarm_arg.cast_name()
						, swarm_arg.cast_privtext());

	// if the casti_swarm already exists, do nothing
	if( casti_swarm )	return true;

	// launch the casti_swarm_t
	casti_swarm	= nipmem_new casti_swarm_t();
	bt_err		= casti_swarm->start(swarm_arg);
	if( bt_err.failed() ){
		nipmem_zdelete	casti_swarm;
		KLOG_ERR("cant launch casti_swarm due to "<< bt_err);
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
clineopt_arr_t	casti_ctrl_cline_t::clineopt_arr()	throw()
{
	clineopt_arr_t	clineopt_arr;
	clineopt_t	clineopt;
	// add the --cast_name cmdline option
	clineopt	= clineopt_t("cast_name", clineopt_mode_t::REQUIRED)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("Specify the cast_name of the broadcast."
					"\n\t\trequired to access swarm by cmdline");
	clineopt_arr	+= clineopt;
	// add the --cast_name cmdline option
	clineopt	= clineopt_t("cast_privtext", clineopt_mode_t::REQUIRED)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("Specify the cast_privtext of the broadcast."
					"\n\t\trequired to access swarm by cmdline");
	clineopt_arr	+= clineopt;
	// add the --mdata_srv_uri cmdline option
	clineopt	= clineopt_t("mdata_srv_uri", clineopt_mode_t::REQUIRED)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("specify the mdata_srv_uri where to publish the broadcast metadata");
	clineopt_arr	+= clineopt;
	// add the --scasti_uri cmdline option
	clineopt	= clineopt_t("scasti_uri", clineopt_mode_t::REQUIRED)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("specify the uri of the http stream to be broadcasted."
					"\n\t\trequired to access swarm by cmdline");
	clineopt_arr	+= clineopt;
	// add the --scasti_mod cmdline option
	clineopt	= clineopt_t("scasti_mod", clineopt_mode_t::REQUIRED)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("To set the 'scasti_mod' of the stream [raw|flv]. defaults to raw");
	clineopt_arr	+= clineopt;
	// add the --http_peersrc_uri cmdline option
	clineopt	= clineopt_t("http_peersrc_uri", clineopt_mode_t::REQUIRED)
				.option_mode(clineopt_mode_t::OPTIONAL)
				.help_string("specify the uri of a peersrc");
	clineopt_arr	+= clineopt;
	// return the just built clineopt_arr_t
	return clineopt_arr;
}


NEOIP_NAMESPACE_END;




