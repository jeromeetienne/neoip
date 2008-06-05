/*! \file
    \brief Definition of the \ref httpd_t class

\par Possible Improvement
- to port on top of socket_t once it works well
- to handle the maysend_on/off in transmit

*/

/* system include */
/* local include */
#include "neoip_httpd.hpp"
#include "neoip_httpd_cnx.hpp"
#include "neoip_tcp.hpp"
#include "neoip_string.hpp"
#include "neoip_pkt.hpp"
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
httpd_t::httpd_t()		throw()
{
	// zero some field
	tcp_resp	= NULL;
}

/** \brief Destructor
 */
httpd_t::~httpd_t()		throw()
{
	// delete all the pending connection
	while( !cnx_db.empty() )	nipmem_delete	cnx_db.front();
	
	// delete the responder	
	nipmem_zdelete tcp_resp;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         start function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the action
 * 
 * @return false if no error occured, true otherwise
 */
bool	httpd_t::start(const ipport_addr_t &listen_addr)	throw()
{
	inet_err_t	inet_err;
	// start the tcp_resp_t
	tcp_resp	= nipmem_new tcp_resp_t();
	inet_err	= tcp_resp->start(listen_addr, this, NULL);	
	if( inet_err.failed() )	KLOG_ERR("starting tcp_resp_t failed due to " << inet_err);
	if( inet_err.failed() )	return true;
	// return no error	
	return false;
}

/** \brief to notify a delayed reply for a connection
 * 
 * - so the handler MUST have returned httpd_err_t::DELAYED_REPLY when receiving
 *   the request
 */
void	httpd_t::notify_delayed_reply(const httpd_request_t &request, const httpd_err_t &httpd_err)throw()
{
	std::list<cnx_t *>::iterator	iter;
	cnx_t *				cnx = NULL;
	// try to find the connection matching this slot_id
	for( iter = cnx_db.begin(); iter != cnx_db.end(); iter++ ){
		cnx = *iter;
		if( cnx->get_slot_id() == request.get_slot_id() )	break;
	}
	// if no connection matches, log the event and return
	if( iter == cnx_db.end() ){
		KLOG_INFO("Tried to send a http reply to an unexisting connection."
						<< " slot id=" << request.get_slot_id());
		return;
	}
	// if the connection is found, ask it to send the delayed reply
	cnx->handle_send_reply(request, httpd_err);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         handler database management
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Add a handler in this \ref httpd_t
 */
void httpd_t::handler_add(const httpd_handler_t &handler)		throw()
{
	bool	succeed = handler_db.insert(std::make_pair(handler.get_path(), handler)).second;
	DBG_ASSERT( succeed );
}

/** \brief remove a handler in this \ref httpd_t
 */
void httpd_t::handler_del(const httpd_handler_t &handler)		throw()
{
	// remove it from the database
	handler_db.erase(handler.get_path());	
}

/** \brief find a handler in this \ref httpd_t
 */
httpd_handler_t *httpd_t::handler_find(const std::string &path)	throw()
{
	std::map<std::string, httpd_handler_t>::iterator	iter;
	// log to debug
	KLOG_DBG("try to find a handler for url path =" << path );
	// try to get the handler from the database
	iter = handler_db.find(path);

	// if there is an handler for this exact path, return it;
	if( iter != handler_db.end() )	return &(iter->second);
	
	// test if there is a path which matches with DEEPER_PATH_OK
	for( iter = handler_db.begin(); iter != handler_db.end(); iter++ ){
		httpd_handler_t	&	httpd_handler = iter->second;
		// if this handler is NOT  DEEPER_PATH_OK, skip it
		if( !httpd_handler.is_deeper_path_ok() )	continue;
		// if the path parameter is a deeper path of the handler's path, return this handler
		std::string	handler_path	= httpd_handler.get_path() + "/";
		if( path.substr(0, handler_path.size()) == handler_path )
			return &httpd_handler;
	}

	// if there is no handler for this path, return NULL
	return NULL;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     reponder callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref tcp_resp_t when a connection is established
 */
bool	httpd_t::neoip_tcp_resp_event_cb(void *userptr, tcp_resp_t &cb_tcp_resp
							, const tcp_event_t &tcp_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << tcp_event);
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( tcp_event.is_resp_ok() );
	
	// handle each possible events from its type
	switch( tcp_event.get_value() ){
	case tcp_event_t::CNX_ESTABLISHED:{
			// spawn a cnx_t which gonna do something
			tcp_full_t *	tcp_full = tcp_event.get_cnx_established();
			nipmem_new	cnx_t(this, tcp_full);
			break;}
	default:	DBG_ASSERT(0);
	}	
	return true;
}

NEOIP_NAMESPACE_END









