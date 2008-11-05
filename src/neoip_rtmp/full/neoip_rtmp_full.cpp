/*! \file
    \brief Class to handle the rtmp_full_t


*/

/* system include */
/* local include */
#include "neoip_rtmp_full.hpp"
#include "neoip_rtmp_event.hpp"
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
rtmp_full_t::rtmp_full_t(socket_full_t *p_socket_full
				, const bytearray_t &p_recved_data)	throw()
{
	// zero some fields
	this->socket_full	= p_socket_full;
	this->m_recved_data	= p_recved_data;
	KLOG_ERR("m_recved_data" << m_recved_data);
	// ensure the socket_full_t has no callback
	socket_full->set_callback(NULL, NULL);
}

/** \brief Destructor
 */
rtmp_full_t::~rtmp_full_t()		throw()
{
	// delete the socket_full_t if needed
	nipmem_zdelete	socket_full;
}

/** \brief Autodelete this object
 */
bool rtmp_full_t::autodelete()	throw()
{
	// autodelete itself
	nipmem_delete	this;
	// return false - to 'simulate' a dontkeep
	return false;
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
	// set the zerotimer to parse the just recved_data
	first_parse_zerotimer.append(this, NULL);
	// return no error
	return rtmp_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	rtmp_full_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// sanity check - the zerotimer_t MUST be first_parse_zerotimer
	DBG_ASSERT( &cb_zerotimer == &first_parse_zerotimer );
	// parse the receved_data
	return parse_recved_data();
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
			// queue the received data to the one already received
			m_recved_data.append(pkt->void_ptr(), pkt->length());
			// parse the recved command
			return parse_recved_data();}
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         generic command parsing
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief parse the received commands
 *
 * - it read a whole command out of this->recved_data and then pass it to parse_recved_cmd()
 *
 * @return a tokeep for the whole rtmp_full_t
 */
bool	rtmp_full_t::parse_recved_data()	throw()
{
	// TODO do something here :)

	// return tokeep
	return true;
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





