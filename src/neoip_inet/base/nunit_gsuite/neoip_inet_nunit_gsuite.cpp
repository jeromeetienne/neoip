/*! \file
    \brief Definition of the nunit_gsuite_t for the inet_t layer

*/

/* system include */
/* local include */
#include "neoip_inet_nunit_gsuite.hpp"
#include "neoip_nunit.hpp"

#include "neoip_ip_addr_nunit.hpp"
#include "neoip_ip_netaddr_nunit.hpp"
#include "neoip_ipport_addr_nunit.hpp"
#include "neoip_ipport_aview_nunit.hpp"
#include "neoip_ipport_strlist_src_nunit.hpp"
#include "neoip_tcp_client_nunit.hpp"
#include "neoip_tcp_chargen_nunit.hpp"
#include "neoip_udp_client_nunit.hpp"
#include "neoip_host2ip_nunit.hpp"
#include "neoip_ipcountry_nunit.hpp"
#include "neoip_nslan_nunit.hpp"
#include "neoip_nslan_rec_src_nunit.hpp"
#include "neoip_nudp_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief Implement a nunit_gsuite_fct_t for timeout_t
 */
static void inet_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	inet_suite	= nipmem_new nunit_suite_t("inet");		
	// log to debug
	KLOG_DBG("enter");

/********************* ip_addr_t	***************************************/
	// init the testclass for the ip_addr_t
	nunit_testclass_t<ip_addr_testclass_t> *	ip_addr_testclass;
	ip_addr_testclass	= nipmem_new nunit_testclass_t<ip_addr_testclass_t>("ip_addr", nipmem_new ip_addr_testclass_t());
	// add some test functions
	ip_addr_testclass->append("convert_from_string"	, &ip_addr_testclass_t::convert_from_string);
	ip_addr_testclass->append("comparison_op"	, &ip_addr_testclass_t::comparison_op);
	ip_addr_testclass->append("serial_consistency"	, &ip_addr_testclass_t::serial_consistency);
	ip_addr_testclass->append("general"		, &ip_addr_testclass_t::general);
	// add the testclass to the nunit_suite
	inet_suite->append(ip_addr_testclass);

/********************* ip_netaddr_t	***************************************/
	// init the testclass for the ip_netaddr_t
	nunit_testclass_t<ip_netaddr_testclass_t> *	ip_netaddr_testclass;
	ip_netaddr_testclass	= nipmem_new nunit_testclass_t<ip_netaddr_testclass_t>("ip_netaddr", nipmem_new ip_netaddr_testclass_t());
	// add some test functions
	ip_netaddr_testclass->append("comparison_op"	, &ip_netaddr_testclass_t::comparison_op);
	ip_netaddr_testclass->append("serial_consistency", &ip_netaddr_testclass_t::serial_consistency);
	ip_netaddr_testclass->append("query_function"	, &ip_netaddr_testclass_t::query_function);
	// add the testclass to the nunit_suite
	inet_suite->append(ip_netaddr_testclass);

/********************* ipport_addr_t	***************************************/
	// init the testclass for the ipport_addr_t
	nunit_testclass_t<ipport_addr_testclass_t> *	ipport_addr_testclass;
	ipport_addr_testclass	= nipmem_new nunit_testclass_t<ipport_addr_testclass_t>("ipport_addr", nipmem_new ipport_addr_testclass_t());
	// add some test functions
	ipport_addr_testclass->append("comparison_op"	, &ipport_addr_testclass_t::comparison_op);
	ipport_addr_testclass->append("serial_consistency"	, &ipport_addr_testclass_t::serial_consistency);
	ipport_addr_testclass->append("query_function"	, &ipport_addr_testclass_t::query_function);
	// add the testclass to the nunit_suite
	inet_suite->append(ipport_addr_testclass);

/********************* ipport_aview_t	***************************************/
	// init the testclass for the ipport_aview_t
	nunit_testclass_t<ipport_aview_testclass_t> *	ipport_aview_testclass;
	ipport_aview_testclass	= nipmem_new nunit_testclass_t<ipport_aview_testclass_t>("ipport_aview", nipmem_new ipport_aview_testclass_t());
	// add some test functions
	ipport_aview_testclass->append("general"			, &ipport_aview_testclass_t::general);
	ipport_aview_testclass->append("xmlrpc_consitency"	, &ipport_aview_testclass_t::xmlrpc_consistency);
	// add the testclass to the nunit_suite
	inet_suite->append(ipport_aview_testclass);

/********************* ipport_strlist_src_t	***************************************/
	// init the testclass for the ipport_strlist_src_t
	nunit_testclass_t<ipport_strlist_src_testclass_t> *	ipport_strlist_src_testclass;
	ipport_strlist_src_testclass	= nipmem_new nunit_testclass_t<ipport_strlist_src_testclass_t>("ipport_strlist_src", nipmem_new ipport_strlist_src_testclass_t());
	// add some test functions
	ipport_strlist_src_testclass->append("general"	, &ipport_strlist_src_testclass_t::general);
	// add the testclass to the nunit_suite
	inet_suite->append(ipport_strlist_src_testclass);

/********************* tcp_client_t	***************************************/
	// init the testclass for the tcp_client_t
	nunit_testclass_t<tcp_client_testclass_t> *	tcp_client_testclass;
	tcp_client_testclass	= nipmem_new nunit_testclass_t<tcp_client_testclass_t>("tcp_client", nipmem_new tcp_client_testclass_t());
	// add some test functions
	tcp_client_testclass->append("cnx_establishement"	, &tcp_client_testclass_t::cnx_establishement);
	// add the testclass to the nunit_suite
	inet_suite->append(tcp_client_testclass);

/********************* tcp_chargen_t	***************************************/
	// init the testclass for the tcp_chargen_t
	nunit_testclass_t<tcp_chargen_testclass_t> *	tcp_chargen_testclass;
	tcp_chargen_testclass	= nipmem_new nunit_testclass_t<tcp_chargen_testclass_t>("tcp_chargen", nipmem_new tcp_chargen_testclass_t());
	// add some test functions
	tcp_chargen_testclass->append("general"	, &tcp_chargen_testclass_t::general);
	// add the testclass to the nunit_suite
	inet_suite->append(tcp_chargen_testclass);

#if 1
/********************* udp_client_t	***************************************/
	// init the testclass for the udp_client_t
	nunit_testclass_t<udp_client_testclass_t> *	udp_client_testclass;
	udp_client_testclass	= nipmem_new nunit_testclass_t<udp_client_testclass_t>("udp_client", nipmem_new udp_client_testclass_t());
	// add some test functions
	udp_client_testclass->append("echo_localhost_inetd"	, &udp_client_testclass_t::echo_localhost_inetd);
	// add the testclass to the nunit_suite
	inet_suite->append(udp_client_testclass);
#endif

/********************* host2ip_t	***************************************/
	// init the testclass for the host2ip_t
	nunit_testclass_t<host2ip_testclass_t> *	host2ip_testclass;
	host2ip_testclass	= nipmem_new nunit_testclass_t<host2ip_testclass_t>("host2ip", nipmem_new host2ip_testclass_t());
	// add some test functions
	host2ip_testclass->append("general"	, &host2ip_testclass_t::general);
	// add the testclass to the nunit_suite
	inet_suite->append(host2ip_testclass);

/********************* ipcountry_t	***************************************/
	// init the testclass for the ipcountry_t
	nunit_testclass_t<ipcountry_testclass_t> *	ipcountry_testclass;
	ipcountry_testclass	= nipmem_new nunit_testclass_t<ipcountry_testclass_t>("ipcountry", nipmem_new ipcountry_testclass_t());
	// add some test functions
	ipcountry_testclass->append("general"	, &ipcountry_testclass_t::general);
	// add the testclass to the nunit_suite
	inet_suite->append(ipcountry_testclass);


/********************* nslan_t	***********************************************/
	// init the testclass for the nslan_t
	nunit_testclass_t<nslan_testclass_t> *	nslan_testclass;
	nslan_testclass	= nipmem_new nunit_testclass_t<nslan_testclass_t>("nslan", nipmem_new nslan_testclass_t());
	// add some test functions
	nslan_testclass->append("basic_query"	, &nslan_testclass_t::basic_query);
	// add the testclass to the nunit_suite
	inet_suite->append(nslan_testclass);

/********************* nslan_rec_src_t	***************************************/
	// init the testclass for the nslan_rec_src_t
	nunit_testclass_t<nslan_rec_src_testclass_t> *	nslan_rec_src_testclass;
	nslan_rec_src_testclass	= nipmem_new nunit_testclass_t<nslan_rec_src_testclass_t>("nslan_rec_src", nipmem_new nslan_rec_src_testclass_t());
	// add some test functions
	nslan_rec_src_testclass->append("general"	, &nslan_rec_src_testclass_t::general);
	// add the testclass to the nunit_suite
	inet_suite->append(nslan_rec_src_testclass);

#if 0	// TODO to fix - disabled as it is never ending
/********************* nudp_t	***********************************************/
	// init the testclass for the nudp_t
	nunit_testclass_t<nudp_testclass_t> *	nudp_testclass;
	nudp_testclass	= nipmem_new nunit_testclass_t<nudp_testclass_t>("nudp", nipmem_new nudp_testclass_t());
	// add some test functions
	nudp_testclass->append("general"	, &nudp_testclass_t::general);
	// add the testclass to the nunit_suite
	inet_suite->append(nudp_testclass);
#endif

	// add the inet_suite to the nunit_suite
	nunit_suite.append(inet_suite);	
}

// definition of the nunit_gsuite_t for inet
NUNIT_GSUITE_DEFINITION(inet_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(inet_nunit_gsuite, 0, inet_gsuite_fct);

NEOIP_NAMESPACE_END




