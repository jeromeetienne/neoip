/*! \file
    \brief Definition of the unit test for the \ref tcp_client_t

*/

/* system include */
/* local include */
#include "neoip_tcp_chargen_nunit.hpp"
#include "neoip_tcp_chargen_client.hpp"
#include "neoip_tcp_chargen_server.hpp"
#include "neoip_tcp.hpp"
#include "neoip_rate_sched.hpp"
#include "neoip_ipport_addr.hpp"
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
tcp_chargen_testclass_t::tcp_chargen_testclass_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	chargen_client	= NULL;
	chargen_server	= NULL;
	xmit_rsched	= NULL;
	recv_rsched	= NULL;
}

/** \brief Destructor
 */
tcp_chargen_testclass_t::~tcp_chargen_testclass_t()	throw()
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
nunit_err_t	tcp_chargen_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	tcp_chargen_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");

	nipmem_zdelete	chargen_client;
	nipmem_zdelete	chargen_server;
	nipmem_zdelete	xmit_rsched;
	nipmem_zdelete	recv_rsched;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	tcp_chargen_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	inet_err_t	inet_err;
	rate_err_t	rate_err;
	// log to debug
	KLOG_DBG("enter");

	// init the recv_rsched
	recv_rsched	= nipmem_new rate_sched_t();
	rate_err	= recv_rsched->start(300*1000);
	NUNIT_ASSERT( rate_err.succeed() );

	// init the xmit_rsched
	xmit_rsched	= nipmem_new rate_sched_t();
	rate_err	= xmit_rsched->start(300*1000);
	NUNIT_ASSERT( rate_err.succeed() );

	// init and start the tcp_chargen_server_t
	tcp_profile_t		resp_profile;
#if 0	// put a xmit_limit on all tcp_full_t from the tcp_chargen_server_t
	resp_profile.full().xmit_limit_arg().rate_sched(xmit_rsched)
						.rate_prec(rate_prec_t(50))
						.absrate_max(15*1000)
						;
#endif

#if 1	// TODO to reenable - disabled to test without server
	tcp_resp_arg_t	resp_arg = tcp_resp_arg_t().listen_addr("127.0.0.1:4019").profile(resp_profile);
	chargen_server	= nipmem_new tcp_chargen_server_t();
	inet_err	= chargen_server->start(resp_arg);
	NUNIT_ASSERT( inet_err.succeed() );
#endif

	// setup the tcp_itor_arg_t
	tcp_profile_t	itor_profile;
#if 0	// put a recv_limit on all tcp_full_t from the tcp_chargen_server_t
	itor_profile.full().recv_limit_arg().rate_sched(recv_rsched)
						.rate_prec(rate_prec_t(50))
						.absrate_max(15*1000)
						;
#endif
	// start the tcp_chargen_client_t and make it stop after receiving 300kbyte
#if 1	// NOTE: to use the internal chargen server
	tcp_itor_arg_t	itor_arg = tcp_itor_arg_t().remote_addr("127.0.0.1:4019")
							.profile(itor_profile);
#else	// NOTE: to use the inetd chargen server
	tcp_itor_arg_t	itor_arg = tcp_itor_arg_t().remote_addr("127.0.0.1:19")
							.profile(itor_profile);
#endif
	chargen_client	= nipmem_new tcp_chargen_client_t();
	inet_err	= chargen_client->start(itor_arg, 300*1024, this, NULL);
	NUNIT_ASSERT( inet_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     tcp_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref tcp_chargen_client_t to provide event
 */
bool	tcp_chargen_testclass_t::neoip_tcp_chargen_client_cb(void *userptr
							, tcp_chargen_client_t &cb_chargen_client
							, const inet_err_t &inet_err)	throw()
{
	// log to debug
	KLOG_DBG("enter inet_err=" << inet_err);
	// delete the client
	nipmem_zdelete	chargen_client;

	// notify the nunit layer of the success/faillure
	if( inet_err.succeed() )	nunit_ftor(NUNIT_RES_OK);
	else				nunit_ftor(NUNIT_RES_ERROR);
	
	// return dontkeep
	return false;
}



NEOIP_NAMESPACE_END

