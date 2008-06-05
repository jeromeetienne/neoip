/*! \file
    \brief Declaration of the udp_itor_t
    
*/

/* system include */
/* local include */
#include "nc2_client.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
nc2_client_t::nc2_client_t(const std::string &addr_str)		throw()
{
	// zero some fields
	udp_full	= NULL;
	nlay_itor	= NULL;
	nlay_full	= NULL;
	
	inet_err_t	inet_err;
	// set the listen address
	inet_err = udp_itor.set_remote_addr(addr_str.c_str());
	DBG_ASSERT( inet_err.succeed() );
	// set the callback
	inet_err = udp_itor.set_callback(this, NULL);
	DBG_ASSERT( inet_err.succeed() );
	// start the udp_itor_t
	inet_err = udp_itor.start();
	DBG_ASSERT( inet_err.succeed() );
}

/** \brief Desstructor
 */
nc2_client_t::~nc2_client_t()		throw()
{
	// stop the ping_timeout
	ping_timeout.stop();
	
	if( nlay_full )	nipmem_delete nlay_full;
	if( nlay_itor )	nipmem_delete nlay_itor;
	if( udp_full )	nipmem_delete udp_full;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     initiator callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_itor_t when a connection is established
 */
bool	nc2_client_t::neoip_inet_udp_itor_event_cb(void *userptr, udp_itor_t &cb_udp_itor
							, const udp_event_t &udp_event)	throw()
{
	inet_err_t	inet_err;
	nlay_err_t	nlay_err;
	KLOG_ERR("enter event=" << udp_event);
	// sanity check - the event MUST be itor_ok
	DBG_ASSERT( udp_event.is_itor_ok() );

	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_ESTABLISHED:
			udp_full = udp_event.get_cnx_established();
			// set the callback
			inet_err = udp_full->set_callback(this, NULL);
			DBG_ASSERT( inet_err.succeed() );
			// start the udp_full
			inet_err = udp_full->start();
			if( inet_err.failed() ){
				KLOG_ERR("udp_full->start() produced error = " << inet_err );
				DBG_ASSERT( 0 );
				return true;
			}
			KLOG_ERR("create a udp full socket local=" << udp_full->get_local_addr()
							<< " remote=" << udp_full->get_remote_addr() );
				
			// start the nlay_itor
			nlay_itor = nipmem_new nlay_itor_t(&nlay_profile, nlay_type_t::DGRAM
								, nlay_type_t::DGRAM, this, NULL);
			nlay_err  = nlay_itor->start();
			break;
	case udp_event_t::CNX_REFUSED:	
			KLOG_ERR("connection refused");
			DBG_ASSERT( 0 );
			break;
	default:	DBG_ASSERT( 0 );
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     udp_full callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref udp_full_t when to notify an event
 */
bool	nc2_client_t::neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
							, const udp_event_t &udp_event)	throw()
{
	nlay_err_t	nlay_err;	
	KLOG_ERR("enter event=" << udp_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( udp_event.is_full_ok() );
	
	// handle each possible events from its type
	switch( udp_event.get_value() ){
	case udp_event_t::CNX_CLOSED:
			KLOG_ERR("Closing the udp_full_t");
			DBG_ASSERT( 0 );	// TODO
			return false;
	case udp_event_t::RECVED_DATA:{
			pkt_t *	pkt	= udp_event.get_recved_data();
			DBG_ASSERT( nlay_itor || nlay_full );
#if 0
			if( nlay_itor )	nlay_err = nlay_itor->pkt_from_lower( *pkt );
			else		nlay_err = nlay_full->pkt_from_lower( *pkt );
#else
			if(pkt);
			DBG_ASSERT(0);
#endif
			if( !nlay_err.succeed() )
				KLOG_ERR("processing incoming udp packet thru nlay produced "<< nlay_err);
			break;}
	default:	DBG_ASSERT( 0 );
	}
	return true;
}

/** \brief Callback notified with \ref nlay_itor_t report an event
 */
bool nc2_client_t::neoip_nlay_itor_event_cb(void *cb_userptr, nlay_itor_t &cb_nlay_itor
					, const nlay_event_t &nlay_event)	throw()
{
	nlay_err_t	nlay_err;
	KLOG_ERR("enter event=" << nlay_event);
	// sanity check - the event MUST be itor_ok
	DBG_ASSERT( nlay_event.is_async_itor_ok() );
	// sanity check - the nlay_itor MUST NOT be null
	DBG_ASSERT( nlay_itor );
	// sanity check - the udp_full MUST NOT be null
	DBG_ASSERT( udp_full  );
	
	// handle each possible events from its type
	switch( nlay_event.get_value() ){
	case nlay_event_t::CNX_REFUSED:
			KLOG_ERR("Closing the nlay_full_t");
			DBG_ASSERT( 0 );	// TODO
			return false;
	case nlay_event_t::PKT_TO_LOWER:{
			pkt_t *	pkt	= nlay_event.get_pkt_to_lower();
			udp_full->send(pkt->get_data(), pkt->get_len());
			break;}
	case nlay_event_t::CNX_ESTABLISHED:
			nlay_full = nlay_event.get_cnx_established();
			// setup the nlay_full
			nlay_err = nlay_full->set_callback(this, NULL);
			DBG_ASSERT( nlay_err.succeed() );
			nlay_err = nlay_full->start();
			DBG_ASSERT( nlay_err.succeed() );
			// start the timeout
			ping_timeout.start(delay_t::from_sec(1), this, NULL);
			// stop the nlay_itor
			nipmem_delete nlay_itor;
			nlay_itor = NULL;
			return false;
	default:	DBG_ASSERT( 0 );
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     timeout callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief callback called when the neoip_timeout expire
 */
bool nc2_client_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	KLOG_ERR("Send dummy packet to fill the connection");
	
	char message[3000];
	time_t t = time(NULL); 
	sprintf(message, "NeoIP time = %-24.24s\n", ctime(&t));
	sprintf(message+strlen(message), "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
		"asdlfkjadslfkjllwoeupoiu.,vmnzclxvjhldfjkghr0g7q3tlkjadf");

	for( size_t i = strlen(message)+1; i < sizeof(message); i++ )
		message[i] = 'a' + i % 16;
	
	pkt_t	pkt( message, strlen(message) );
	KLOG_ERR("pkt before=" << pkt );
	nlay_err_t	nlay_err;
	nlay_err	= nlay_full->pkt_from_upper( pkt );
	KLOG_ERR("pkt after=" << pkt );
	if( nlay_err.succeed() )	udp_full->send(pkt.get_data(), pkt.get_len());

	return true;
}

/** \brief Callback notified with \ref nlay_full_t report an event
 */
bool	nc2_client_t::neoip_nlay_full_event_cb(void *cb_userptr, nlay_full_t &cb_nlay_full
						, const nlay_event_t &nlay_event)	throw()
{
	KLOG_ERR("enter event=" << nlay_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( nlay_event.is_full_ok() );
	// sanity check - the nlay_itor MUST NOT be null
	DBG_ASSERT( nlay_full != NULL );
	// sanity check - the udp_full MUST NOT be null
	DBG_ASSERT( udp_full != NULL );
	
	// handle each possible events from its type
	switch( nlay_event.get_value() ){
	case nlay_event_t::CNX_CLOSING:
	case nlay_event_t::CNX_DESTROYED:
			KLOG_ERR("Closing the nlay_full_t");
			DBG_ASSERT( 0 );	// TODO
			return false;
	case nlay_event_t::PKT_TO_LOWER:{
			pkt_t *	pkt	= nlay_event.get_pkt_to_lower();
			udp_full->send(pkt->get_data(), pkt->get_len());
			break;}
	default:	DBG_ASSERT( 0 );
	}
	return true;	
}

NEOIP_NAMESPACE_END


