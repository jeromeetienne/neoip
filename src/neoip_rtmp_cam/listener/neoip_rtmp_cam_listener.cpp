/*! \file
    \brief Definition of the \ref rtmp_cam_listener_t

*/

/* system include */
/* local include */
#include "neoip_rtmp_cam_listener.hpp"
#include "neoip_rtmp_cam_resp.hpp"
#include "neoip_rtmp_cam_full.hpp"
#include "neoip_rtmp_event.hpp"
#include "neoip_rtmp_resp.hpp"
#include "neoip_rtmp_full.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
rtmp_cam_listener_t::rtmp_cam_listener_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	rtmp_resp	= NULL;
}

/** \brief Destructor
 */
rtmp_cam_listener_t::~rtmp_cam_listener_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the rtmp_resp if needed
	nipmem_zdelete rtmp_resp;
	// close all pending rtmp_cam_resp_t
	while( !cam_resp_db.empty() )	nipmem_delete cam_resp_db.front();
	// close all pending rtmp_cam_full_t
	while( !cam_full_db.empty() ){
		rtmp_cam_full_t * cam_full	= cam_full_db.front();
		full_unlink(cam_full);
		nipmem_delete cam_full;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
rtmp_err_t	rtmp_cam_listener_t::start(const socket_resp_arg_t &resp_arg)	throw()
{
	rtmp_err_t	rtmp_err;
	// start the rtmp_resp_t
	rtmp_resp	= nipmem_new rtmp_resp_t();
	rtmp_err	= rtmp_resp->start(resp_arg, this, NULL);
	if( rtmp_err.failed() )	return rtmp_err;

	// return no error
	return rtmp_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return a pointer on a rtmp_cam_resp_t which matches this http_uri_t, or NULL if none does
 */
rtmp_cam_resp_t *	rtmp_cam_listener_t::find_resp(const http_uri_t &connect_uri)	throw()
{
	std::list<rtmp_cam_resp_t *>::iterator	iter;
	// go thru the whole resp_db
	for(iter = cam_resp_db.begin(); iter != cam_resp_db.end(); iter++){
		rtmp_cam_resp_t *	cam_resp	= *iter;
		// if this rtmp_cam_resp_t match the connect_uri, return it
		if( cam_resp->may_handle(connect_uri) )	return cam_resp;
	}
	// if this point is reached, no rtmp_cam_resp_t matches this http_reqhd_t
	return NULL;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			rtmp_resp_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref rtmp_resp_t when to notify an event
 */
bool	rtmp_cam_listener_t::neoip_rtmp_resp_cb(void *cb_userptr, rtmp_resp_t &cb_rtmp_resp
						, rtmp_full_t *rtmp_full)	throw()
{
	// log to debug
	KLOG_ERR("enter");

	// create a rtmp_cam_full_t from the spawned rtmp_full_t
	rtmp_cam_full_t*cam_full;
	rtmp_err_t	rtmp_err;
	cam_full	= nipmem_new rtmp_cam_full_t();
	rtmp_err	= cam_full->start(rtmp_full, this, NULL);
	if( rtmp_err.failed() ){
		nipmem_zdelete	cam_full;
		return true;
	}
	// link it to this rtmp_cam_listener_t
	full_dolink(cam_full);

	// return tokeep
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     rtmp_cam_full_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref rtmp_cam_full_t to provide event
 */
bool	rtmp_cam_listener_t::neoip_rtmp_cam_full_cb(void *userptr, rtmp_cam_full_t &cb_cam_full
					, const rtmp_event_t &rtmp_event) throw()
{
	rtmp_cam_full_t * cam_full	= &cb_cam_full;
	// log to debug
	KLOG_ERR("enter event=" << rtmp_event);
	// sanity check - rtmp_event_t MUST be is_cam_full_ok()
	DBG_ASSERT( rtmp_event.is_cam_full_ok() );

	// if rtmp_event.is_fatal(), simply delete the rtmp_cam_full_t
	if( rtmp_event.is_fatal() ){
		full_unlink(cam_full);
		nipmem_zdelete cam_full;
		return false;
	}

	// handle each possible events from its type
	switch( rtmp_event.value() ){
	case rtmp_event_t::CONNECTED:
			return handle_event_connected(cam_full, rtmp_event);
	default:	// simply discarded
			KLOG_ERR("discard rtmp_event=" << rtmp_event);
	}

	// return tokeep
	return true;
}

/** \brief handle rtmp_event_t for rtmp_event_t::CONNECTED
 *
 * @return tokeep for the rtmp_cam_full_t
 */
bool	rtmp_cam_listener_t::handle_event_connected(rtmp_cam_full_t *cam_full
					, const rtmp_event_t &rtmp_event)	throw()
{
	// sanity check - rtmp_event_t MUST be is_connected()
	DBG_ASSERT( rtmp_event.is_connected() );

	KLOG_ERR("received a connect for " << rtmp_event.get_connected_uri());

	// get the connect_uri
	http_uri_t	connect_uri;
	connect_uri	= rtmp_event.get_connected_uri();
	// try to find the rtmp_cam_resp_t for this connect_uri
	rtmp_cam_resp_t*cam_resp;
	cam_resp	= find_resp(connect_uri);
	// if none matches, delete rtmp_cam_full_t and return dontkeep
	if( cam_resp == NULL ){
		full_unlink(cam_full);
		nipmem_zdelete cam_full;
		return false;
	}

	// backup the object_slotid of the rtmp_cam_full_t - to be able to return its tokeep value
	slot_id_t	cam_full_slotid	= cam_full->get_object_slotid();
	// notify this rtmp_cam_full_t thru rtmp_cam_resp_t
	full_unlink(cam_full);
	cam_resp->notify_new_cnx(cam_full);
	// if the rtmp_cam_full_t has no been deleted, so 'tokeep' else return 'dontkeep'
	return object_slotid_tokeep(cam_full_slotid);
}

NEOIP_NAMESPACE_END;




