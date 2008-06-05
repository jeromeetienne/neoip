/*! \file
    \brief Definition of the \ref kad_db_t

*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_kad_db.hpp"
#include "neoip_kad_db_rec.hpp"
#include "neoip_kad_db_replicate.hpp"
#include "neoip_kad_db_republish.hpp"
#include "neoip_kad_peer.hpp"
#include "neoip_kad_recdups.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN
	
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
kad_db_t::kad_db_t(kad_peer_t *kad_peer)	throw()
{
	// copy the parameter
	this->kad_peer		= kad_peer;
	this->db_replicate	= nipmem_new kad_db_replicate_t(this);
	this->db_republish	= nipmem_new kad_db_republish_t(this);
	// zero some field
	db_size			= 0;
}

/** \brief Constructor by default
 */
kad_db_t::~kad_db_t()	throw()
{
	// delete kad_db_replicate_t if needed
	nipmem_zdelete	db_replicate;
	// delete kad_db_republish_t if needed
	nipmem_zdelete	db_republish;
	// delete all records
	while( !distid_db.empty() )	nipmem_delete distid_db.begin()->second;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   function to link rec_t to this database
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Do link a kad_db_t::rec_t to this kad_db_t
 */
void	kad_db_t::rec_dolink(rec_t *db_rec)	throw()
{
	// update the database_size
	db_size		+= db_rec->get_payload().get_len();

	// link it to the key-distance database
	kad_keyid_t	distid	= get_distid(db_rec->get_keyid());
	distid_db.insert(std::make_pair(distid, db_rec));

	// link it to the recid database
	bool	succeed	= recid_db.insert(std::make_pair(db_rec->get_recid(), db_rec)).second;
	DBG_ASSERT(succeed);
}

/** \brief unlink a kad_db_t::rec_t to this kad_db_t
 */
void	kad_db_t::rec_unlink(rec_t *db_rec)	throw()
{
	// update the database_size
	DBG_ASSERT( db_size >= db_rec->get_payload().get_len() );
	db_size		-= db_rec->get_payload().get_len();
	// remove this record from the recid database
	recid_db.erase( db_rec->get_recid() );
	// remove this record from the key-distance database
	distid_db.erase( get_iter_from_ptr(db_rec) );
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   UTILITY function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true if this kad_db_t is the remote_db in the kad_peer_t, false otherwise
 */
bool	kad_db_t::is_remote_db()	const throw()
{
	if( kad_peer->get_remote_db() == this )	return true;
	if( kad_peer->get_local_db() == this )	return false;
	// NOTE: this point MUST never be reached
	DBG_ASSERT( 0 );
	return false;
}

/** \brief Return the kad_db_profile_t of this kad_db_t
 * 
 * - the profile is specific to its local/remote status
 */
const kad_db_profile_t &kad_db_t::get_profile()	const throw()
{
	if( is_remote_db() )	return kad_peer->get_profile().remote_db();
	return kad_peer->get_profile().local_db();
}


/** \brief return the distance between the local peerid and this id
 * 
 * - it helps while determining if a remote node is closer to a record id
 *   than the local node. see notify_new_node() 
 */
kad_targetid_t	kad_db_t::get_distid(const kad_targetid_t &targetid)	const throw()
{
	return kad_peer->local_peerid() ^ targetid;
}

/** \brief return an iterator on the map element which contains this pointer as data
 * 
 * - usefull on multimap as the usual stl's find() only find based on key and not data
 * - usefull to delete a particular record and not all the ones with the same key
 */
kad_db_t::distid_db_t::iterator kad_db_t::get_iter_from_ptr(kad_db_t::rec_t *rec_ptr)
										throw()
{
	// compute the key in the map
	kad_targetid_t		map_key	= get_distid(rec_ptr->get_keyid());
	// get the first element >= than the map_key, or .end() if none exist
	distid_db_t::iterator iter	= distid_db.lower_bound(map_key);

	// sanity check - the element MUST be in the database
	DBG_ASSERT( iter != distid_db.end() );
	DBG_ASSERT( iter->first == map_key );

	// scan all the elements with the same keys
	for( ; iter != distid_db.end() && iter->first == map_key; iter++ ){
		// test if this element is the searched one, if so return it
		if( iter->second == rec_ptr )
			return iter;
	}

	// NOTE: this point MUST NOT be reached (as the element is supposed to be in the database)
	DBG_ASSERT(0);
	return distid_db.end();	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   DATABASE handling by public functions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/** \brief return true if the record is acceptable by the database
 */
bool	kad_db_t::may_accept(const kad_rec_t &kad_rec)	const throw()
{
	const kad_db_profile_t &profile	= get_profile();
	size_t			rec_len	= kad_rec.get_payload().get_len();

	// if no duplicate kad_keyid_t are allowed and the record keyid is already contained, reject it
	if( profile.keyid_nodups_ok() && contain(kad_rec.get_keyid()) )		return false;
	// if the record ttl is greater then the maximum, reject it
// TODO this check on ttl seems like quite too much for me, maybe it should just be clamped
// - a lot about this about the republishing timer
	if( kad_rec.get_ttl() > profile.rec_max_ttl() )				return false;
	// if the record length is greater then the maximum, reject it	
	if( profile.rec_max_len() && rec_len > profile.rec_max_len() )		return false;	
	// if this record will increase the database limit beyond its maximum, reject it
	if( profile.db_max_len() && db_size + rec_len > profile.db_max_len() )	return false;
	// if all tests passed, this record is allowed
	return true;
}

/** \brief return true if the kad_recdups is acceptable by the database
 */
bool	kad_db_t::may_accept(const kad_recdups_t &kad_recdups)	const throw()
{
	const kad_db_profile_t &profile	= get_profile();
	// compute the size of the whole kad_recdups_t
	size_t	recdups_len	= 0;
	for(size_t i = 0; i < kad_recdups.size(); i++ )
		recdups_len	+= kad_recdups[i].get_payload().get_len();
	// if this kad_recdups_t will increase the database size limit beyond its maximum, reject it
	if( profile.db_max_len() && db_size + recdups_len > profile.db_max_len() )
		return false;
		
	// check all the kad_rec_t individually
	for(size_t i = 0; i < kad_recdups.size(); i++ ){
		// if this particular kad_recdups_t is not acceptable, return false now
		if( !may_accept(kad_recdups[i]) )
			return false;
	}
	// if all tests passed, this record is allowed
	return true;
}


/** \brief update a record in the database
 * 
 * - if this record recid is already in the database, the 'worst' one is removed
 */
void	kad_db_t::update(const kad_rec_t &kad_rec)	throw()
{
	// sanity check - here the kad_rec MUST be allowed
	DBG_ASSERT( may_accept(kad_rec) );
	// try to find a record with the same recid
	recid_db_t::iterator	iter = recid_db.find(kad_rec.get_recid());
 	// if there is a record with the same recid, do a tie check
	if( iter != recid_db.end() ){
	 	// if the new record to insert has a shorted ttl than the existing one, discard the new
		if( iter->second->get_kad_rec().is_tie_winner_against(kad_rec) )	return;
		// else delete the old	
		nipmem_delete iter->second;
	}

	// NOTE: in 2nd paper, section 2.5
	// "First, when a node receives a store RPC for a given key-value pair, it assumes
	//  the RPC was also issued to the other k − 1 closest nodes, and thus the recipient
	//  will not republish the key-value pair in the next hour. This ensures that as long
	//  as republication intervals are not exactly synchronized, only one node will
	//  republish a given key-value pair every hour."

	// create the rec_t, it links itself to the database
	nipmem_new rec_t(this, kad_rec);
}


/** \brief update the whole kad_recdups_t in the database
 * 
 * - Just a helper which call update(kad_rec_t) on each record
 */
void	kad_db_t::update(const kad_recdups_t &kad_recdups)	throw()
{
	for(size_t i = 0; i < kad_recdups.size(); i++ )
		update(kad_recdups[i]);
}

/** \brief remove the record matching this recid from the database
 * 
 * - it is ok if the record id is NOT in the database
 */
void	kad_db_t::remove(const kad_recid_t &recid)	throw()
{
	recid_db_t::iterator	iter = recid_db.find(recid);
	// if the recid is not in the database, exit not
	if( iter == recid_db.end() )	return;
	// delete this record - it remove itself from the database
	nipmem_delete iter->second;
}

/** \brief return true if this database contains records with this kad_recid_t
 */
bool	kad_db_t::contain(const kad_recid_t &recid)		const throw()
{
	recid_db_t::const_iterator	iter = recid_db.find(recid);
	if( iter == recid_db.end() )	return false;
	return true;
}

/** \brief return true if this database contains records with this kad_keyid_t
 */
bool	kad_db_t::contain(const kad_keyid_t &keyid)		const throw()
{
	distid_db_t::const_iterator	iter = distid_db.find(get_distid(keyid));
	if( iter == distid_db.end() )	return false;
	return true;		
}

/** \brief return the kad_recdups_t of all records matching this keyid
 * 
 * - if the kad_recdups_t.size() == 0 then no record matches
 * 
 * @param keyid		the keyid that all returns records MUST have
 * @param recid_gt	all returneds records MUST have a recid greater than this value
 *			if is_null() it is ignored
 */
kad_recdups_t	kad_db_t::find_by_keyid(const kad_keyid_t &keyid, const kad_recid_t &recid_gt)	const throw()
{	
	kad_recdups_t	result;
	// compute the key in the map
	kad_keyid_t	map_key	= get_distid(keyid);
	// get the first element >= than the map_key, or .end() if none exist
	distid_db_t::const_iterator	iter	= distid_db.lower_bound(map_key);	
	// scan all the elements with the proper key
	for( ; iter != distid_db.end() && iter->first == map_key; iter++ ){
		const rec_t *	rec_ptr	= iter->second;
		// if its recid is NOT greater than recid_gt, skip it
		if( !recid_gt.is_null() && rec_ptr->get_recid() <= recid_gt )	continue;
		// add this record in the result
		result.update(rec_ptr->get_kad_rec());	
	}
	// return the result
	return result;
}

/** \brief return a keyid which has records and is greater than the one in parameter
 */
kad_keyid_t	kad_db_t::get_next_keyid(const kad_keyid_t &keyid)	const throw()
{
	distid_db_t::const_iterator	iter;
	// compute the key in the map
	kad_keyid_t	map_key	= get_distid(keyid);
	// get the first element > than the map_key, or .end() if none exist
	iter	= distid_db.upper_bound(map_key);
	// if no greater keyid exists, return a null kad_keyid_t
	if( iter == distid_db.end() )	return kad_keyid_t();
	// return the keyid of this element
	return iter->second->get_keyid();
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                           NOTIFY_new_NODE
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief To be called when the local node observe a new nodes
 * 
 * - this function sends a STORE to this node for each record which is closer
 *   to it than the local one.
 */
void	kad_db_t::notify_new_node(const kad_addr_t &kad_addr)	throw()
{
	// - from the paper, "Finally, in order to sustain consistency in the publishing-searching
	//   life-cycle of a key,value pair, we require that whenever a node w observes a new node
	//   u which is closer to some of w’s (key,value) pairs, w replicates these pairs to u without
	//   removing them from its own database."
	
	// compute the middle distance between the local peer and the remote one
	kad_targetid_t	node_dist	= kad_peer->local_peerid() ^ kad_addr.get_peerid();
	kad_targetid_t	mid_dist	= node_dist >> 1;
	// get the first element >= than the mid_dist, or .end() if none exist
	distid_db_t::const_iterator iter= distid_db.lower_bound(mid_dist);	
	// scan all the elements from the middle distance to the end - as they are closer to the 
	// new peer than the local peer.
	for( ; iter != distid_db.end(); iter++ ){
		rec_t &		rec	= *iter->second;
		// Start the STORE for this record
		kad_err_t	kad_err = db_replicate->replicate_in(rec.get_kad_rec(), kad_addr);
		// if the start failed, log the event
		if( kad_err.failed() )
			KLOG_INFO("Failed to replicate a record " << rec.get_kad_rec() << " to " << kad_addr);
	}
}

/** \brief helper forwarding the function to kad_db_replicate_t
 */
kad_err_t kad_db_t::replicate_in(const kad_recdups_t &kad_recdups, const kad_addr_t &remote_addr) throw()
{
	return db_replicate->replicate_in(kad_recdups, remote_addr);
}



NEOIP_NAMESPACE_END;






