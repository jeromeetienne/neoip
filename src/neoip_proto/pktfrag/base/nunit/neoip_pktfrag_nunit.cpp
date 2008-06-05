/*! \file
    \brief Definition of the unit test for the \ref pktfrag_t

*/

/* system include */
/* local include */
#include "neoip_pktfrag_nunit.hpp"
#include "neoip_pktfrag.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief General test
 */
nunit_res_t	pktfrag_testclass_t::fragreasm(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	pktfrag_t		pktfrag;
	pktfrag_profile_t	profile;
	pktfrag_err_t		pktfrag_err;
	// log to debug
	KLOG_DBG("enter");
	// setup the packet reassembly
	profile.outter_mtu( 32 ).pool_max_size( 1000 );
	pktfrag_err		= pktfrag.set_from_profile(profile);
	NUNIT_ASSERT( pktfrag_err.succeed() );
	

	// fill a datum with dummy data
	datum_t		datum(45);
	for( size_t i = 0; i < datum.get_len(); i++ )	((char *)datum.get_data())[i] = i;
	// build the full datagram
	pkt_t		orig_dgram( datum );

	// fragment the packet
	pktfrag_fraglist_t	fraglist;
	fraglist	= pktfrag.pkt_from_upper(orig_dgram);

	// some logging
	KLOG_DBG("fraglist=" << fraglist );
	
	// simulate a reassembly
	for( size_t i = 0; i < fraglist.size(); i++ ){
		pkt_t		recved_pkt;
		// set recved_pkt as if the fragment was from the network
		recved_pkt	= fraglist[i];
		// log to debug
		KLOG_DBG("recved_pkt=" << recved_pkt);
		// send it to the pktfrag_t
		pktfrag_err	= pktfrag.pkt_from_lower(recved_pkt);
		NUNIT_ASSERT( pktfrag_err.succeed() );
		
		// test if the recved_pkt is now containing the whole datagram
		if( recved_pkt.is_null() ){
			KLOG_DBG("no new dgram completed");
		}else{
			KLOG_DBG("new dgram completed =" << recved_pkt << " original data=" << datum );
			KLOG_DBG("are they equal=" << (recved_pkt.to_datum() == datum) );
			// check that the reassembled packet is equal to the original one
			NUNIT_ASSERT( recved_pkt.to_datum() == datum );
			// return OK
			return NUNIT_RES_OK;
		}
	}	
	// return error
	return NUNIT_RES_ERROR;
}

NEOIP_NAMESPACE_END

