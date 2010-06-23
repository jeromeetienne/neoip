/*! \file
    \brief Class to handle the bt_scasti_http_t

\par Brief Description
bt_scasti_http_t reads data from a http connection (no requirement on seekability)
and write it in a bt_io_pfile_t as a "circular buffer"
- it starts writing at the offset 0
- and will warp around the buffer size (i.e. bt_mfile.totfile_size) when it is
  reached.

\par About chunk notification
- when a chunk is notified, it is already written to disk
  - so it can be read if needed
- the bt_scasti_http_t::cur_offset() is the one at which the chunk starts
  - aka the length of the chunk is updated AFTER the callback notification

\par About the circularidx
- bt_scasti_http_t do not handle any of the circularidx, it is handled by
  the bt_io_pfile_t

*/

/* system include */
/* local include */
#include "neoip_bt_scasti_http.hpp"
#include "neoip_bt_scasti_event.hpp"
#include "neoip_bt_scasti_mod_vapi.hpp"
#include "neoip_bt_scasti_mod_raw.hpp"
#include "neoip_bt_scasti_mod_flv.hpp"
#include "neoip_bt_io_write.hpp"
#include "neoip_bt_io_vapi.hpp"
#include "neoip_bt_io_pfile.hpp"
#include "neoip_http_client.hpp"
#include "neoip_http_rephd.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
bt_scasti_http_t::bt_scasti_http_t()	throw()
{
	// zero some fields
	m_io_vapi	= NULL;
	m_mod_vapi	= NULL;
	http_client	= NULL;
	socket_full	= NULL;
	bt_io_write	= NULL;
	m_cur_offset	= 0;
}

/** \brief Destructor
 */
bt_scasti_http_t::~bt_scasti_http_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the bt_scasti_mod_vapi_t if needed
	nipmem_zdelete	m_mod_vapi;
	// delete the bt_io_write_t if needed
	nipmem_zdelete	bt_io_write;
	// delete http_client_t if needed
	nipmem_zdelete	http_client;
	// delete socket_full_t if needed
	nipmem_zdelete	socket_full;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
bt_scasti_http_t &	bt_scasti_http_t::set_profile(const bt_scasti_http_profile_t &profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( profile.check() == bt_err_t::OK );
	// copy the parameter
	this->profile	= profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
bt_err_t	bt_scasti_http_t::start(const http_uri_t &m_scasti_uri, bt_io_vapi_t *m_io_vapi
				, const bt_scasti_mod_type_t &mod_type
				, bt_scasti_cb_t *callback, void *userptr) 	throw()
{
	// log to debug
	KLOG_ERR("enter");
	// copy the parameters
	this->m_scasti_uri	= m_scasti_uri;
	this->m_io_vapi		= m_io_vapi;
	this->callback		= callback;
	this->userptr		= userptr;

	// init the static link http - to ease access while debugging
	wikidbg_obj_add_static_page("/bt_scasti_http_" + OSTREAMSTR(this));

	// Start a http_client_t
	http_err_t	http_err;
	http_client	= nipmem_new http_client_t();
	http_err	= http_client->start(http_reqhd_t().uri(m_scasti_uri), this, NULL);
	if( http_err.failed() )	return bt_err_from_http(http_err);

	// start the bt_scasti_mod_vapi_t
	bt_err_t	bt_err;
	bt_err		= mod_vapi_ctor(mod_type);
	if( bt_err.failed() )	return bt_err;

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_scasti_mod_vapi_t stuff
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Construct the bt_scasti_mod_vapi_t based on the bt_scasti_mod_type_t
 */
bt_err_t	bt_scasti_http_t::mod_vapi_ctor(const bt_scasti_mod_type_t &mod_type)	throw()
{
	bt_err_t		bt_err	= bt_err_t::OK;
	bt_scasti_mod_raw_t *	mod_raw;
	bt_scasti_mod_flv_t *	mod_flv;
	// construct the bt_scasti_mod_vapi_t depending on the bt_scasti_mod_type_t
	switch( mod_type.get_value() ){
	case bt_scasti_mod_type_t::RAW:	mod_raw		= nipmem_new bt_scasti_mod_raw_t();
					m_mod_vapi	= mod_raw;
					bt_err		= mod_raw->profile(profile.mod_raw())
								.start(this);
					break;
	case bt_scasti_mod_type_t::FLV:	mod_flv		= nipmem_new bt_scasti_mod_flv_t();
					m_mod_vapi	= mod_flv;
					bt_err		= mod_flv->start(this);
					break;
	default:			DBG_ASSERT( 0 );
	}
	// return noerror
	return bt_err_t::OK;
}

/** \brief Called by bt_scasti_mod_vapi_t to notify event to the caller
 */
bool	bt_scasti_http_t::mod_vapi_notify_callback(const bt_scasti_event_t &event)	throw()
{
	// just forward the bt_scasti_event_t to the caller
	return notify_callback(event);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    http_client_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref http_client_t when to notify an event
 */
bool	bt_scasti_http_t::neoip_http_client_cb(void *cb_userptr, http_client_t &cb_http_client
				, const http_err_t &http_err, const http_rephd_t &http_rephd
				, socket_full_t *socket_full, const bytearray_t &recved_data) throw()
{
	// log to debug
	KLOG_DBG("enter http_err=" << http_err << " http_rephd=" << http_rephd << "recved_data=" << recved_data);

	// if the connection failed, notify the caller
	if( http_err.failed() )	return notify_callback_failed( bt_err_from_http(http_err) );
	// TODO is that the the only stuff to test for the http_client_t
	// - this http client stuff is poorly made

	// setup the socket_full_t - it is already started by http_client_t
	this->socket_full	= socket_full;
	socket_full->set_callback(this, NULL);

	// If there is already a recved_data, handle it now
	if( !recved_data.empty() ){
		// convert the pkt_t * socket_event.get_recved_data() into a bytearray_t
		// TODO clean this up - bytearray_t and pkt_t are both inheriting from serial_t
		pkt_t	pkt;
		pkt.work_on_data_nocopy(recved_data.void_ptr(), recved_data.length());
		// handle it as if it cames from the socket_full_t
		bool	tokeep 	= handle_recved_data(pkt);
		if( !tokeep )	return false;
	}

	// backup the parameters of this http_client_t parameter in case of forward to http_client_pool_t
	// delete the http_client_t
	nipmem_zdelete	http_client;

	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     socket_full_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_full_t to provide event
 */
bool	bt_scasti_http_t::neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
					, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( socket_event.is_full_ok() );
	// sanity check - the socket_full MUST be set
	DBG_ASSERT( socket_full );

	// handle the fatal events
	if( socket_event.is_fatal() ){
		std::string	reason = "HTTP socket reported fatal event " + socket_event.to_string();
		return notify_callback_failed(bt_err_t(bt_err_t::ERROR, reason));
	}

	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::RECVED_DATA:
			return handle_recved_data( *socket_event.get_recved_data() );
	default:	DBG_ASSERT( 0 );
	}

	// return tokeep
	return true;
}

/** \brief Handle the socket_event_t::RECVED_DATA event
 *
 * @return a tokeep for the socket_full_t
 */
bool	bt_scasti_http_t::handle_recved_data(pkt_t &pkt)	throw()
{
	// log to debug
	KLOG_DBG("enter pkt=" << pkt);

	// notify the data to the bt_scasti_mod_vapi_t
	m_mod_vapi->notify_data(pkt.to_datum(datum_t::NOCOPY));

	// sanity check - no bt_io_write_t MUST be inprogress
	DBG_ASSERT( bt_io_write == NULL );

	// launcht the bt_io_write_t
	// - a bt_io_write_t NEVER fails on launch
	// - bt_io_vapi_t will handle the circularidx if needed
	file_range_t	file_range(cur_offset(), cur_offset() + pkt.length()-1);
	bt_io_write	= m_io_vapi->write_ctor(file_range, pkt.to_datum(datum_t::NOCOPY)
							, this, NULL);

	// stop the socket_full_t from reading - BUT only while writing the data on disk
	socket_full->rcvdata_maxlen(0);

	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        bt_io_write_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref file_aread_t when to notify the result of the operation
 */
bool	bt_scasti_http_t::neoip_bt_io_write_cb(void *cb_userptr, bt_io_write_t &cb_io_write
						, const bt_err_t &bt_err)	throw()
{
	// log to debug
	KLOG_DBG("enter bt_err=" << bt_err << " written_len=" << bt_io_write->written_len());

	// if the bt_io_write_t failed, notify an error
	if( bt_err.failed() )	return notify_callback_failed(bt_err);

	// build the bt_scasti_event_t::CHUNK_AVAIL
	bt_scasti_event_t scasti_event;
	scasti_event	= bt_scasti_event_t::build_chunk_avail(bt_io_write->written_len());
	bool	tokeep	= notify_callback(scasti_event);
	if( !tokeep )	return false;

	// update the m_cur_offset
	// - it MUST be done AFTER the callback notification
	// - thus the notified function may read cur_offset
	m_cur_offset	+= bt_io_write->written_len();

	// autodelete this sched_block and return dontkeep
	nipmem_zdelete	bt_io_write;

	// start reading from the socket_full_t again
	socket_full->rcvdata_maxlen(profile.rcvdata_maxlen());

	// return dontkeep - as the bt_io_write_t as just been deleted
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback in case of a faillure
 */
bool	bt_scasti_http_t::notify_callback_failed(const bt_err_t &bt_err)		throw()
{
	// build the bt_scasti_event_t
	bt_scasti_event_t scasti_event	= bt_scasti_event_t::build_error(bt_err);
	// forward to notify_callback
	return notify_callback(scasti_event);
}

/** \brief notify the callback with the bt_scasti_event_t
 */
bool	bt_scasti_http_t::notify_callback(const bt_scasti_event_t &scasti_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_scasti_cb(userptr, *this, scasti_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}



NEOIP_NAMESPACE_END





