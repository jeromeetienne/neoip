/*! \file
    \brief Class to handle the bt_peersrc_nslan_t

\par Brief Description
\ref bt_peersrc_nslan_t is a bt_peersrc_vapi_t implemenation publishing and 
querying other bt peer on a nslan_peer_t. The published record contains the 
bt_session_t::listen_ipport_lview. This allows to find other peer on the 
same LAN even if the public view is unknown or if the LAN is behind a
non-loopback NAT.
- this is possible because nslan_query_t provides the source ipport_addr_t of
the replier. thus a destination address to reach the replier on this LAN. 

\par About the absence of nb_seeder/nb_leecher
- to handle them requires to determine when the nslan_rec_src_t has a full round
  in order to zero the counter
- issue is that nslan_rec_src_t provide a never ending list
- so it is easy to increment the nb_seeder/nb_leecher when parsing
  the received nslan_rec_t
- but as the same nslan_rec_t will be received over and over, the 
  counter will be incremented for ever...
- it would require to modify the nslan_rec_src_t in order to do a single query
  and then complete a round.
  - but doing that will likely remove the advantage of reacting immediatly 
    when another peer publish a record as it wont have the 'live query' feature.
      
*/

/* system include */
/* local include */
#include "neoip_bt_peersrc_nslan.hpp"
#include "neoip_bt_peersrc_event.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_session.hpp"
#include "neoip_nslan.hpp"
#include "neoip_nslan_publish.hpp"
#include "neoip_nslan_rec_src.hpp"
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
bt_peersrc_nslan_t::bt_peersrc_nslan_t()		throw()
{
	// zero some fields
	bt_swarm	= NULL;
	callback	= NULL;
	m_nb_seeder	= 0;
	m_nb_leecher	= 0;
	nslan_publish	= NULL;
	nslan_rec_src	= NULL;
}

/** \brief Destructor
 */
bt_peersrc_nslan_t::~bt_peersrc_nslan_t()		throw()
{
	// unregister this object
	// - NOTE: here the notified callback is not the owner, so it is not allowed to return donkeep
	if( callback ){
		bool	tokeep	= notify_callback( bt_peersrc_event_t::build_unregister() );
		DBG_ASSERT( tokeep );
	}
	// delete nslan_publish_t if needed
	nipmem_zdelete	nslan_publish;
	// delete the nslan_rec_src if needed
	nipmem_zdelete	nslan_rec_src;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_peersrc_nslan_t::start(bt_swarm_t *bt_swarm, nslan_peer_t *nslan_peer
					, bt_peersrc_cb_t *callback, void *userptr)	throw()
{
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	// copy the parameter
	this->bt_swarm	= bt_swarm;
	this->nslan_peer= nslan_peer;
	this->callback	= callback;
	this->userptr	= userptr;

	// register this peersrc to the callback
	// - NOTE: here the notified callback is not the owner, so it is not allowed to return donkeep
	bool	tokeep	= notify_callback( bt_peersrc_event_t::build_doregister() );
	DBG_ASSERT( tokeep );

	// publish the peer record
	bt_err_t	bt_err;
	bt_err		= publish_peer_record();
	if( bt_err.failed() )	return bt_err;

	// Start the nslan_rec_src_t
	inet_err_t	inet_err;
	nslan_keyid_t	nslan_keyid	= bt_mfile.infohash().to_canonical_string();
	nslan_rec_src	= nipmem_new nslan_rec_src_t();
	inet_err	= nslan_rec_src->start(nslan_peer, nslan_keyid, this, NULL);
	if( inet_err.failed() )	return bt_err_from_inet(inet_err);

	// request one record from nslan_rec_src_t
	nslan_rec_src->get_more();
	
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      handle the ntudp_peer_rec_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief publish the peer record 
 */
bt_err_t	bt_peersrc_nslan_t::publish_peer_record()				throw()
{
	bt_session_t *		bt_session	= bt_swarm->get_session();
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	// if the peer record was already published, delete the nslan_publish
	nipmem_zdelete	nslan_publish;
	
	// build the nslan_rec_t payload
	bytearray_t	payload;
	payload	<< bt_session->local_peerid();
	payload << bt_swarm->is_seed();
	payload << bt_session->listen_lview();
	// TODO change that to get a bt_peersrc_peer_t directly
	// - bt_swarm->to_peersrc_peer_lview()
	
	// build the peer record
	nslan_keyid_t	keyid		= bt_mfile.infohash().to_canonical_string();
	nslan_rec_t	nslan_rec	= nslan_rec_t(keyid, payload.to_datum(), delay_t::from_sec(60));
	// log to debug
	KLOG_DBG("keyid=" << keyid << " payload=" << payload);

	// publish the just-built nslan_rec_t
	nslan_publish	= nipmem_new nslan_publish_t(nslan_peer, nslan_rec);

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			nslan_rec_src_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref nslan_rec_src_t when to notify an ipport_addr_t
 */
bool	bt_peersrc_nslan_t::neoip_nslan_rec_src_cb(void *cb_userptr, nslan_rec_src_t &cb_nslan_rec_src
		, const nslan_rec_t &nslan_rec, const ipport_addr_t &publisher_ipport)	throw()
{
	// log to debug
	KLOG_DBG("enter notified record=" << nslan_rec << " publisher_ipport=" << publisher_ipport);

	// sanity check - the nslan_rec_t MUST never be null as the nslan_rec_src_t is a never-ending src
	DBG_ASSERT( !nslan_rec.is_null() );
	
	// handle this received nslan_rec_t
	handle_recved_nslan_rec(nslan_rec, publisher_ipport);
	
	// request one more nslan_rec_t to the nslan_rec_src_t
	nslan_rec_src->get_more();

	// return 'tokeep'
	return true;
}

/** \brief Handle the received nslan_recdups
 */
void	bt_peersrc_nslan_t::handle_recved_nslan_rec(const nslan_rec_t &nslan_rec
				, const ipport_addr_t &publisher_ipport)	throw()
{
	bytearray_t	payload( nslan_rec.get_payload() );
	bt_id_t		remote_peerid;
	ipport_addr_t	remote_listen_ipport;
	bool		is_seeder;
	// log to debug
	KLOG_DBG("enter");
	// parse the incoming packet
	try{
		// TODO change that to get a bt_peersrc_peer_t directly
		payload	>> remote_peerid;
		payload >> is_seeder;
		payload	>> remote_listen_ipport;
	}catch(serial_except_t &e){
		// log the event
		KLOG_ERR("Can't parse peer record " << nslan_rec << " due to " << e.what() );
		return;
	}
	// log to debug
	KLOG_DBG("remote_peerid=" << remote_peerid << " remote_listen_ipport=" << remote_listen_ipport
						<< " is_seeder=" << is_seeder);

	// if remote_listen_ipport.get_ipaddr() is any, replace it by the published_ipport.get_ipaddr()
	// - NOTE: this allow to create a fully_qualified destination ipport_addr_t on the LAN
	//   even when the remote peer is listening on ANY ipv4 address
	if( remote_listen_ipport.get_ipaddr().is_any() )
		remote_listen_ipport	= ipport_addr_t(publisher_ipport.get_ipaddr(), remote_listen_ipport.get_port() );
	// sanity check - now the remote_listen_ipport MUST be fully_qualified
	DBG_ASSERT( remote_listen_ipport.is_fully_qualified() );

	// build the bt_tracker_peer_t from the record payload
	socket_addr_t		remote_addr	= std::string("stcp://") + remote_listen_ipport.to_string();
	bt_peersrc_peer_t	peersrc_peer(remote_peerid, remote_addr);

	// log to debug
	KLOG_DBG("notify peersrc_peer=" << peersrc_peer);

	// notify the caller
	bool	tokeep	= notify_callback(bt_peersrc_event_t::build_new_peer(&peersrc_peer));
	if( !tokeep )	return;		
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the tcp_event
 */
bool bt_peersrc_nslan_t::notify_callback(const bt_peersrc_event_t &event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_peersrc_cb(userptr, *this, event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





