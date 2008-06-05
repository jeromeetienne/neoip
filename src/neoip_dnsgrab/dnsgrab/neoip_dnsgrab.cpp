/*! \file
    \brief Definition of the \ref dnsgrab_t class

\par Possible Improvements
- a security layer to prevent connection from other users
  - put a nonce in config file read by nss_neoip
  - check the file is readable/writable only by the user
  - if the tcp connection doesnt contains this nounce, reject the connection

*/

/* system include */
/* local include */
#include "neoip_dnsgrab.hpp"
#include "neoip_dnsgrab_arg.hpp"
#include "neoip_dnsgrab_cnx.hpp"
#include "neoip_dnsgrab_register.hpp"
#include "neoip_tcp.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                          ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
dnsgrab_t::dnsgrab_t(dnsgrab_cb_t *callback, void *userptr)		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// copy some parameter
	this->callback	= callback;
	this->userptr	= userptr;
	// zero the tcp_resp
	tcp_resp	= NULL;
	// zero the dnsgrab_register
	dnsgrab_register= NULL;
}

/** \brief Destructor
 */
dnsgrab_t::~dnsgrab_t()		throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete all the pending connection
	while( !cnx_db.empty() )	nipmem_delete cnx_db.front();
	// delete the responder	if needed
	nipmem_zdelete tcp_resp;
	// delete the dnsgrab_register_t if needed
	nipmem_zdelete dnsgrab_register;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         start function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the action
 * 
 * @param listen_addr		the tcp address on which to listen
 * @param reg_location		the location of the registration ("user"|"system")
 * @param reg_priority		the priority of the registration [0,999]
 * @param reg_request_ttl	the amount of time the querier will wait for an answer before expiring
 * @param reg_tag		the tag of the registered filename (if not provided, it default to "dnsgrab")
 * @param reg_nounce		the nounce of the registered filename (if not provided, it default to rand())
 * 
 * @return a dnsgrab_err_t
 */
dnsgrab_err_t	dnsgrab_t::start(const ipport_addr_t &listen_addr, std::string reg_location
				, size_t reg_priority, const delay_t &reg_request_ttl
				, const std::string &reg_tag, const std::string &reg_nounce)	throw()
{
	dnsgrab_err_t	dnsgrab_err;
	inet_err_t	inet_err;
	// start the tcp_resp_t
	tcp_resp	= nipmem_new	tcp_resp_t();
	inet_err 	= tcp_resp->start(listen_addr, this, NULL);	
	if( inet_err.failed() )		return dnsgrab_err_from_inet(inet_err);

	// log to debug
	KLOG_DBG("enter listen_addr=" << tcp_resp->get_listen_addr());

	// create the grabber register
	dnsgrab_register= nipmem_new dnsgrab_register_t();
	dnsgrab_err 	= dnsgrab_register->start(reg_location, reg_priority, tcp_resp->get_listen_addr()
							, reg_request_ttl, reg_tag, reg_nounce);
	if( dnsgrab_err.failed() )	return dnsgrab_err;

	// return no error
	return dnsgrab_err_t::OK;
}


/** \brief Start helper function from a dnsgrab_arg_t
 */
dnsgrab_err_t	dnsgrab_t::start(const dnsgrab_arg_t &dnsgrab_arg)			throw()
{
	// sanity check - the arg MUST be valid
	DBG_ASSERT( dnsgrab_arg.is_valid() );
	// do call the start() now
	return start( dnsgrab_arg.listen_addr(), dnsgrab_arg.reg_location()
				, dnsgrab_arg.reg_priority(), dnsgrab_arg.reg_request_ttl()
				, dnsgrab_arg.reg_tag(), dnsgrab_arg.reg_nonce());
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     reponder callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref tcp_resp_t when a connection is established
 */
bool	dnsgrab_t::neoip_tcp_resp_event_cb(void *userptr, tcp_resp_t &cb_tcp_resp
							, const tcp_event_t &tcp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << tcp_event);
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( tcp_event.is_resp_ok() );
	
	// handle each possible events from its type
	switch( tcp_event.get_value() ){
	case tcp_event_t::CNX_ESTABLISHED:{
			// spawn a dnsgrab_cnx_t which gonna do handle the request sent over it
			tcp_full_t *	tcp_full = tcp_event.get_cnx_established();
			nipmem_new	dnsgrab_cnx_t(this, tcp_full);
			break;}
	default:	DBG_ASSERT(0);
	}	
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          for reply notification
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief to notify a reply for a previously notify dnsgrab_request_t
 */
dnsgrab_err_t	dnsgrab_t::notify_reply(const dnsgrab_request_t &request)	throw()
{
	std::list<dnsgrab_cnx_t *>::iterator	iter;
	dnsgrab_cnx_t *				cnx = NULL;
	// try to find the connection matching this slot_id
	for( iter = cnx_db.begin(); iter != cnx_db.end(); iter++ ){
		cnx = *iter;
		if( cnx->get_slot_id() == request.get_cnx_slot_id() )	break;
	}
	// if no connection matches, log the event and return
	if( iter == cnx_db.end() ){
		std::string	reason = "Tried to send a dnsgrab reply to an unexisting connection.";
		return dnsgrab_err_t(dnsgrab_err_t::ERROR, reason);
	}
	// if the connection is found, ask it to send the delayed reply
	cnx->notify_reply(request);
	// return no error
	return dnsgrab_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the dnsgrab_t result
 */
bool dnsgrab_t::notify_callback(dnsgrab_request_t &request)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_dnsgrab_cb(userptr, *this, request);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}


NEOIP_NAMESPACE_END









