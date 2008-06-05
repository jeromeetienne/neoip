/*! \file
    \brief Class to handle the bt_tracker_server_t

*/

/* system include */
/* local include */
#include "neoip_bt_tracker_server.hpp"
#include "neoip_bt_tracker_request.hpp"
#include "neoip_bt_tracker_reply.hpp"
#include "neoip_httpd.hpp"
#include "neoip_file_path.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_tracker_server_t::bt_tracker_server_t()		throw()
{
	// zero some fields
	httpd	= NULL;
}

/** \brief Destructor
 */
bt_tracker_server_t::~bt_tracker_server_t()		throw()
{
	// remove the handler if needed
	if( httpd )	httpd->handler_del(handler_path.to_string(), this, NULL);
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_tracker_server_t::start(httpd_t *httpd, const file_path_t &handler_path
				, bt_tracker_server_cb_t *callback, void *userptr)		throw()
{
	// copy the parameter
	this->httpd		= httpd;
	this->handler_path	= handler_path;
	this->callback		= callback;
	this->userptr		= userptr;
	
	// register the handler
	httpd->handler_add(handler_path.to_string(), this, NULL);

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//             reply notification by the caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Notify the bt_tracker_reply_t in answer to a bt_tracker_request previouslly notified
 * 
 * - warning: it MUST NOT be done *during* the callback notification
 */
void	bt_tracker_server_t::notify_reply(slot_id_t cnx_slotid, const bt_tracker_request_t &request
					, const bt_tracker_reply_t &reply)		throw()
{
	std::map<slot_id_t, httpd_request_t>::iterator	iter;
	// sanity check - the bt_tracker_reply_t MUST NOT be null
	DBG_ASSERT( !reply.is_null() );
	// sanity check - the bt_tracker_request_t MUST NOT be null
	DBG_ASSERT( !request.is_null() );
	// find the matching httpd_request_db entry
	iter	= httpd_request_db.find(cnx_slotid);
	// sanity check - the cnx_slotid MUST be in the httpd_request_db
	DBG_ASSERT( iter != httpd_request_db.end() );
	// copy the httpd_request_t
	httpd_request_t	httpd_request	= iter->second;
	// remove it from the httd_request_db
	httpd_request_db.erase(cnx_slotid);
	
	// put the reply into the httpd reply
	httpd_request.get_reply() << string_t::from_datum( reply.to_bencode(request) );
	// notify the reply
	httpd->notify_delayed_reply(httpd_request, httpd_err_t::OK);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                             HTTPD callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when a httpd request is received by this neoip_httpd_handler
 */
httpd_err_t bt_tracker_server_t::neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &httpd_request)
										throw()
{
	bt_tracker_request_t	bt_tracker_request;
	bt_tracker_reply_t		bt_tracker_reply;
	// log to debug
	KLOG_DBG("enter");


	// build the bt_tracker_request_t from the httpd_request_t variable
	bt_tracker_request	= bt_tracker_request_t::from_httpd_request(httpd_request);

	// notify the bt_tracker_request to the caller
	bool	tokeep	= notify_callback(httpd_request.get_slot_id(), bt_tracker_request, bt_tracker_reply);
	if( !tokeep )	return httpd_err_t::INTERNAL_ERROR;

	// if the bt_tracker_reply_t is not null, the callback specified the reply in sync
	if( !bt_tracker_reply.is_null() ){
		// put the reply into the httpd reply
		httpd_request.get_reply() << string_t::from_datum(bt_tracker_reply.to_bencode(bt_tracker_request));
		// return no error
		return httpd_err_t::OK;
	}

	// NOTE: from here handle the reply as delayed
	
	// queue the httpd_request_t
	bool	succeed	= httpd_request_db.insert(std::make_pair(httpd_request.get_slot_id(), httpd_request)).second;
	DBG_ASSERT( succeed );

	// delay the reply to this httpd_request_t
	return httpd_err_t::DELAYED_REPLY;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool bt_tracker_server_t::notify_callback(slot_id_t cnx_slotid, const bt_tracker_request_t &request
						, bt_tracker_reply_t &reply_out)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_tracker_server_cb(userptr, *this, cnx_slotid, request, reply_out);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}



NEOIP_NAMESPACE_END





