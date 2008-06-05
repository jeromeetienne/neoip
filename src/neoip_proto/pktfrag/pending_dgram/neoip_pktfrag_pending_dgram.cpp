/*! \file
    \brief Definition of the \ref pktfrag_t::pending_dgram_t

*/


/* system include */
/* local include */
#include "neoip_pktfrag_pending_dgram.hpp"
#include "neoip_pktfrag.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//	            ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
pktfrag_t::pending_dgram_t::pending_dgram_t(pktfrag_t *pktfrag, uint32_t seqnb)	throw()
{
	// set local variables
	this->seqnb		= seqnb;
	this->pktfrag		= pktfrag;
	this->full_dgram_len	= 0;
	// start the expiration timeout
	expire_timeout.start(pktfrag->pending_dgram_expire, this, NULL);
	// link this object to the pktfrag_t
	pktfrag->pending_dgram_dolink(this);
}

/** \brief Destructor
 */
pktfrag_t::pending_dgram_t::~pending_dgram_t()		throw()
{
	// unlink this object from the pktfrag_t
	pktfrag->pending_dgram_unlink(this);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         add a fragment
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief add a fragment in this pending_dgram_t
 *
 * - TODO this is unsecure for bogus input! do it
 *   - it is unclear what i should do...
 *   - if the reasm is done from unsecured packets... what can be done ?
 *   - if the reasm is done from secured packets... it would mean a bug
 * 
 * @return true if the datagram is now complete, false otherwise
 */
bool	pktfrag_t::pending_dgram_t::add_fragment(uint32_t offset, bool more_bit, const pkt_t &fragment)
										throw()
{
	// sanity check - the datagram MUST not be already complete
	DBG_ASSERT( is_complete() == false );
	// if this offset has already been received, discard it
	if( fragment_db.find(offset) != fragment_db.end() )	return false;
	// add this fragment to the fragment_db
	bool    succeed = fragment_db.insert(std::make_pair(offset, fragment)).second;
	DBG_ASSERT( succeed );

	// if this fragment is the last of the datagram, compute the dgram_length
	if( more_bit == false )		full_dgram_len	= offset + fragment.get_len();
	
	// return true if the datagram is now complete, false otherwise
	return is_complete();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   is_complete/get_complete
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true if the pending datagram is complete
 */
bool	pktfrag_t::pending_dgram_t::is_complete()				const throw()
{
	size_t	offset = 0;
	// if the full datagram length is still unknown, the last fragment hasn't been yet received
	// => the datagram isnt yet completed
	if( full_dgram_len == 0 )	return false;
	
	// scan the fragments to see if some are missing
	while( offset < full_dgram_len ){
		std::map<uint32_t, pkt_t>::const_iterator	iter;
		// try to find a fragment for this offset
		iter = fragment_db.find(offset);	
		// if this offset isnt yet received, the datagram isnt complete
		if( iter == fragment_db.end() )	return false;
		// shift the offset to the next fragment
		offset += iter->second.get_len();
	}
	return true;
}

/** \brief return the complete datagram
 */
pkt_t	pktfrag_t::pending_dgram_t::get_complete_dgram()			const throw()
{
	std::map<uint32_t, pkt_t>::const_iterator	iter;
	pkt_t						full_dgram;
	// sanity check - the pending_dgram_t MUST be completed
	DBG_ASSERT( is_complete() );
	// build the complete datagram
	for( iter = fragment_db.begin(); iter != fragment_db.end(); iter++ ){
		const pkt_t	&fragment = iter->second;
		full_dgram.append(fragment.get_data(), fragment.get_len());
	}
	return full_dgram;	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   expire_timeout callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	pktfrag_t::pending_dgram_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// some logging
	KLOG_DBG("expiration of pending datagram senqb=" << seqnb);
	// autodelete
	nipmem_delete this;
	// warn the timeout_t that it has been deleted
	return false;
}


NEOIP_NAMESPACE_END






