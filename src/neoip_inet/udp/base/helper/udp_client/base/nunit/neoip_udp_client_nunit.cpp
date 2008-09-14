/*! \file
    \brief Definition of the unit test for the \ref udp_client_t

*/

/* system include */
/* local include */
#include "neoip_udp_client_nunit.hpp"
#include "neoip_udp_client.hpp"
#include "neoip_udp.hpp"
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
udp_client_testclass_t::udp_client_testclass_t()	throw()
{
	// zero some field
	udp_resp	= NULL;	
}

/** \brief Destructor
 */
udp_client_testclass_t::~udp_client_testclass_t()	throw()
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
nunit_err_t	udp_client_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	udp_client_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete all the udp_client_t in udp_client_db
	while( !udp_client_db.empty() ){
		udp_client_t *	udp_client	= udp_client_db.front();
		udp_client_db.remove(udp_client);
		nipmem_zdelete	udp_client;
	}
	// delete all the udp_full_t in udp_full_db
	while( !udp_full_db.empty() ){
		udp_full_t *	udp_full	= udp_full_db.front();
		udp_full_db.remove(udp_full);
		nipmem_zdelete	udp_full;
	}
	// close the udp_resp if needed
	nipmem_zdelete	udp_resp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	udp_client_testclass_t::echo_localhost_inetd(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	inet_err_t	inet_err;
	ipport_addr_t	listen_addr	= "127.0.0.1:4000";
	// log to debug
	KLOG_DBG("enter");

	// create the udp_resp_t	
	udp_resp	= nipmem_new udp_resp_t();
	inet_err 	= udp_resp->set_listen_addr(listen_addr);
	if( inet_err.succeed() )	inet_err = udp_resp->set_callback(this, NULL);
	if( inet_err.succeed() )	inet_err = udp_resp->start();
	NUNIT_ASSERT( inet_err.succeed() );
		
	// init and start all udp_client_t
	for(size_t i = 0; i < 10; i++){
		udp_client_t *	udp_client;
		udp_client	= nipmem_new udp_client_t();
		inet_err	= udp_client->start(listen_addr, this, NULL);
		udp_client_db.push_back(udp_client);
		NUNIT_ASSERT( inet_err.succeed() );
	}
	
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			udp_resp_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_resp_t when a connection is established
 */
bool	udp_client_testclass_t::neoip_inet_udp_resp_event_cb(void *userptr
					, udp_resp_t &cb_udp_resp
					, const udp_event_t &udp_event)		throw()
{
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( udp_event.is_resp_ok() );	
	// log to debug
	KLOG_ERR("enter event=" << udp_event);
	
	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_ESTABLISHED:{
			inet_err_t	inet_err;
			// if an established connection is notified by the udp_resp_t, create a cnx_t to handle it
			udp_full_t *	udp_full = udp_event.get_cnx_established();
			// set the callback for the udp_full
			inet_err = udp_full->set_callback(this, NULL);
			DBG_ASSERT( inet_err.succeed() );
			// start the udp_full
			inet_err = udp_full->start();
			DBG_ASSERT( inet_err.succeed() );
			// put it in the udp_full_db
			udp_full_db.push_back(udp_full);
			break;}
	default:	DBG_ASSERT( 0 );	
	}
	// return a 'tokeep'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			udp_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_full_t when to notify an event
 */
bool	udp_client_testclass_t::neoip_inet_udp_full_event_cb(void *userptr
						, udp_full_t &cb_udp_full
						, const udp_event_t &udp_event)	throw()
{
	udp_full_t *	udp_full	= &cb_udp_full;
	// log to debug
	KLOG_DBG("enter event=" << udp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( udp_event.is_full_ok() );

	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::RECVED_DATA:{
			// get the received packet
			pkt_t &	pkt		= *udp_event.get_recved_data();
			// just echo it back
			size_t	written_len	= udp_full->send(pkt);
			DBG_ASSERT( written_len == pkt.length() );
			// close the udp_full_t and remove it from the 
			udp_full_db.remove(udp_full);
			nipmem_zdelete	udp_full;		
			// return dontkeep - as in all cases, the udp_full_t has been deleted
			return false;}
	default:	DBG_ASSERT( 0 );
	}
	// return a 'tokeep'
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     udp_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_client_t to provide event
 */
bool	udp_client_testclass_t::neoip_udp_client_event_cb(void *userptr, udp_client_t &cb_udp_client
							, const udp_event_t &udp_event)	throw()
{
	udp_client_t *	udp_client	= &cb_udp_client;
	// log to debug
	KLOG_ERR("enter event=" << udp_event);
	// sanity check - the event MUST be client_ok
	DBG_ASSERT( udp_event.is_client_ok() );
	
	// build the message
	pkt_t	pkt;
	pkt << udp_client->local_addr();

	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_ESTABLISHED:{
			// write the packet thru the socket
			size_t	written_len = udp_client->send(pkt.to_datum());
			DBG_ASSERT( written_len == pkt.length() );
			break;}
	case udp_event_t::RECVED_DATA:{
			pkt_t &	recved_pkt	= *udp_event.get_recved_data();
			// check that the received packet is the one previously xmited
			if( recved_pkt != pkt)		return nunit_ftor(NUNIT_RES_ERROR);
			// delete this udp_client
			udp_client_db.remove(udp_client);
			nipmem_zdelete	udp_client;
			// if udp_client_db is empty, return success			
			if( udp_client_db.empty() )	nunit_ftor(NUNIT_RES_OK);
			return false;
			break;}
	default:	DBG_ASSERT(0);
	}

	// return tokeep
	return true;
}

NEOIP_NAMESPACE_END

