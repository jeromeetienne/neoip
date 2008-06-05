/*! \file
    \brief Definition of the unit test for the \ref tcp_client_t

*/

/* system include */
/* local include */
#include "neoip_tcp_chargen_client.hpp"
#include "neoip_tcp_client.hpp"
#include "neoip_tcp.hpp"
#include "neoip_pkt.hpp"
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
tcp_chargen_client_t::tcp_chargen_client_t()	throw()
{
	// zero some field
	tcp_client		= NULL;
	recv_curlen		= 0;
}

/** \brief Destructor
 */
tcp_chargen_client_t::~tcp_chargen_client_t()	throw()
{
	// destruct the tcp client
	nipmem_zdelete tcp_client;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
inet_err_t	tcp_chargen_client_t::start(const tcp_itor_arg_t &itor_arg, size_t recv_maxlen
				, tcp_chargen_client_cb_t *callback, void *userptr)	throw()
{
	inet_err_t	inet_err;
	// copy the parameter
	this->recv_maxlen	= recv_maxlen;
	this->callback		= callback;
	this->userptr		= userptr;

	// init and start tcp_client_t
	tcp_client	= nipmem_new tcp_client_t();
	inet_err	= tcp_client->start(itor_arg, this, NULL);
	if( inet_err.failed() )	return inet_err;

	// return no error
	return inet_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     tcp_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref tcp_client_t to provide event
 */
bool	tcp_chargen_client_t::neoip_tcp_client_event_cb(void *userptr, tcp_client_t &cb_tcp_client
							, const tcp_event_t &tcp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << tcp_event);
	// sanity check - the event MUST be client_ok
	DBG_ASSERT( tcp_event.is_client_ok() );

	// handle the fatal events
	if( tcp_event.is_fatal() )	return notify_callback(inet_err_t::ERROR);

	// handle each possible events from its type
	switch( tcp_event.get_value() ){
	case tcp_event_t::CNX_ESTABLISHED:
			// log to debug
			KLOG_DBG("Connection established");
			// return tokeep
			return true;
	case tcp_event_t::RECVED_DATA:
			// handle the received packet
			return handle_recved_data(*tcp_event.get_recved_data());
	default:	DBG_ASSERT(0);
	}
	// NOTE: this point MUST NOT be reached
	DBG_ASSERT( 0 );

	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   packet reception
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Handle received data on the cnx_t
 * 
 * @return a 'tokeep/dontkeep' for the tcp_client_t
 */
bool	tcp_chargen_client_t::handle_recved_data(pkt_t &pkt)	throw()
{
	// log to debug
	KLOG_DBG("enter pkt size=" << pkt.size());
	// update the recv_curlen
	recv_curlen	+= pkt.size();
	// check if it is completed
	if( recv_maxlen && recv_curlen >= recv_maxlen )	return notify_callback(inet_err_t::OK);

#if 0
	// to display the output
	KLOG_STDOUT(std::string((char *)pkt.get_ptr(), pkt.get_len()));
#endif
	// return 'tokeep'
	return true;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        function to notify caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool tcp_chargen_client_t::notify_callback(const inet_err_t &inet_err)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_tcp_chargen_client_cb(userptr, *this, inet_err);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END

