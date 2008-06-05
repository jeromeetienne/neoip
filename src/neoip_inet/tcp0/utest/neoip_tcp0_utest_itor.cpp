/*! \file
    \brief Declaration of the tcp_itor_t
    
*/

/* system include */
/* local include */
#include "neoip_tcp_utest_itor.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
utest_tcp_itor_t::utest_tcp_itor_t()		throw()
{
	inet_err_t	inet_err;
	// start the tcp_itor_t
	inet_err	= tcp_itor.start("127.0.0.1:4000", this, NULL);
	DBG_ASSERT( inet_err.succeed() );
}

/** \brief Desstructor
 */
utest_tcp_itor_t::~utest_tcp_itor_t()		throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     initiator callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref tcp_itor_t when a connection is established
 */
bool	utest_tcp_itor_t::neoip_tcp_itor_event_cb(void *userptr, tcp_itor_t &cb_tcp_itor
							, const tcp_event_t &tcp_event)	throw()
{
	KLOG_ERR("enter event=" << tcp_event);
	if( tcp_event.is_cnx_established() ){
		inet_err_t	inet_err;
		tcp_full	= tcp_event.get_cnx_established();
		// start the tcp_full
		inet_err	= tcp_full->start(this, NULL);
		if( inet_err.failed() ){
			KLOG_ERR("tcp_full->start() produced error = " << inet_err );
			nipmem_delete	tcp_full;
			return true;
		}
		KLOG_ERR("create a tcp full socket local=" << tcp_full->get_local_addr()
						<< " remote=" << tcp_full->get_remote_addr() );
		// start the timer
		ping_timeout.start(delay_t(1000), this, NULL);
	}
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     tcp_full callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref tcp_full_t when to notify an event
 */
bool	utest_tcp_itor_t::neoip_tcp_full_event_cb(void *userptr, tcp_full_t &cb_tcp_full
							, const tcp_event_t &tcp_event)	throw()
{
	KLOG_ERR("enter event=" << tcp_event);
	if( tcp_event.is_fatal() ){
		KLOG_ERR("Closing the tcp_full_t");
		// stop the ping_timeout
		ping_timeout.stop();
		// delete the tcp_full and mark it unused
		nipmem_delete tcp_full;
		tcp_full = NULL;
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
bool utest_tcp_itor_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	KLOG_ERR("Send dummy packet to fill the connection");
	
	char message[3000];
	time_t t = time(NULL); 
	sprintf(message, "NeoIP time = %-24.24s\n", ctime(&t));

	for( size_t i = strlen(message)+1; i < sizeof(message); i++ )
		message[i] = 'a' + i % 16;
	
	tcp_full->send( message, strlen(message) );

	return true;
}

NEOIP_NAMESPACE_END


