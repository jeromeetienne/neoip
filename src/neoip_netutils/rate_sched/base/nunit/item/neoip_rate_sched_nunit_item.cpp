/*! \file
    \brief Definition of the \ref rate_sched_nunit_item_t

*/

/* system include */
/* local include */
#include "neoip_rate_sched_nunit_item.hpp"
#include "neoip_rate_sched_nunit.hpp"
#include "neoip_rate_sched.hpp"
#include "neoip_rate_limit.hpp"
#include "neoip_socket_client.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
rate_sched_nunit_item_t::rate_sched_nunit_item_t()	throw()
{
	// zero some fields
	rate_sched_testclass	= NULL;
	socket_client		= NULL;
	rate_limit		= NULL;
}

/** \brief Destructor
 */
rate_sched_nunit_item_t::~rate_sched_nunit_item_t()	throw()
{
	// delete the socket_client_t if needed
	nipmem_zdelete	socket_client;
	// delete the rate_limit_t if needed
	nipmem_zdelete	rate_limit;
	// unlink this object from the rate_sched_testclass_t if needed
	if( rate_sched_testclass )	rate_sched_testclass->item_unlink(this);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
rate_err_t	rate_sched_nunit_item_t::start(rate_sched_testclass_t *rate_sched_testclass
				, const rate_prec_t &rate_prec, const delay_t &chunk_init_delay
				, size_t chunk_stop_len)			throw()
{
	rate_sched_t *	rate_sched	= rate_sched_testclass->rate_sched;
	rate_err_t	rate_err;

	// copy the parameter
	this->rate_sched_testclass	= rate_sched_testclass;
	this->chunk_init_delay		= chunk_init_delay;
	this->chunk_stop_len		= chunk_stop_len;
	// link this object to the rate_sched_testclass_t
	rate_sched_testclass->item_dolink(this);

	// reset the chunk_sent_len
	this->chunk_sent_len		= 0;

	// start the rate_limit_t
	rate_limit		= nipmem_new rate_limit_t();
	rate_err		= rate_limit->start(rate_sched, rate_prec, this, NULL);
	if( rate_err.failed() )	return rate_err;

	// init the socket_itor_arg_t
	socket_itor_arg_t	itor_arg;
	itor_arg.remote_addr("tcp://192.168.1.3:9").profile(socket_profile_t("tcp"))
					.domain(socket_domain_t::TCP).type(socket_type_t::STREAM);
	// start the socket_client_t
	socket_err_t	socket_err;
	socket_client	= nipmem_new socket_client_t();	
	socket_err	= socket_client->start(itor_arg, this, NULL);
	if( socket_err.failed() )	return rate_err_t(rate_err_t::ERROR, socket_err.to_string());

	// return no error
	return rate_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       init_timeout callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	rate_sched_nunit_item_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw()
{
	// log to debug
	KLOG_ERR("enter");
	// stop the init_timeout
	init_timeout.stop();
	// try to send data
	bool	tokeep	= try_send_data();
	if( !tokeep )	return false;
	// return tokeep
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       rate_limit_t callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref rate_limit_t becomes unblocked
 */
bool	rate_sched_nunit_item_t::neoip_rate_limit_cb(void *cb_userptr, rate_limit_t &cb_rate_limit)	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// try to send data
	bool	tokeep	= try_send_data();
	if( !tokeep )	return false;
	// return tokeep
	return true;
}

/** \brief Try to send data
 * 
 * @return a tokeep for the rate_sched_nunit_item_t
 */
bool	rate_sched_nunit_item_t::try_send_data()				throw()
{
	size_t			request_len	= std::min(4000000u, chunk_stop_len - chunk_sent_len);
	rate_limit_tmp_t	rate_tmp;

	// log to debug
	KLOG_ERR("enter *********************************** this=" << this);
	KLOG_ERR("enter request_len=" << request_len << " maxi_rate=" << rate_limit->maxi_prec());
	
	// if there are nomore data to write, dont try to
	if( request_len == 0 )	return true;
	
	// request the data to the rate_limit_t
	size_t		allowed_len;
	allowed_len	= rate_limit->data_request(request_len, rate_tmp);
	
//	size_t		written_len	 = std::min(allowed_len/2, request_len);
	size_t		written_len	 = allowed_len;

	// build the data to send
	datum_t	datum(written_len);
	char *	ptr	= (char *)datum.get_ptr();
	for(size_t i = 0; i < datum.size(); i++)	ptr[i] = i;
	// send the data thru the socket
	socket_client->send( datum );
	// update the chunk_sent_len
	chunk_sent_len	+= datum.size();

	// notify the data actually sent
	rate_limit->data_notify(written_len, rate_tmp);

	// log to debug
	KLOG_ERR("enter allowed_len=" << allowed_len << " written_len=" << written_len
				<< " maxi_prec=" << rate_limit->maxi_prec()
				<< " used_prec=" << rate_limit->used_prec()
				);
	// return tokeep
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_client_t to provide event
 */
bool	rate_sched_nunit_item_t::neoip_socket_client_event_cb(void *userptr
						, socket_client_t &cb_socket_client
						, const socket_event_t &socket_event) throw()
{
	// log to debug
	KLOG_ERR("enter event=" << socket_event);
	// sanity check - the event MUST be client_ok
	DBG_ASSERT( socket_event.is_client_ok() );

	// handle the fatal events
	if( socket_event.is_fatal() ){
		nipmem_delete	this;
		return false;
	}

	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::CNX_ESTABLISHED:
				// start the xmit_timeout
				init_timeout.start(chunk_init_delay, this, NULL);
				// return tokeep
				return true;
	default:	DBG_ASSERT(0);
	}

	// return tokeep
	return true;
}

NEOIP_NAMESPACE_END;




