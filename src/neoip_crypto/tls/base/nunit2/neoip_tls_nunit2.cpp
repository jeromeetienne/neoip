/*! \file
    \brief Definition of the unit test for the \ref socket_client_t

\par Brief Description
- this nunit is a testbed to establish connection using tls_*_t layer
- this may perform raw http client connection to test against https server
  - NOTE: this require tls_itor_t to use certificate
- this may perform connection against gnutls server example

*/

/* system include */
/* local include */
#include "neoip_tls_nunit2.hpp"
#include "neoip_socket_client.hpp"
#include "neoip_socket.hpp"
#include "neoip_tls.hpp"
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
tls_testclass2_t::tls_testclass2_t()	throw()
{
	// zero some field
	socket_client	= NULL;
	tls_itor	= NULL;
	tls_full	= NULL;	
}

/** \brief Destructor
 */
tls_testclass2_t::~tls_testclass2_t()	throw()
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
nunit_err_t	tls_testclass2_t::neoip_nunit_testclass_init()	throw()
{
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	tls_testclass2_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// destruct the tls_itor_t if needed
	nipmem_zdelete tls_itor;
	// destruct the tls_full_t if needed
	nipmem_zdelete tls_full;
	// destruct the socket_client_t if needed
	nipmem_zdelete socket_client;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	tls_testclass2_t::cnx_establishement(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	tls_profile_t	tls_profile;
	// log to debug
	KLOG_DBG("enter");

	socket_addr_t	remote_addr;
#if 1
	// test a https connection - require tls with certificate
	remote_addr	= "tcp://192.168.1.3:443";
	tls_profile.authtype(tls_authtype_t::CERT);
#else
	// test a anonymous connection with a local server - ex-serv-anon in gnutls examples
	remote_addr	= "tcp://127.0.0.1:5556";
	tls_profile.authtype(tls_authtype_t::ANON);
#endif

	
	// init and start socket_client_t
	socket_err_t		socket_err;
	socket_itor_arg_t	itor_arg;
	itor_arg	= socket_itor_arg_t().domain(socket_domain_t::TCP)
					.type(socket_type_t::STREAM)
					.profile(socket_profile_t("tcp"))
					.remote_addr(remote_addr);	
	socket_client	= nipmem_new socket_client_t();
	socket_err	= socket_client->start(itor_arg, this, NULL);
	NUNIT_ASSERT( socket_err.succeed() );
	
	// start a tls_itor_t
	crypto_err_t	crypto_err;
	tls_itor	= nipmem_new tls_itor_t();
	crypto_err	= tls_itor->profile(tls_profile).start();
	NUNIT_ASSERT( crypto_err.succeed() );
	
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

/** \brief callback notified by \ref socket_client_t to provide event
 */
bool	tls_testclass2_t::neoip_socket_client_event_cb(void *userptr, socket_client_t &cb_socket_client
							, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_ERR("enter event=" << socket_event);
	// sanity check - the event MUST be client_ok
	DBG_ASSERT( socket_event.is_client_ok() );
	
	// handle fatal socket_event_t
	if( socket_event.is_fatal()  ){
		neoip_nunit_testclass_deinit();
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}
	
	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::CNX_ESTABLISHED:	return handle_cnx_established();
	case socket_event_t::RECVED_DATA:	return handle_recved_data(*socket_event.get_recved_data());
	case socket_event_t::MAYSEND_ON:	return handle_maysend_on();
	default:	DBG_ASSERT( 0 );
	}	

	// return dontkeep
	return false;
}


/** \brief Handle the socket_event_t::CNX_ESTABLISHED event
 * 
 * @return a tokeep for the socket_client_t
 */
bool	tls_testclass2_t::handle_cnx_established()	throw()
{
	socket_err_t	socket_err;

	// configure the socket_client_t
	socket_client->rcvdata_maxlen( 512*1024 );
	socket_client->xmitbuf_maxlen( 50*1024 );
	socket_client->maysend_tshold( socket_full_t::UNLIMITED );

	// notify a empty pkt_t to tls_itor_t to get it started
	crypto_err_t	crypto_err;
	crypto_err	= tls_itor->notify_recved_data(pkt_t(), &tls_full);
	DBG_ASSERT( tls_full == NULL );
	DBG_ASSERT( crypto_err.succeed() );
	// xmit_ifneeded
	xmit_ifneeded();
	// return tokeep
	return true;
}

/** \brief Handle the socket_event_t::RECVED_DATA event
 * 
 * @return a tokeep for the socket_client_t
 */
bool	tls_testclass2_t::handle_recved_data(pkt_t &pkt)	throw()
{
	crypto_err_t	crypto_err;
	// log to debug
	KLOG_ERR("enter pkt.size()=" << pkt.size());

	// notify the data to tls_itor_t - 
	if( tls_itor ){
		crypto_err	= tls_itor->notify_recved_data(pkt, &tls_full);
		DBG_ASSERT( crypto_err.succeed() );
		// if tls_full is returned non-null, then connection is considered established
		if( tls_full ){
			// xmit_ifneeded
			xmit_ifneeded();
			// delete tls_itor_t as it is now useless
			nipmem_zdelete	tls_itor;
			KLOG_ERR("connection established");
#if 1
			// send a raw http request
			datum_t		toxmit_data("GET / HTTP/1.0\r\n\r\n");
			crypto_err	= tls_full->notify_toxmit_data( toxmit_data );
			DBG_ASSERT( crypto_err.succeed() );
#endif
		}
		// xmit_ifneeded
		xmit_ifneeded();
	}else{
		// sanity check - at this point, tls_full_t MUST be non NULL
		DBG_ASSERT( tls_full );
		// notify the recved data to tls_full_t
		crypto_err	= tls_full->notify_recved_data(pkt);
		DBG_ASSERT( crypto_err.succeed() );
		
		// display and empty the tls_full->recv_buffer()
		KLOG_ERR("tls_full recv_buffer=" << tls_full->recv_buffer());
		tls_full->recv_buffer().head_free( tls_full->recv_buffer().length() );
				
		// xmit_ifneeded
		xmit_ifneeded();
		
#if 0	// TODO to remove- crap stuff to test gracefull_shutdown
		tls_full->notify_gracefull_shutdown();
		xmit_ifneeded();
		nipmem_zdelete socket_client;
#endif	
	}
	// return tokeep
	return true;
}

/** \brief Handle the socket_event_t::MAYSEND_ON event
 */
bool	tls_testclass2_t::handle_maysend_on()		throw()
{
	// xmit_ifneeded
	xmit_ifneeded();
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief xmit more data if needed
 */
void	tls_testclass2_t::xmit_ifneeded()					throw()
{
	socket_err_t	socket_err;
	// log to debug
	KLOG_DBG("enter");
	
	// determine the xmit_buffer 
	pkt_t *		xmit_buffer	= NULL;
	if( tls_itor )	xmit_buffer	= &tls_itor->xmit_buffer();
	else		xmit_buffer	= &tls_full->xmit_buffer();
	
	// sanity check - this function is used IIF the http_reqhd.method() is POST
	DBG_ASSERT( socket_client->xmitbuf_freelen() != socket_full_t::UNLIMITED );
	// compute the length of data to xmit 
	size_t xmit_len	= std::min(socket_client->xmitbuf_freelen(), xmit_buffer->length());

	// write the data thru the socket_client_t
	socket_err	= socket_client->send( xmit_buffer->range(0, xmit_len, datum_t::NOCOPY) );
	DBG_ASSERT( socket_err.succeed() );	// NOTE: this is only temporary this may return error in the future

	// free xmit_len byte from_itor->xmit_buffer();
	xmit_buffer->head_free( xmit_len );

	// if xmit_buffer not empty,set socket_client->maysend_tshold to half xmitbuf_usedlen
	if( !xmit_buffer->empty() ){
		socket_client->maysend_tshold( socket_client->xmitbuf_usedlen() / 2 );
	}
}

NEOIP_NAMESPACE_END

