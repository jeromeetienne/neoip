/*! \file
    \brief Definition of the nunit_suite_t

*/

/* system include */
/* local include */
#include "neoip_ntudp_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

#include "neoip_ntudp_addr_nunit.hpp"
#include "neoip_ntudp_rdvpt_nunit.hpp"
#include "neoip_ntudp_npos_cli_nunit.hpp"
#include "neoip_ntudp_npos_eval_nunit.hpp"
#include "neoip_ntudp_aview_pool_nunit.hpp"
#include "neoip_ntudp_pserver_pool_nunit.hpp"
#include "neoip_ntudp_itor_retry_nunit.hpp"
#include "neoip_ntudp_client_nunit.hpp"
#include "neoip_ntudp_socket_direct_nunit.hpp"
#include "neoip_ntudp_socket_estarelay_nunit.hpp"
#include "neoip_ntudp_socket_reverse_nunit.hpp"
#include "neoip_ntudp_socket_error_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit_gsuite_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Implement a nunit_gsuite_fct_t for the ntudp layer
 */
static void ntudp_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	ntudp_suite	= nipmem_new nunit_suite_t("ntudp");	
	// log to debug
	KLOG_DBG("enter");

/********************* ntudp_addr_t	***************************************/
	// init the testclass for the ntudp_addr_t
	nunit_testclass_t<ntudp_addr_testclass_t> *	ntudp_addr_testclass;
	ntudp_addr_testclass	= nipmem_new nunit_testclass_t<ntudp_addr_testclass_t>("ntudp_addr_t"
							, nipmem_new ntudp_addr_testclass_t());
	// add some test functions
	ntudp_addr_testclass->append("serial_consistency", &ntudp_addr_testclass_t::serial_consistency);
	ntudp_addr_testclass->append("ctor_from_string"	, &ntudp_addr_testclass_t::ctor_from_string);
	// add the testclass to the nunit_suite
	ntudp_suite->append(ntudp_addr_testclass);

/********************* ntudp_rdvpt_t	***************************************/
	// init the testclass for the ntudp_rdvpt_t
	nunit_testclass_t<ntudp_rdvpt_testclass_t> *	ntudp_rdvpt_testclass;
	ntudp_rdvpt_testclass	= nipmem_new nunit_testclass_t<ntudp_rdvpt_testclass_t>("ntudp_rdvpt_t"
							, nipmem_new ntudp_rdvpt_testclass_t());
	// add some test functions
	ntudp_rdvpt_testclass->append("serial_consistency"	, &ntudp_rdvpt_testclass_t::serial_consistency);
	// add the testclass to the nunit_suite
	ntudp_suite->append(ntudp_rdvpt_testclass);

/********************* ntudp_npos_cli_t	***************************************/
	// init the testclass for the ntudp_npos_cli_t
	nunit_testclass_t<ntudp_npos_cli_testclass_t> *	ntudp_npos_testclass;
	ntudp_npos_testclass	= nipmem_new nunit_testclass_t<ntudp_npos_cli_testclass_t>("npos_cli"
							, nipmem_new ntudp_npos_cli_testclass_t());
	// add some test functions
	ntudp_npos_testclass->append("inetreach"	, &ntudp_npos_cli_testclass_t::inetreach);
	ntudp_npos_testclass->append("saddrecho"	, &ntudp_npos_cli_testclass_t::saddrecho);
	ntudp_npos_testclass->append("natlback"		, &ntudp_npos_cli_testclass_t::natlback);
	// add the testclass to the nunit_suite
	ntudp_suite->append(ntudp_npos_testclass);	

/********************* ntudp_npos_eval_t	***************************************/
	// init the testclass for the ntudp_npos_eval_t
	nunit_testclass_t<ntudp_npos_eval_testclass_t> *	ntudp_npos_eval_testclass;
	ntudp_npos_eval_testclass	= nipmem_new nunit_testclass_t<ntudp_npos_eval_testclass_t>("npos_eval"
							, nipmem_new ntudp_npos_eval_testclass_t());
	// add some test functions
	ntudp_npos_eval_testclass->append("general"	, &ntudp_npos_eval_testclass_t::general);
	// add the testclass to the nunit_suite
	ntudp_suite->append(ntudp_npos_eval_testclass);

/********************* ntudp_aview_pool_t	*******************************/
	// init the testclass for the ntudp_aview_pool_t
	nunit_testclass_t<ntudp_aview_pool_testclass_t> *	ntudp_aview_pool_testclass;
	ntudp_aview_pool_testclass	= nipmem_new nunit_testclass_t<ntudp_aview_pool_testclass_t>("aview_pool"
							, nipmem_new ntudp_aview_pool_testclass_t());
	// add some test functions
	ntudp_aview_pool_testclass->append("get_5_aview"	, &ntudp_aview_pool_testclass_t::get_5_aview);
	// add the testclass to the nunit_suite
	ntudp_suite->append(ntudp_aview_pool_testclass);

/********************* ntudp_pserver_pool_t	***************************************/
	// init the testclass for the ntudp_pserver_pool_t
	nunit_testclass_t<ntudp_pserver_pool_testclass_t> *	ntudp_pserver_pool_testclass;
	ntudp_pserver_pool_testclass	= nipmem_new nunit_testclass_t<ntudp_pserver_pool_testclass_t>("pserver_pool"
							, nipmem_new ntudp_pserver_pool_testclass_t());
	// add some test functions
	ntudp_pserver_pool_testclass->append("general"	, &ntudp_pserver_pool_testclass_t::general);
	// add the testclass to the nunit_suite
	ntudp_suite->append(ntudp_pserver_pool_testclass);

/********************* ntudp_itor_retry_t	***************************************/
	// init the testclass for the ntudp_itor_retry_t
	nunit_testclass_t<ntudp_itor_retry_testclass_t> *	ntudp_itor_retry_testclass;
	ntudp_itor_retry_testclass = nipmem_new nunit_testclass_t<ntudp_itor_retry_testclass_t>("itor_retry"
							, nipmem_new ntudp_itor_retry_testclass_t());
	// add some test functions
	ntudp_itor_retry_testclass->append("general"	, &ntudp_itor_retry_testclass_t::general);
	// add the testclass to the nunit_suite
	ntudp_suite->append(ntudp_itor_retry_testclass);

/********************* ntudp_client_t	***************************************/
	// init the testclass for the ntudp_client_t
	nunit_testclass_t<ntudp_client_testclass_t> *	ntudp_client_testclass;
	ntudp_client_testclass = nipmem_new nunit_testclass_t<ntudp_client_testclass_t>("client"
							, nipmem_new ntudp_client_testclass_t());
	// add some test functions
	ntudp_client_testclass->append("general"	, &ntudp_client_testclass_t::general);
	// add the testclass to the nunit_suite
	ntudp_suite->append(ntudp_client_testclass);

/********************* ntudp_socket_direct_t	***************************************/
	// init the testclass for the ntudp_socket_direct_t
	nunit_testclass_t<ntudp_socket_direct_testclass_t> *	ntudp_socket_direct_testclass;
	ntudp_socket_direct_testclass	= nipmem_new nunit_testclass_t<ntudp_socket_direct_testclass_t>("socket_direct"
							, nipmem_new ntudp_socket_direct_testclass_t());
	// add some test functions
	ntudp_socket_direct_testclass->append("one_client"	, &ntudp_socket_direct_testclass_t::one_client);
	ntudp_socket_direct_testclass->append("ten_client"	, &ntudp_socket_direct_testclass_t::ten_client);
	// add the testclass to the nunit_suite
	ntudp_suite->append(ntudp_socket_direct_testclass);

/********************* ntudp_socket_estarelay_t	***************************************/
	// init the testclass for the ntudp_socket_estarelay_t
	nunit_testclass_t<ntudp_socket_estarelay_testclass_t> *	ntudp_socket_estarelay_testclass;
	ntudp_socket_estarelay_testclass	= nipmem_new nunit_testclass_t<ntudp_socket_estarelay_testclass_t>("socket_estarelay"
							, nipmem_new ntudp_socket_estarelay_testclass_t());
	// add some test functions
	ntudp_socket_estarelay_testclass->append("one_client"	, &ntudp_socket_estarelay_testclass_t::one_client);
	ntudp_socket_estarelay_testclass->append("ten_client"	, &ntudp_socket_estarelay_testclass_t::ten_client);
	// add the testclass to the nunit_suite
	ntudp_suite->append(ntudp_socket_estarelay_testclass);

/********************* ntudp_socket_reverse_t	***************************************/
	// init the testclass for the ntudp_socket_reverse_t
	nunit_testclass_t<ntudp_socket_reverse_testclass_t> *	ntudp_socket_reverse_testclass;
	ntudp_socket_reverse_testclass	= nipmem_new nunit_testclass_t<ntudp_socket_reverse_testclass_t>("socket_reverse"
							, nipmem_new ntudp_socket_reverse_testclass_t());
	// add some test functions
	ntudp_socket_reverse_testclass->append("one_client"	, &ntudp_socket_reverse_testclass_t::one_client);
	ntudp_socket_reverse_testclass->append("ten_client"	, &ntudp_socket_reverse_testclass_t::ten_client);
	// add the testclass to the nunit_suite
	ntudp_suite->append(ntudp_socket_reverse_testclass);

/********************* ntudp_socket_error_t	***************************************/
	// init the testclass for the ntudp_socket_error_t
	nunit_testclass_t<ntudp_socket_error_testclass_t> *	ntudp_socket_error_testclass;
	ntudp_socket_error_testclass	= nipmem_new nunit_testclass_t<ntudp_socket_error_testclass_t>("socket_error"
							, nipmem_new ntudp_socket_error_testclass_t());
	// add some test functions
	ntudp_socket_error_testclass->append("unknown_host"	, &ntudp_socket_error_testclass_t::unknown_host);
	ntudp_socket_error_testclass->append("no_route_to_host"	, &ntudp_socket_error_testclass_t::no_route_to_host);
	// add the testclass to the nunit_suite
	ntudp_suite->append(ntudp_socket_error_testclass);

	// add the ntudp_suite to the nunit_suite
	nunit_suite.append(ntudp_suite);	
}

// definition of the nunit_gsuite_t for ntudp
NUNIT_GSUITE_DEFINITION(ntudp_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(ntudp_nunit_gsuite, 0, ntudp_gsuite_fct);


NEOIP_NAMESPACE_END




