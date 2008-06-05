/*! \file
    \brief Definition of the unit test for the \ref clineopt_t

*/

/* system include */
#include <vector>
#include <string>
/* local include */
#include "neoip_clineopt_nunit.hpp"
#include "neoip_clineopt_helper.hpp"
#include "neoip_clineopt_arr.hpp"
#include "neoip_clineopt_err.hpp"
#include "neoip_strvar_db.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	clineopt_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor)	throw()
{
	// log to debug
	KLOG_ERR("enter");

	strvar_db_t			strvar_db;
	clineopt_arr_t		clineopt_arr;
	std::vector<std::string>	arg;

	clineopt_arr	+= clineopt_t("type", clineopt_mode_t::REQUIRED);
	clineopt_arr	+= clineopt_t("c", clineopt_mode_t::REQUIRED);
//	clineopt_arr	+= clineopt_t("config-file", 1);
//	clineopt_arr	+= clineopt_t("t", 1);
//	clineopt_arr	+= clineopt_t("resumedata", 0);
//	clineopt_arr	+= clineopt_t("metalink", 0);

	arg.push_back("--type");
	arg.push_back("joujoue");
	arg.push_back("-c");
	arg.push_back("superdir");
	arg.push_back("superfile.torrent");

	// parse the command line options
	clineopt_err_t	clineopt_err;
	clineopt_err	= clineopt_helper_t::parse(arg, strvar_db, clineopt_arr);
	KLOG_ERR("clineopt_err=" << clineopt_err);
	NUNIT_ASSERT( clineopt_err.succeed() );
	
	KLOG_ERR("strvar_db="		<< strvar_db);
	KLOG_ERR("clineopt_arr="	<< clineopt_arr);
	for(size_t i = 0; i < arg.size(); i++)	KLOG_ERR("i=" << i << " arg=" << arg[i]);
	
	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END

