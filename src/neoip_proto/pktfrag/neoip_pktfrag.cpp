/*! \file
    \brief Definition of the \ref pktfrag_t

\par Possible Improvement
- the format on the wire is quite crude, you may revamp it to improve it
  - e.g. the more_bit is stored on a 8-bit
  - e.g. the fragment offset and sequence number are 32-bit
  => along with the packettype, it is 10-byte of overhead per fragment
  - how to reduce the overhead
    - the more bit could be the lowest bit of the fragment offset
      - this would gain 8-bit
      - or even simplier, to have another packettype for the last fragment
    - i dont want to put the fragment offset in 16-bit as it would limit the 
      size of the neoip dgram to 64k
    - the sequence number could be 16-bit .... unclear
      - ok it is uint and not a wai_t, so only the collision needs to be taken care of
      - but if there are more than 64k fragmented dgram in the air... this could reach
        a DEADLOCK... aka no more correct
    - NOTE: this is only to reduce the overhead of fragmentation
      - which is mostly avoided at the ip datagram level => not important as it is rare
      - which may be frequent at the neoip dgram level but would only be for large packets... 
        makeing the overhead negligible.
- may add a notification in case of packet expiration in the reasm pool
  - it would help tune the rasm pool size

\par TODO 
- the POOL_MAX_SIZE_DFL largely depends on the dgram size
  - if the size of the fragmented is larger than the pool size the dgram has no way to 
    be reassembled
  - it is related to the neoip dgram size, see pktcomp for a similar issue

*/


/* system include */
/* local include */
#include "neoip_pktfrag.hpp"
#include "neoip_pktfrag_pending_dgram.hpp"
#include "neoip_pktfrag_pkttype.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref pktfrag_t constant
#if 1
	const size_t	pktfrag_t::OUTTER_MTU_DFL		= 1400;
	const size_t	pktfrag_t::POOL_MAX_SIZE_DFL		= 30*1024;
	const delay_t	pktfrag_t::PENDING_DGRAM_EXPIRE_DFL	= delay_t::from_sec(60);
#else
	// value for debug
	const size_t	pktfrag_t::OUTTER_MTU_DFL		= 40;
	const size_t	pktfrag_t::POOL_MAX_SIZE_DFL		= 30*1024;
	const delay_t	pktfrag_t::PENDING_DGRAM_EXPIRE_DFL	= delay_t::from_sec(60);
#endif
// end of constants definition


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                         ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
pktfrag_t::pktfrag_t()	throw()
{
	// set delault values
	outter_mtu		= pktfrag_t::OUTTER_MTU_DFL;
	pending_dgram_expire	= pktfrag_t::PENDING_DGRAM_EXPIRE_DFL;
	pool_max_size		= pktfrag_t::POOL_MAX_SIZE_DFL;

	// set initialization values
	pool_cur_size		= 0;
	next_xmit_seqnb		= 0;
}

/** \brief Destructor
 */
pktfrag_t::~pktfrag_t()	throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         dolink/unlink in pending_dgram_db
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Dolink a pending_dgram_t to this pktfrag_t
 */
void 	pktfrag_t::pending_dgram_dolink(pending_dgram_t *pending_dgram) 	throw()
{
	// add this pending_dgram_t to the pending_dgram_db
	bool    succeed = pending_dgram_db.insert(std::make_pair(pending_dgram->get_seqnb(), pending_dgram)).second;
	DBG_ASSERT( succeed );	
}

/** \brief Unlink a pending_dgram_t to this pktfrag_t
 */
void 	pktfrag_t::pending_dgram_unlink(pending_dgram_t *pending_dgram) 	throw()
{
	// sanity check the pending_dgram MUST be in the database
	DBG_ASSERT( pending_dgram_db.find( pending_dgram->get_seqnb() ) != pending_dgram_db.end() );
	// remote this pending_dgram from the database
	pending_dgram_db.erase( pending_dgram->get_seqnb() );
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    pool_size function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief make room in the pool (in order to insert a new fragment)
 */
void	pktfrag_t::pool_drain(size_t needed_len)		throw()
{
	// remove pending_dgram_t until (i) needed_len is available OR (ii) it is empty 
	while( pool_cur_size && (pool_max_size-pool_cur_size) < needed_len ){
		pending_dgram_db_t::iterator	iter;
		// sanity check - the database MUST NOT be empty
		DBG_ASSERT( !pending_dgram_db.empty() );
		// get the first of the database
		iter = pending_dgram_db.begin();
		// remove this pending_dgram from the database
		nipmem_delete iter->second;
	}
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                   setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief set the outter mtu to use in this pktfrag_t
 */
pktfrag_err_t	pktfrag_t::set_outter_mtu(size_t new_outter_mtu)	throw()
{
	// copy the parameter
	outter_mtu	= new_outter_mtu;
	// TODO in which case there is an error
	// - if the outter_mtu is too small to contain an header ?
	// return noerror
	return pktfrag_err_t::OK;
}

/** \brief set the parameters
 */
pktfrag_err_t pktfrag_t::set_from_profile(const pktfrag_profile_t &profile)	throw()
{
	// check the profile
	DBG_ASSERT( profile.check().succeed() );
	// copy the values
	outter_mtu		= profile.outter_mtu();
	pool_max_size		= profile.pool_max_size();
	pending_dgram_expire	= profile.pending_dgram_expire();
	// return no error
	return pktfrag_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       fragmentation
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief fragment a packet (with a automatically generated sequence number)
 */
pktfrag_fraglist_t	pktfrag_t::pkt_from_upper(pkt_t &orig_pkt)	throw()
{
	// update the next_xmit_seqnb to include the new packet
	next_xmit_seqnb++;
	return pkt_from_upper(orig_pkt, next_xmit_seqnb);
}

/** \brief fragment a packet (with a special sequence number)
 * 
 * @param seqnb	the sequence number to use for the each fragments
 * 		- This allows the caller to specify the seqnb used for each fragments
 * 		  of this packet. Thus it this packet has already been sent and is
 *                retransmitted, the receiver may rebuild the original packet from fragments
 *                received from the first sent and the following ones.
 *              - This improve the efficiency of the fragmentation/reassembly process
 */
pktfrag_fraglist_t	pktfrag_t::pkt_from_upper(pkt_t &orig_pkt, uint32_t seqnb)	throw()
{
	pktfrag_fraglist_t	fraglist;
	uint32_t		frag_offset;
	pkt_t			frag_pkt;
	size_t			frag_len;
	// compute the maximum amount of data that a single fragment may contains
	size_t			payl_mtu	= outter_mtu - cpu_fragment_hd_len();
	// compute the amount of data contained in every fragment but the last
	// int	frag_base_len	= orig_pkt.get_len() / nb_frag;	// to have all fragment of same size
	int			frag_base_len	= payl_mtu;
	
	// sanity check - the packet len MUST be larger than the MTU
	DBG_ASSERT( orig_pkt.get_len() > outter_mtu );

	// some logging
	KLOG_DBG("pkt_len="	<< orig_pkt.get_len() );
	KLOG_DBG("outter_mtu="	<< outter_mtu );
	KLOG_DBG("payl_mtu="	<< payl_mtu );
	KLOG_DBG("frag_baselen="<< frag_base_len );
	KLOG_DBG("seqnb="	<< seqnb );
	
	// loop over each fragment
	for( frag_offset = 0; orig_pkt.get_len(); frag_offset += frag_len ){
		bool	more_bit;
		// reset the fragment variable
		frag_pkt = pkt_t();
		// compute the amount of data that this fragment will contain
		if( orig_pkt.get_len() > payl_mtu ){
			more_bit	= true;
			frag_len	= frag_base_len;
		}else{
			more_bit	= false;
			frag_len	= orig_pkt.get_len();
		}
		// build the fragment header
		frag_pkt << pktfrag_pkttype_t(pktfrag_pkttype_t::PKT_FRAGMENT);
		frag_pkt << seqnb;
		frag_pkt << frag_offset;
		frag_pkt << (uint8_t) more_bit;
		// log to debug
		KLOG_DBG("frag_offset=" << frag_offset );
		KLOG_DBG("frag_len=" << frag_len );
		// add the data from the original packet to the fragment one
		frag_pkt.tail_add(orig_pkt.head_consume(frag_len));
		// add a fragment in the fraglist
		fraglist.add_fragment(frag_pkt);
	}
	// set the sequence number of this fragmented datagram in the fraglist
	fraglist.set_seqnb( seqnb );
	// log to debug
	KLOG_DBG("fraglist from the pkt_from_upper() = " << fraglist );
	// return the fraglist
	return fraglist;
}

/** \brief return the length of the fragmentation header
 */
size_t	pktfrag_t::cpu_fragment_hd_len()	const throw()
{
	serial_t	serial;
	// build a fake header
	serial << pktfrag_pkttype_t(pktfrag_pkttype_t::PKT_FRAGMENT);
	serial << next_xmit_seqnb;
	serial << uint32_t(0);
	serial << uint8_t(0);
	// return its length
	return serial.get_len();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       reassembly
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief handle packet from the lower network layer
 */
pktfrag_err_t	pktfrag_t::pkt_from_lower(pkt_t &pkt)		throw()
{
	pktfrag_pkttype_t	pkttype;

	try {	// read the pkttype
		pkt >> pkttype;
	}catch(serial_except_t &e){
		return pktfrag_err_t(pktfrag_err_t::BOGUS_PKT, "Cant read the packet type due to " + e.what());
	}

	// some logging
	KLOG_DBG("received a pkttype=" << pkttype );

	// parse the packet according to its type
	switch( pkttype.get_value() ){
	case pktfrag_pkttype_t::PKT_FRAGMENT:	return	recv_pkt_fragment(pkt);
	default:				break;
	}
	return pktfrag_err_t(pktfrag_err_t::BOGUS_PKT, "Unknown packet type");	
}

/** \brief handle the reception of a DATA_PKT
 */
pktfrag_err_t	pktfrag_t::recv_pkt_fragment(pkt_t &pkt)		throw()
{
	uint32_t	recved_seqnb;
	uint32_t	recved_offset;
	uint8_t		recved_more_bit;
	// read the fragment header
	try {
		pkt >> recved_seqnb;
		pkt >> recved_offset;
		pkt >> recved_more_bit;
	}catch(serial_except_t &e){
		return pktfrag_err_t(pktfrag_err_t::BOGUS_PKT, "Cant read incoming packet seqnb due to " + e.what());
	}
	// log to debug
	KLOG_DBG("recved_seqnb=" << recved_seqnb );
	KLOG_DBG("recved_offset=" << recved_offset );
	KLOG_DBG("fragment len=" << pkt.get_len() );
	KLOG_DBG("recved_more_bit=" << (bool)recved_more_bit );

	// handle the pool size if it is limited
	if( pool_size_is_limited() ){
		// make room in the pool
		pool_drain(pkt.get_len());
		// if the fragment is too large to be stored in the pool, discard it
		if( pool_max_size-pool_cur_size < pkt.get_len() ){
			KLOG_ERR("Received a single fragment larger than the whole pool. discarding it.");
			return pktfrag_err_t::FRAGMENT_TOO_LARGE;
		}
	}
	
	// try to find a pending_dgram_t with a seqnb equal to recved_seqnb
	pending_dgram_db_t::iterator	iter	= pending_dgram_db.find(recved_seqnb);
	pending_dgram_t *		pending_dgram;
	// if the pending datagram for this seuquence number doesnt exists, create it
	if( iter == pending_dgram_db.end() )
		pending_dgram	= nipmem_new pending_dgram_t(this, recved_seqnb);
	else
		pending_dgram	= iter->second;

	// add the received fragment to it and test if the datagram is now completed
	if( pending_dgram->add_fragment(recved_offset, (bool)recved_more_bit, pkt) ){
		// copy the complete datagram in the pkt parameter
		pkt	= pending_dgram->get_complete_dgram();
		// delete this pending dgram
		nipmem_delete pending_dgram;
	}else{
		// nullify the pkt to show that there is no output packet
		pkt	= pkt_t();
	}
	// return no error
	return pktfrag_err_t::OK;
}

NEOIP_NAMESPACE_END






