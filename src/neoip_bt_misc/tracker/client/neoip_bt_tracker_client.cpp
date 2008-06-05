/*! \file
    \brief Class to handle the bt_tracker_client_t

*/

/* system include */
/* local include */
#include "neoip_bt_tracker_client.hpp"
#include "neoip_bt_tracker_request.hpp"
#include "neoip_bt_tracker_reply.hpp"
#include "neoip_bencode.hpp"
#include "neoip_http_sclient.hpp"
#include "neoip_socket_itor_arg.hpp"
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
bt_tracker_client_t::bt_tracker_client_t()		throw()
{
	// zero some field
	http_sclient	= NULL;
	itor_arg	= NULL;
}

/** \brief Destructor
 */
bt_tracker_client_t::~bt_tracker_client_t()		throw()
{
	// delete the socket_itor_arg_t if needed
	nipmem_zdelete	itor_arg;	
	// delete the http_client_t if needed
	nipmem_zdelete	http_sclient;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
bt_tracker_client_t &	bt_tracker_client_t::set_profile(const bt_tracker_client_profile_t &profile)throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == bt_err_t::OK );	
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Set the socket_itor_arg_t for this object
 */
bt_tracker_client_t &	bt_tracker_client_t::set_itor_arg(const socket_itor_arg_t &itor_arg)throw()
{
	// copy the itor_arg
	this->itor_arg	= nipmem_new socket_itor_arg_t(itor_arg);	
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
bt_err_t	bt_tracker_client_t::start(const bt_tracker_request_t &request, bt_tracker_client_cb_t *callback
							, void *userptr)	throw()
{
	http_err_t	http_err;
	// sanity check - the request MUST NOT be null
	DBG_ASSERT( !request.is_null() );
	// log to debug
	KLOG_DBG("enter bt_tracker_request=" << request);
	KLOG_DBG("request full_uri=" << request.build_full_uri());
	// copy the parameter
	this->callback	= callback;
	this->userptr	= userptr;

	// start the http_client_t for the request
	http_sclient	= nipmem_new http_sclient_t();
	if( itor_arg )	http_sclient->set_itor_arg(*itor_arg);
	http_err	= http_sclient->set_profile(profile.http_sclient())
					.start(request.build_full_uri(), this, NULL);
	if( http_err.failed() )	return bt_err_from_http(http_err);
	
	// delete the socket_itor_arg_t if needed
	nipmem_zdelete	itor_arg;

	// return no error
	return bt_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     http_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref http_client_t to provide event
 */
bool	bt_tracker_client_t::neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_http_sclient
						, const http_sclient_res_t &sclient_res)	throw() 
{
	// log to debug
	KLOG_DBG("enter sclient_res=" << sclient_res);
	KLOG_DBG("enter sclient_res body=" << sclient_res.reply_body().to_datum());

	// if the http_client_t failed, notify the caller
	if( !sclient_res.full_get_ok() )
		return notify_callback(bt_err_t::ERROR, bt_tracker_reply_t() );
	
	// parse the reply
	bt_tracker_reply_t	bt_tracker_reply;
	bt_tracker_reply	= bt_tracker_reply_t::from_bencode(sclient_res.reply_body().to_datum());
	// if the parsing failed, notify the caller
	if( bt_tracker_reply.is_null() ){
		bt_err_t bt_err	= bt_err_t(bt_err_t::ERROR, "poorly formed tracker server reply");
		notify_callback(bt_err, bt_tracker_reply_t());		
		return false;
	}
	// notify the result
	notify_callback(bt_err_t::OK, bt_tracker_reply);
	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool bt_tracker_client_t::notify_callback(const bt_err_t &bt_err
				, const bt_tracker_reply_t &reply)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_tracker_client_cb(userptr, *this, bt_err, reply);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}



NEOIP_NAMESPACE_END





