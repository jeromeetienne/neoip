/*! \file
    \brief Definition of the nunit_suite_t

- TODO a nunit for the linger property
  - i dunno where to note it so i note it here
  - a client initiates a socket and write "hello\n" in it
  - a server respond to this socket and immediatly echo everything it 
    read until a '\n' is read.
    - when the '\n' is read and echoed, close the socket immediatly
  - if the client receives the "hello\n" back, the linger works OK, else it doesnt

*/

/* system include */
/* local include */
#include "neoip_socket_nunit_gsuite.hpp"
#include "neoip_string.hpp"
#include "neoip_nunit.hpp"

#include "neoip_socket_peerid_nunit.hpp"
#include "neoip_socket_portid_nunit.hpp"
#include "neoip_socket_addr_nunit.hpp"
#include "neoip_socket_client_nunit.hpp"
#include "neoip_socket_echo_nunit.hpp"
#include "neoip_socket_chargen_nunit.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief Implement a nunit_gsuite_fct_t for the kad layer
 */
static void socket_gsuite_domain_type_fct(nunit_suite_t &nunit_suite, const socket_domain_t &socket_domain
						, const socket_type_t &socket_type) throw()
{
	// init the nunit_suite
	std::string	suite_name	= socket_domain.to_string() + "_" 
						+ string_t::to_lower(socket_type.to_string());
	nunit_suite_t *	domain_suite	= nipmem_new nunit_suite_t(suite_name);	
	// log to debug
	KLOG_DBG("enter suite_name=" << suite_name);

/********************* socket_client_t	***************************************/
	// init the testclass for the socket_client_t
	nunit_testclass_t<socket_client_testclass_t> *	socket_client_testclass;
	socket_client_testclass	= nipmem_new nunit_testclass_t<socket_client_testclass_t>("client"
					, nipmem_new socket_client_testclass_t(socket_domain, socket_type));
	// add some test functions
	socket_client_testclass->append("cnx_establishement"	, &socket_client_testclass_t::cnx_establishement);
	// add the testclass to the nunit_suite
	domain_suite->append(socket_client_testclass);

#if 1
/********************* socket_echo_t	***************************************/
	// init the testclass for the socket_echo_t
	nunit_testclass_t<socket_echo_testclass_t> *	socket_echo_testclass;
	socket_echo_testclass	= nipmem_new nunit_testclass_t<socket_echo_testclass_t>("echo"
					, nipmem_new socket_echo_testclass_t(socket_domain, socket_type));
	// add some test functions
	socket_echo_testclass->append("general"	, &socket_echo_testclass_t::general);
	// add the testclass to the nunit_suite
	domain_suite->append(socket_echo_testclass);
#endif

	// add the domain_suite to the nunit_suite
	nunit_suite.append(domain_suite);	
}

/** \brief Implement a nunit_gsuite_fct_t for the kad layer
 */
static void socket_gsuite_domain_fct(nunit_suite_t &nunit_suite, const socket_domain_t &socket_domain) throw()
{
	// launch add suite for each supported
	for(size_t i = 0; i < socket_type_t::size(); i++ ){
		socket_type_t	socket_type	= socket_type_t::at(i);
		// if this socket_type_t is not suppported by this socket_domain_t, skip it
		if( !socket_domain.support( socket_type ) )	continue;
		// add a suite for this socket_domain/socket_type couple
		socket_gsuite_domain_type_fct(nunit_suite, socket_domain, socket_type);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit_gsuite_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Implement a nunit_gsuite_fct_t for the kad layer
 */
static void socket_gsuite_fct(nunit_suite_t &nunit_suite)
{
	// init the nunit_suite
	nunit_suite_t *	socket_suite	= nipmem_new nunit_suite_t("neoip_socket");	
	// log to debug
	KLOG_DBG("enter");
	
/********************* socket_peerid_t	***************************************/
	// init the testclass for the socket_peerid_t
	nunit_testclass_t<socket_peerid_testclass_t> *	socket_peerid_testclass;
	socket_peerid_testclass	= nipmem_new nunit_testclass_t<socket_peerid_testclass_t>("peerid"
							, nipmem_new socket_peerid_testclass_t());
	// add some test functions
	socket_peerid_testclass->append("test_ctor"		, &socket_peerid_testclass_t::test_ctor);
	socket_peerid_testclass->append("comparison_operator"	, &socket_peerid_testclass_t::comparison_operator);
	socket_peerid_testclass->append("copy_operator"		, &socket_peerid_testclass_t::copy_operator);
	socket_peerid_testclass->append("serial_with_null"	, &socket_peerid_testclass_t::serial_with_null);
	socket_peerid_testclass->append("serial_with_domain"	, &socket_peerid_testclass_t::serial_with_domain);
	// add the testclass to the nunit_suite
	socket_suite->append(socket_peerid_testclass);	

/********************* socket_portid_t	***************************************/
	// init the testclass for the socket_portid_t
	nunit_testclass_t<socket_portid_testclass_t> *	socket_portid_testclass;
	socket_portid_testclass	= nipmem_new nunit_testclass_t<socket_portid_testclass_t>("portid"
							, nipmem_new socket_portid_testclass_t());
	// add some test functions
	socket_portid_testclass->append("test_ctor"		, &socket_portid_testclass_t::test_ctor);
	socket_portid_testclass->append("comparison_operator"	, &socket_portid_testclass_t::comparison_operator);
	socket_portid_testclass->append("copy_operator"		, &socket_portid_testclass_t::copy_operator);
	socket_portid_testclass->append("serial_with_null"	, &socket_portid_testclass_t::serial_with_null);
	socket_portid_testclass->append("serial_with_domain"	, &socket_portid_testclass_t::serial_with_domain);
	// add the testclass to the nunit_suite
	socket_suite->append(socket_portid_testclass);


/********************* socket_addr_t	***************************************/
	// init the testclass for the socket_addr_t
	nunit_testclass_t<socket_addr_testclass_t> *	socket_addr_testclass;
	socket_addr_testclass	= nipmem_new nunit_testclass_t<socket_addr_testclass_t>("addr"
							, nipmem_new socket_addr_testclass_t());
	// add some test functions
	socket_addr_testclass->append("test_ctor"		, &socket_addr_testclass_t::test_ctor);
	socket_addr_testclass->append("comparison_operator"	, &socket_addr_testclass_t::comparison_operator);
	socket_addr_testclass->append("copy_operator"		, &socket_addr_testclass_t::copy_operator);
	socket_addr_testclass->append("serial_with_null"	, &socket_addr_testclass_t::serial_with_null);
	socket_addr_testclass->append("serial_with_domain"	, &socket_addr_testclass_t::serial_with_domain);
	// add the testclass to the nunit_suite
	socket_suite->append(socket_addr_testclass);	

	// init the nunit_suite for each socket_domain_t
	socket_gsuite_domain_fct(*socket_suite, socket_domain_t::UDP);
	socket_gsuite_domain_fct(*socket_suite, socket_domain_t::NTUDP);
	socket_gsuite_domain_fct(*socket_suite, socket_domain_t::NTLAY);
	socket_gsuite_domain_fct(*socket_suite, socket_domain_t::TCP);
	socket_gsuite_domain_fct(*socket_suite, socket_domain_t::STCP);
	
#if 1
// NOTE: special test to do connect chargen on jmehost2 - used to benchmark the tcp socket
/********************* socket_chargen_t	***************************************/
	// init the testclass for the socket_chargen_t
	nunit_testclass_t<socket_chargen_testclass_t> *	socket_chargen_testclass;
	socket_chargen_testclass	= nipmem_new nunit_testclass_t<socket_chargen_testclass_t>("chargen"
							, nipmem_new socket_chargen_testclass_t());
	// add some test functions
	socket_chargen_testclass->append("general"	, &socket_chargen_testclass_t::general);
	// add the testclass to the nunit_suite
	socket_suite->append(socket_chargen_testclass);
#endif

	// add the socket_suite to the nunit_suite
	nunit_suite.append(socket_suite);	
}

// definition of the nunit_gsuite_t for kad
NUNIT_GSUITE_DEFINITION(socket_nunit_gsuite);

// example of inserting a gsuite function in a nunit_gsuite_t 
NUNIT_GSUITE_INSERTER(socket_nunit_gsuite, 0, socket_gsuite_fct);

NEOIP_NAMESPACE_END




