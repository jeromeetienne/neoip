/*! \file
    \brief Class to handle the bt_session_cnx_t

\par Short Description
\ref bt_session_cnx_t parses the bt_handshake_t for the connection received by
the bt_session_t. Then forward it to the proper bt_swarm_t

\par Parsing description
- the first half parsing is done up to the info_hash of the handshake
  - then if the info_hash IS NOT part of this bt_session_t, the connection is closed
  - else the local handshake is sent
- the second half parsing is done with the peerid
  - once it is read, a bt_swarm_full_t is spawned for the bt_swarm_t of this info_hash
    and this bt_session_cnx_t is autodeleted
- at any time, if the socket_full_t received a fatal event, the bt_session_cnx_t is 
  autodeleted
- the bt_handshake_t parsing is splitted in two due to a bug in the
  tracker 'nat detection' feature: the tracker doesnt send a peerid. 
  - it could send one as it is just a random number.
  - but for unknown reason it doesnt which break the bt protocol

\par Possible Improvement
- removing any new peer just because the maximum number is currently reached 
  is quite undistinctive. the new one may be better
  - what if the new peer got the piece not present in any presently connected peer
  - what if the new peer provides data faster than the current ones
  - what if the new peer is a friend of mine ? relation with the bt_peeracct_t
  - TODO make a more knowledgable choise about which connection to drop
    when the max is reached
    - drop the worst connection you got
    - to drop the new one all the time is not a suitable choise
    - when receiving a new cnx, drop the slowest one if below a given rate ?

*/

/* system include */
/* local include */
#include "neoip_bt_session_cnx.hpp"
#include "neoip_bt_session.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_full.hpp"
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
bt_session_cnx_t::bt_session_cnx_t()		throw()
{
	// zero some fields
	bt_session	= NULL;
	socket_full	= NULL;
}

/** \brief Destructor
 */
bt_session_cnx_t::~bt_session_cnx_t()		throw()
{
	// delete the socket_full_t if needed
	nipmem_zdelete	socket_full;		
	// unlink this object from the bt_session
	if( bt_session )	bt_session->cnx_unlink(this);
}

/** \brief Autodelete the object and return false to ease readability
 */
bool	bt_session_cnx_t::autodelete()		throw()
{
	nipmem_delete	this;
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief Start the operation
 */
bt_err_t	bt_session_cnx_t::start(bt_session_t *bt_session, socket_full_t *socket_full)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// copy the parameter
	this->bt_session	= bt_session;
	this->socket_full	= socket_full;
	// link this object to the bt_session
	bt_session->cnx_dolink(this);
	// start the socket_full
	socket_err_t	socket_err;
	socket_err	= socket_full->start(this, NULL);	
	if( socket_err.failed() )	return bt_err_from_socket(socket_err);	
	// return no error
	return bt_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_full_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_full_t when a connection is established
 */
bool	bt_session_cnx_t::neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
							, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_DBG("enter event=" << socket_event);
	// sanity check - the event MUST be full_ok
	DBG_ASSERT( socket_event.is_full_ok() );

	// handle the fatal events
	if( socket_event.is_fatal() )	return autodelete();
	
	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::RECVED_DATA:
			// handle the received packet
			return handle_recved_data(*socket_event.get_recved_data());
	case socket_event_t::MAYSEND_ON:
			// FIXME to review
			// those events are for reliable socket_type_t only - so NOT on DGRAM
			// fall thru
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}


/** \brief Handle received data on the cnx_t
 * 
 * @return a 'tokeep/dontkeep' for the socket_full_t
 */
bool	bt_session_cnx_t::handle_recved_data(pkt_t &pkt)	throw()
{
	// log to debug
	KLOG_DBG("enter pkt=" << pkt);
	// queue the received data to the one already received
	recved_data.append(pkt.to_datum(datum_t::NOCOPY));

	// if the first half of the remote_handshake has not yet been parsed, do it
	if( remote_handshake.infohash().is_null() ){
		bool	tokeep	= parse_first_half_handshake();
		if( !tokeep )	return false;
	}
	
	// if the first half of the remote_handshake has been parsed, parse the second part
	if( remote_handshake.infohash().is_null() == false ){
		bool	tokeep	= parse_second_half_handshake();
		if( !tokeep )	return false;
	}
	
	// return 'tokeep'
	return true;
}

/** \brief parse the first half of the handshake
 * 
 * @return a 'tokeep/dontkeep' for the socket_full_t
 */
bool	bt_session_cnx_t::parse_first_half_handshake()	throw()
{	
	uint8_t		protid_len;
	std::string	protid_str;
	bt_protoflag_t	protoflag;
	bt_id_t		infohash;
	// sanity check - the first half of the remote_handshake MUST NOT be already parsed
	DBG_ASSERT( remote_handshake.infohash().is_null() );
	
	// parse the bt_handshake_t up to the infohash
	bytearray_t	data_copy	= recved_data;
	try {
		data_copy >> protid_len;
		if( data_copy.get_len() < (ssize_t)protid_len)	nthrow_serial_plain("protocolid Short");
		protid_str	= std::string((char*)data_copy.get_data(), protid_len);
		data_copy.head_remove( protid_len );
		data_copy >> protoflag;
		data_copy >> infohash;
	}catch(serial_except_t &e){
		// if the unserialization failed, it is assumed the data are not yet fully received
		return true;
	}
	// do consume the data from the receved_data
	recved_data	= data_copy;
	
	// try to find a bt_swarm_t matching this infohash
	bt_swarm_t *	bt_swarm;
	bt_swarm	= bt_session->swarm_by_infohash(infohash);
	// if no bt_swarm_t matches this infohash, autodelete
	if( !bt_swarm ){
		KLOG_INFO("received a bt_session_cnx_t for infohash " << infohash << " but no bt_swarm_t for it");
		return autodelete();
	}
	
	// send back the local handshake
	bytearray_t	local_handshake;
	local_handshake	<< bt_swarm->get_handshake();
	socket_full->send(local_handshake.to_datum());	
	
	// set the first half of the remote_handshake
	remote_handshake.protocolid	( protid_str );
	remote_handshake.protoflag	( protoflag );
	remote_handshake.infohash	( infohash );
	// return 'tokeep'
	return true;
}


/** \brief parse the second half of the handshake
 * 
 * @return a 'tokeep/dontkeep' for the socket_full_t
 */
bool	bt_session_cnx_t::parse_second_half_handshake()	throw()
{
	bt_id_t		peerid;
	// sanity check - the first half of the remote_handshake MUST be already parsed
	DBG_ASSERT( remote_handshake.infohash().is_null() == false );
	
	// if the recved_data is empty, autodelete the connection
	// - TODO it is only because the old bytearray_t on top of the new serial_t doesnt
	//   support to create empty serial. 
	//   - remove it when the new bytearray_t in ported
	//   - meanwhile this doesnt hurt anything
	if( recved_data.empty() )	return autodelete();
	
	// parse the bt_handshake_t up to the infohash
	bytearray_t	data_copy	= recved_data;
	try {
		data_copy >> peerid;
	}catch(serial_except_t &e){
		// if the unserialization failed, it is assumed the data are not yet fully received
		return true;
	}
	// do consume the data from the receved_data
	recved_data	= data_copy;

	// set the second half of the remote_handshake - it is now complete
	remote_handshake.peerid	( peerid );
	
	// try to find a bt_swarm_t matching this infohash
	bt_swarm_t *	bt_swarm	= bt_session->swarm_by_infohash(remote_handshake.infohash());
	// if no bt_swarm_t matches this infohash, autodelete
	// - this may happen if the bt_swarm_t have been delete between the first and second half parsing
	if( !bt_swarm )	return autodelete();
	
	
	// set the bt_session->is_tcp_inetreach if the socket is TCP and from a public address
	// - it allow to check if the bt_session IS actually inetreach via tcp
	if( socket_full->remote_addr().get_peerid().is_public() 
					&& socket_full->domain() == socket_domain_t::TCP ){
		bt_session->is_tcp_inetreach	= true;	
	}

	// if the remote peerid is the local peerid, autodelete - this avoid to connect myself
	if( bt_session->local_peerid() == remote_handshake.peerid() )		return autodelete();
	// if it is no more allowed to create a bt_swarm_full, autodelete
	if( bt_swarm->is_new_full_allowed() == false )				return autodelete();
	// if there is already a bt_swarm_full_t for this remote peerid, autodelete
	if( bt_swarm->full_by_remote_peerid(remote_handshake.peerid()) )	return autodelete();

	// backup the object_slotid of the socket_full_t - to be able to return its tokeep value
	slot_id_t	socket_full_slotid	= socket_full->get_object_slotid();
	// steal the socket_full_t and mark it unused
	socket_full_t *	socket_full_stolen	= socket_full;
	socket_full	= NULL;
	// spawn a bt_swarm_full_t
	bt_err_t	bt_err;
	bt_swarm_full_t*swarm_full;
	swarm_full	= nipmem_new bt_swarm_full_t();
	bt_err		= swarm_full->start(bt_swarm, socket_full_stolen, recved_data, remote_handshake);
	if( bt_err.failed() )	nipmem_delete swarm_full;

	// autodelete - as this bt_session_cnx_t is nomore needed
	nipmem_delete	this;
	// compute the socket_full_t tokeep from its slotid as it may and may not be delete here
	return object_slotid_tokeep(socket_full_slotid);
}

NEOIP_NAMESPACE_END





