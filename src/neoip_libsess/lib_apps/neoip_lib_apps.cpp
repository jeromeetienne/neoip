/*! \file
    \brief Implementation of the lib_apps_t

	
\par About the apps info
- exec_name: the basename of the executable
- human_name: a human readable version of the apps name
- canon_name: a cannonical version of the apps name (no '-' in it, only '_')
- summary: a short (one line) description of the apps purpose
- longdesc: a long (possibly multiline) description of the apps purpose
- apps_type: the type of apps (see apps_type_t)

\par TODO
- the separation between the lib_apps_t and the lib_session_t is unclear
  - in theory there is only one lib_apps_t per apps and may have multiple
    lib_session_t 
  - but in practice only one lib_session_t can be done because i didnt 
    want to track the lib_session everywhere
  - i dunno how to fix this

*/

/* system include */
#include <signal.h>
#include <stdlib.h>
/* local include */
#include "neoip_lib_apps.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_libsess_err.hpp"
#include "neoip_clineopt_helper.hpp"
#include "neoip_clineopt_arr.hpp"
#include "neoip_file_path.hpp"

NEOIP_NAMESPACE_BEGIN;

// Prototype definition
static void signal_hd( int signum );

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                   CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief contructor of the lib_apps_t
 */
lib_apps_t::lib_apps_t() throw()
{
}

/** \brief destructor of the lib_apps_t
 */
lib_apps_t::~lib_apps_t() throw()
{
	// deinit the lib_session_t if started
	if( lib_session_get() )	lib_session_deinit();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       start function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
lib_apps_t &	lib_apps_t::set_profile(const lib_apps_profile_t &profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check().succeed() );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief start the apps
 */
libsess_err_t	lib_apps_t::start(int argc, char **argv, const clineopt_arr_t &apps_clineopt_arr
			, const std::string &m_canon_name, const std::string &m_human_name
			, const std::string &m_version, const std::string &m_summary
			, const std::string &m_longdesc, const apps_type_t &m_apps_type) throw()
{	
	// copy the parameter
	this->m_canon_name	= m_canon_name;
	this->m_human_name	= m_human_name;
	this->m_version		= m_version;
	this->m_summary		= m_summary;
	this->m_longdesc	= m_longdesc;
	this->m_apps_type	= m_apps_type;
	
	// sanity check - all parameters MUST be set
	DBG_ASSERT( !canon_name().empty() );
	DBG_ASSERT( !human_name().empty() );
	DBG_ASSERT( !version().empty() );
	DBG_ASSERT( !summary().empty() );
	DBG_ASSERT( !longdesc().empty() );
	DBG_ASSERT( !apps_type().is_null() );

#if 0	// TODO fixup this issue... *apparently* setting this or not under window
	// didnt change the result. so i manually change the OS preference
	// - obviously this is crap :)
	// - as seens in neoip-nunit "all/base/string" which is failing with "1,0" 
	//   on cout/cin 
	
	// set 'locale' to the default C one
	// - without that the operating system may use display float(1.0) as "1,0"
	// - i didnt experience it under linux but did under _WIN32. likely because
	//   i run linux under english default
	// - as i do it only for float/double, maybe i should limit this to LC_NUMERIC ?
	setlocale(LC_ALL, "C" );
#endif	

	// parse the command line
	clineopt_err_t	clineopt_err;
	clineopt_err	= parse_cmdline(argc, argv, apps_clineopt_arr);

	// display the short help if the clineopt is in the cmdline
	if( m_arg_option.contain_key("help") ){
		// build the full clineopt_arr
		clineopt_arr_t	clineopt_arr	= apps_clineopt_arr;
		clineopt_arr	+= lib_session_t::clineopt_arr();
		clineopt_arr	+= neutral_clineopt_arr();
		// display the help
		display_help(clineopt_arr);
		// notify the caller not to launch the apps
		return libsess_err_t(libsess_err_t::OK, "DONTLAUNCHAPPS");
	}

	// display the version if the clineopt is in the cmdline
	if( m_arg_option.contain_key("version") ){
		// display the version
		display_version();
		// notify the caller not to launch the apps
		return libsess_err_t(libsess_err_t::OK, "DONTLAUNCHAPPS");
	}

	// if the cmdline parsing failed, return an error
	if( clineopt_err.failed() )	return libsess_err_from_clineopt(clineopt_err);

	// initialize the lib_session
	if( lib_session_init() )	return libsess_err_t(libsess_err_t::ERROR, "lib_session_init() failed!!!");
	libsess_err_t	libsess_err;
	libsess_err	= lib_session_get()->set_profile(profile.lib_session()).start(this);
	if( libsess_err.failed() )	return libsess_err;

	// unbuffer the output just to debug
	setbuf(stderr, NULL); setbuf(stdout, NULL);
	/* init signal handler for stun shutdown */
	signal( SIGINT, signal_hd );
	signal( SIGTERM, signal_hd );

#ifndef _WIN32	// no sigpipe in mingw
	// NOTE: added to run only ELOOP_LEVT
	// - it was not needed for ELOOP_GLIB, likely glib use it for itself
	signal( SIGPIPE, SIG_IGN );
#endif

	// put a 'random' seed
	srand(time(NULL));
	// return no error
	return libsess_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse the commandline
 */
clineopt_err_t	lib_apps_t::parse_cmdline(int argc, char **argv
				, const clineopt_arr_t &apps_clineopt_arr)	throw()
{
	clineopt_err_t	clineopt_err;
	// convert the argc/argv parameter into a std::vector<std::string>
	std::vector<std::string>	arg_arr;
	for(int i = 0; i < argc; i++)	arg_arr.push_back(argv[i]);
	// add the external clineopt_arr_t and the lib_session_t one
	clineopt_arr_t	clineopt_arr	= apps_clineopt_arr;
	clineopt_arr	+= lib_session_t::clineopt_arr();
	clineopt_arr	+= neutral_clineopt_arr();

	// set the exec_name with the arg_arr[0] basename
	m_exec_name	= file_path_t(arg_arr[0]).basename().to_os_path_string();
	arg_arr.erase( arg_arr.begin() );

	// parse the options
	do{
		// parse the command line
		clineopt_err	= clineopt_helper_t::parse(arg_arr, m_arg_option, clineopt_arr);
		if( clineopt_err.failed() )	break;
		// append all the cmdline_opt from the envvar if it exist
		clineopt_err	= parse_cmdline_from_envvar(clineopt_arr);
		if( clineopt_err.failed() )	break;
		// check if the all mandatory options are present 
		clineopt_err	= clineopt_helper_t::check(m_arg_option, clineopt_arr);
		if( clineopt_err.failed() )	break;
	}while(0);
	// log to debug
	KLOG_DBG("m_arg_option=" << m_arg_option );
	
	// copy the arg_arr into the m_arg_remain
	m_arg_remain	= arg_arr;
	
	// return the error
	return clineopt_err;
}

/** \brief parse the envvar for the cmdline_opt
 * 
 * - NOTE: the env_var is of the form NEOIP_OLOAD_CMDLINE_OPT for neoip-oload
 *   - e.g. export NEOIP_OLOAD_CMDLINE_OPT="-x 50k"
 */
clineopt_err_t	lib_apps_t::parse_cmdline_from_envvar(const clineopt_arr_t &clineopt_arr)
									 	throw()
{
	std::string	envvar_key	= string_t::to_upper(canon_name()) + "_CMDLINE_OPT";
	char *		envvar_val	= getenv(envvar_key.c_str());
	// log to debug
	KLOG_DBG("envvar_key="<< envvar_key);
	// if there are no envvar, return now
	if( !envvar_val )	return clineopt_err_t::OK;

	// parse the envvar_val as a argv array
	std::vector<std::string>	env_arr	= string_t::parse_as_argv(envvar_val);
	// parse the env_arr with the clineopt_arr
	clineopt_err_t	clineopt_err;
	clineopt_err	= clineopt_helper_t::parse(env_arr, m_arg_option, clineopt_arr);
	return clineopt_err;
}


/** \brief Return the clineopt_arr_t for the neutral options
 * 
 * - a neutral option is one which is not spefific to the lib_session_t and specific
 *   to a given application
 */
clineopt_arr_t	lib_apps_t::neutral_clineopt_arr()	throw()
{
	clineopt_arr_t	clineopt_arr;
	clineopt_t	clineopt;
	// add the --help cmdline neutral option
	clineopt	= clineopt_t("help", clineopt_mode_t::NOTHING)
					.option_mode(clineopt_mode_t::OPTIONAL)
					.help_string("Display the short help");
	clineopt.alias_name_db().append("h");
	clineopt_arr	+= clineopt;
	// add the --version cmdline neutral option
	clineopt	= clineopt_t("version", clineopt_mode_t::NOTHING)
					.option_mode(clineopt_mode_t::OPTIONAL)
					.help_string("Display the version of the program");
	clineopt_arr	+= clineopt;
	// return the just built clineopt_arr_t
	return clineopt_arr;
}

/** \brief Display the inlined help
 */
void	lib_apps_t::display_help(const clineopt_arr_t &clineopt_arr)	throw()
{
	// display a inlined help
	KLOG_STDERR(human_name() << " - " << summary() << "\n");
	KLOG_STDERR(canon_name() << " version " << version() << " - list of possible options:\n" );  
	KLOG_STDERR(clineopt_helper_t::help_string(clineopt_arr) );	
	KLOG_STDERR(longdesc() << "\n");
}

/** \brief Display the version
 */
void	lib_apps_t::display_version()					throw()
{
	KLOG_STDERR(version()	<< " (Compilation date:" << std::string(__DATE__)
				<< " " << std::string(__TIME__) << ")\n");	
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       unix signal handler
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief unix signal handler
 */
static void signal_hd( int signum )
{
	lib_session_t *	lib_session	= lib_session_get();
	// reinstall the signal immediatly
	signal( signum, signal_hd );
	// if the lib_session_t is already in stopping, stop now
	if( lib_session->loop_stopping() ){
		lib_session->loop_stop_now();
	}else{
		// warn the lib_session_t that the user wish to interrupt
		lib_session->loop_stop_asap();
	}
}

NEOIP_NAMESPACE_END


