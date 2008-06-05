/*! \file
    \brief Definition of the unit test for the \ref tcp_client_t

*/

/* system include */
/* local include */
#include "neoip_tcp_client_nunit.hpp"
#include "neoip_tcp_client.hpp"
#include "neoip_tcp.hpp"
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
tcp_client_testclass_t::tcp_client_testclass_t()	throw()
{
	// zero some field
	tcp_resp		= NULL;
	tcp_client		= NULL;
}

/** \brief Destructor
 */
tcp_client_testclass_t::~tcp_client_testclass_t()	throw()
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
nunit_err_t	tcp_client_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	tcp_client_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// destruct the tcp client
	nipmem_zdelete tcp_client;
	// destruct the tcp responder
	nipmem_zdelete tcp_resp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	tcp_client_testclass_t::cnx_establishement(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	inet_err_t	inet_err;
	// log to debug
	KLOG_DBG("enter");

	// init and start tcp_resp_t
	tcp_resp	= nipmem_new tcp_resp_t();
	inet_err	= tcp_resp->start("127.0.0.1:4000", this, NULL);
	if( inet_err.failed() )	KLOG_ERR("inet_err=" << inet_err);
	NUNIT_ASSERT( inet_err.succeed() );

	// init and start tcp_client_t
	tcp_client	= nipmem_new tcp_client_t();
	inet_err	= tcp_client->start("127.0.0.1:4000", this, NULL);
	if( inet_err.failed() )	KLOG_ERR("inet_err=" << inet_err);
	NUNIT_ASSERT( inet_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     tcp_resp_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref tcp_resp_t when a connection is established
 */
bool	tcp_client_testclass_t::neoip_tcp_resp_event_cb(void *userptr, tcp_resp_t &cb_tcp_resp
							, const tcp_event_t &tcp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << tcp_event);
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( tcp_event.is_resp_ok() );
	
	// handle each possible events from its type
	switch( tcp_event.get_value() ){
	case tcp_event_t::CNX_ESTABLISHED:{
			// close the tcp_full_t immediatly
			tcp_full_t *	tcp_full = tcp_event.get_cnx_established();
			nipmem_delete	tcp_full;
			break;}
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     tcp_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref tcp_client_t to provide event
 */
bool	tcp_client_testclass_t::neoip_tcp_client_event_cb(void *userptr, tcp_client_t &cb_tcp_client
							, const tcp_event_t &tcp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << tcp_event);
	// sanity check - the event MUST be client_ok
	DBG_ASSERT( tcp_event.is_client_ok() );

	// delete the tcp_client
	nipmem_zdelete	tcp_client;
	
	// report the result depending on the tcp_event
	if( tcp_event.is_cnx_established() )	nunit_ftor(NUNIT_RES_OK);
	else					nunit_ftor(NUNIT_RES_ERROR);

	// return dontkeep
	return false;
}



NEOIP_NAMESPACE_END

