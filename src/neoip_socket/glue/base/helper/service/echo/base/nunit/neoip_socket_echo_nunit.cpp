/*! \file
    \brief Definition of the unit test for the \ref socket_client_t

*/

/* system include */
/* local include */
#include "neoip_socket_echo_nunit.hpp"
#include "neoip_socket_nunit_domain.hpp"
#include "neoip_socket_echo_client.hpp"
#include "neoip_socket_echo_server.hpp"
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
socket_echo_testclass_t::socket_echo_testclass_t(const socket_domain_t &socket_domain
						, const socket_type_t &socket_type)	throw()
{
	// copy the parameter
	this->socket_domain	= socket_domain;
	this->socket_type	= socket_type;
	// zero some field
	nunit_domain		= NULL;
	echo_client		= NULL;
	echo_server		= NULL;
}

/** \brief Destructor
 */
socket_echo_testclass_t::~socket_echo_testclass_t()	throw()
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
nunit_err_t	socket_echo_testclass_t::neoip_nunit_testclass_init()	throw()
{
	socket_err_t	socket_err;
	// allocate the nunit_domain
	nunit_domain	= nipmem_new socket_nunit_domain_t(socket_domain);

#if 1
	// init and start the socket_echo_server_t
	socket_resp_arg_t	resp_arg;
	resp_arg	= nunit_domain->get_resp_arg();	
	echo_server	= nipmem_new socket_echo_server_t();
	socket_err	= echo_server->start(resp_arg.type(socket_type));
	if( socket_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, socket_err.to_string());
#endif
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	socket_echo_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");

	nipmem_zdelete	echo_client;
	nipmem_zdelete	echo_server;
	nipmem_zdelete	nunit_domain;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	socket_echo_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	socket_err_t	socket_err;
	// log to debug
	KLOG_DBG("enter");
	
	// init and start socket_echo_client_t
	socket_itor_arg_t	itor_arg;
	itor_arg	= nunit_domain->get_itor_arg();	
	echo_client	= nipmem_new socket_echo_client_t();
	socket_err	= echo_client->start(itor_arg.type(socket_type), this, NULL);
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

/** \brief callback notified by \ref socket_echo_client_t to provide event
 */
bool	socket_echo_testclass_t::neoip_socket_echo_client_cb(void *userptr
							, socket_echo_client_t &cb_echo_client
							, const socket_err_t &socket_err)	throw()
{
	// log to debug
	KLOG_DBG("enter socker_err=" << socket_err);
	// delete the client
	nipmem_zdelete	echo_client;
	
	// notify the nunit layer of the success/faillure
	if( socket_err.succeed() )	nunit_ftor(NUNIT_RES_OK);
	else				nunit_ftor(NUNIT_RES_ERROR);
	
	// return dontkeep
	return false;
}



NEOIP_NAMESPACE_END

