/*! \file
    \brief Declaration of the tcp_resp_t
    
*/

/* system include */
/* local include */
#include "neoip_tcp_utest_resp.hpp"
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
utest_tcp_resp_t::utest_tcp_resp_t()		throw()
{
	inet_err_t	inet_err;
	// start the tcp_resp_t
	inet_err	= tcp_resp.start("127.0.0.1:4000", this, NULL);
	if( inet_err.failed() )	KLOG_ERR("Cant start tcp_resp due to " << inet_err );
	DBG_ASSERT( inet_err.succeed() );
}

/** \brief Desstructor
 */
utest_tcp_resp_t::~utest_tcp_resp_t()		throw()
{
	// delete all the pending connection
	while( !cnx_list.empty() )	nipmem_delete	cnx_list.front();	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     reponder callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref tcp_resp_t when a connection is established
 */
bool	utest_tcp_resp_t::neoip_tcp_resp_event_cb(void *userptr, tcp_resp_t &cb_tcp_resp
							, const tcp_event_t &tcp_event)	throw()
{
	KLOG_ERR("enter event=" << tcp_event);
	if( tcp_event.is_cnx_established() ){
		tcp_full_t *	tcp_full = tcp_event.get_cnx_established();
		// create the connection
		nipmem_new	utest_tcp_resp_cnx_t(this, tcp_full);
	}
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    utest_tcp_resp_cnx_t
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
utest_tcp_resp_cnx_t::utest_tcp_resp_cnx_t(utest_tcp_resp_t *utest_tcp_resp, tcp_full_t *tcp_full)throw()
{
	inet_err_t	inet_err;
	// copy parameter
	this->utest_tcp_resp	= utest_tcp_resp;
	this->tcp_full		= tcp_full;
	// start the tcp_full
	inet_err	= tcp_full->start(this, NULL);
	DBG_ASSERT( inet_err.succeed() );
	KLOG_ERR("create a tcp full socket local=" << tcp_full->get_local_addr()
						<< " remote=" << tcp_full->get_remote_addr() );	
	// link the connection to the list
	utest_tcp_resp->cnx_link( this );
}

/** \brief Desstructor
 */
utest_tcp_resp_cnx_t::~utest_tcp_resp_cnx_t()			throw()
{
	// close the full connection
	nipmem_zdelete	tcp_full;
	// unlink the connection to the list
	utest_tcp_resp->cnx_unlink( this );
}


/** \brief callback notified by \ref tcp_full_t when to notify an event
 */
bool	utest_tcp_resp_cnx_t::neoip_tcp_full_event_cb(void *userptr, tcp_full_t &cb_tcp_full
							, const tcp_event_t &tcp_event)	throw()
{
	KLOG_ERR("enter event=" << tcp_event);
	if( tcp_event.is_fatal() ){
		nipmem_delete this;
	}
	return true;
}

NEOIP_NAMESPACE_END


