/*! \file
    \brief Class to handle the rtmp_full_t


*/

/* system include */
/* local include */
#include "neoip_rtmp_full.hpp"
#include "neoip_rtmp_event.hpp"
#include "neoip_rtmp_parse.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
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
rtmp_full_t::rtmp_full_t(socket_full_t *p_socket_full, const bytearray_t &recved_data)	throw()
{
	// zero some fields
	this->socket_full	= p_socket_full;
	// ensure the socket_full_t has no callback
	socket_full->set_callback(NULL, NULL);

	// create the rtmp_parse_t
	rtmp_err_t	rtmp_err;
	rtmp_parse	= nipmem_new rtmp_parse_t();
	rtmp_err	= rtmp_parse->start(this, NULL);
	DBG_ASSERT(rtmp_err.succeed());

	// notify the already received data to rtmp_parse_t
	// - NOTE: rtmp_parse_t will parse them after a zerotimer_t
	KLOG_DBG("recved_data" << recved_data);
	rtmp_parse->notify_data(recved_data.to_datum(datum_t::NOCOPY));
}

/** \brief Destructor
 */
rtmp_full_t::~rtmp_full_t()		throw()
{
	// delete the socket_full_t if needed
	nipmem_zdelete	socket_full;
	// delete the rtmp_parse_t if needed
	nipmem_zdelete	rtmp_parse;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
rtmp_err_t	rtmp_full_t::start(rtmp_full_cb_t *callback, void *userptr)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->callback	= callback;
	this->userptr	= userptr;
	// set up the socket_full_t
	socket_full->set_callback(this, NULL);
	// return no error
	return rtmp_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     action function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Send data thru the rtmp_full_t
 *
 * TODO this writing technic is no good
 * - why writing only pure bytestream and not rtmp_pkthd_t + dvar or something
 * - no handling of MAYSEND
 *   - big assumption that
 * - TODO copy the sendq stuff from bt_full_t ?
 *   - not needed for now
 *   - is this over engineering ?
 *   - should i rename this ?
 *   - should i give access to socket_full directly to make it official
 */
rtmp_err_t	rtmp_full_t::send(const void *data_ptr, size_t data_len) 	throw()
{
	socket_err_t	socket_err;
	// send the data thru the socket directly
	socket_err	= socket_full->send(data_ptr, data_len);
	if( socket_err.failed() )	return rtmp_err_from_socket(socket_err);
	// return no error
	return rtmp_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_full_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_full_t to provide event
 */
bool	rtmp_full_t::neoip_socket_full_event_cb(void *userptr
			, socket_full_t &cb_socket_full, const socket_event_t &socket_event) throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( socket_event.is_full_ok() );

	// if the socket_event_t is fatal, notify a rtmp_event_t::ERROR
	if( socket_event.is_fatal() ){
		rtmp_err_t	rtmp_err(rtmp_err_t::ERROR, socket_event.to_string());
		return notify_callback(rtmp_event_t::build_error(rtmp_err));
	}


	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::RECVED_DATA:{
			pkt_t *	pkt	= socket_event.get_recved_data();
			// log to debug
			KLOG_DBG("data len=" << pkt->length());
			// notify the just received data to rtmp_parse_t
			// - NOTE: rtmp_parse_t will parse them after a zerotimer_t
			rtmp_parse->notify_data(pkt->to_datum(datum_t::NOCOPY));
			// parse the recved command
			return true;}
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     rtmp_parse_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref rtmp_parse_t to provide event
 */
bool	rtmp_full_t::neoip_rtmp_parse_cb(void *userptr, rtmp_parse_t &cb_rtmp_parse
					, const rtmp_event_t &rtmp_event) throw()
{
	// log to debug
	KLOG_DBG("enter event=" << rtmp_event);
	// sanity check - rtmp_event_t MUST be is_parse_ok()
	DBG_ASSERT( rtmp_event.is_parse_ok() );

	// simply forward the event to the caller
	return notify_callback(rtmp_event);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool rtmp_full_t::notify_callback(const rtmp_event_t &event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_rtmp_full_cb(userptr, *this, event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





