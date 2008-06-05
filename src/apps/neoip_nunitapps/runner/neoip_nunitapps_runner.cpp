/*! \file
    \brief Definition of the \ref nunitapps_runner_t class
    
*/

/* system include */
/* local include */
#include "neoip_nunitapps_runner.hpp"
#include "neoip_nunit.hpp"
#include "neoip_nunit_gsuite.hpp"
#include "neoip_clineopt_arr.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_lib_session_init.hpp"
#include "neoip_lib_session_exit.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

nunitapps_runner_t::nunitapps_runner_t(const nunit_path_t &path_pattern
					, nunit_gsuite_t *nunit_gsuite)		throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// init the nunit_suite
	nunit_suite	= nipmem_new nunit_suite_t("all");
	// populate the nunit_suite from the nunit_gsuite_t
	nunit_gsuite->populate(*nunit_suite);

	// Start a nunit_runner_t on the just built nunit_suite
	nunit_runner	= nipmem_new nunit_runner_t();
	nunit_runner->start(path_pattern, nunit_suite, this, NULL);
}
	
/** \brief Destructor
 */
nunitapps_runner_t::~nunitapps_runner_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// deinit the nunit_suite
	nipmem_zdelete	nunit_suite;
	// deinit the nunit_runner
	nipmem_zdelete	nunit_runner;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the clineopt_arr_t for the cmdline option of the lib_session_t
 */
clineopt_arr_t	nunitapps_runner_t::clineopt_arr()	throw()
{
	clineopt_arr_t	clineopt_arr;
	// return the just built clineopt_arr_t
	return clineopt_arr;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 nunit_runner_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief called when a nunit_runner_t is completed
 */
bool nunitapps_runner_t::neoip_nunit_runner_completed_cb(void *userptr, nunit_runner_t &nunit_runner)	throw()
{
	// log to debug
	KLOG_ERR("nunit_runner completed");
	// delete nunit_suite
	nipmem_zdelete	nunit_suite;
	// Stop the event loop
	lib_session_get()->loop_stop_asap();
	// return 'dontkeep'
	return false;
}

NEOIP_NAMESPACE_END

