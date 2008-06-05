/*! \file
    \brief Definition of the unit test for the \ref socket_client_t

*/

/* system include */
/* local include */
#include "neoip_socket_chargen_nunit.hpp"
#include "neoip_socket_nunit_domain.hpp"
#include "neoip_socket_chargen_client.hpp"
#include "neoip_socket_chargen_server.hpp"
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
socket_chargen_testclass_t::socket_chargen_testclass_t()	throw()
{
	// zero some field
	chargen_client	= NULL;
	chargen_server	= NULL;
}

/** \brief Destructor
 */
socket_chargen_testclass_t::~socket_chargen_testclass_t()	throw()
{
	// deinit the testclass just in case
	neoip_nunit_testclass_deinit();
}
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit init/deinit
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Init the testclass implementation
 */
nunit_err_t	socket_chargen_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	socket_chargen_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");

	nipmem_zdelete	chargen_client;
	nipmem_zdelete	chargen_server;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	socket_chargen_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	socket_err_t	socket_err;
	// log to debug
	KLOG_DBG("enter");
	
	// init and start the socket_chargen_server_t
	socket_resp_arg_t	resp_arg;
	resp_arg	= socket_resp_arg_t().profile(socket_profile_t(socket_domain_t::TCP))
					.domain(socket_domain_t::TCP)
					.type(socket_type_t::STREAM)
					.listen_addr("tcp://127.0.0.1:4019");
	chargen_server	= nipmem_new socket_chargen_server_t();
	socket_err	= chargen_server->start(resp_arg);
	NUNIT_ASSERT( socket_err.succeed() );
	
	// init the socket_itor_arg_t for the client
	socket_itor_arg_t	itor_arg;
	itor_arg	= socket_itor_arg_t().profile(socket_profile_t(socket_domain_t::TCP))
					.domain(socket_domain_t::TCP)
					.type(socket_type_t::STREAM)
					.remote_addr("tcp://127.0.0.1:4019");
//					.remote_addr("tcp://192.168.1.3:19");
	// start the socket_chargen_client_t and make it stop after receiving 300kbyte
	chargen_client	= nipmem_new socket_chargen_client_t();
	socket_err	= chargen_client->start(itor_arg, 300*1024, this, NULL);
	NUNIT_ASSERT( socket_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_chargen_client_t to provide event
 */
bool	socket_chargen_testclass_t::neoip_socket_chargen_client_cb(void *userptr
							, socket_chargen_client_t &cb_chargen_client
							, const socket_err_t &socket_err)	throw()
{
	// log to debug
	KLOG_DBG("enter socker_err=" << socket_err);
	// delete the client
	nipmem_zdelete	chargen_client;
	
	// notify the nunit layer of the success/faillure
	if( socket_err.succeed() )	nunit_ftor(NUNIT_RES_OK);
	else				nunit_ftor(NUNIT_RES_ERROR);
	
	// return dontkeep
	return false;
}



NEOIP_NAMESPACE_END

