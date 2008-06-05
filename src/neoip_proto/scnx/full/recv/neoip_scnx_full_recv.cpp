/*! \file
    \brief Definition of \ref scnx_full_recv_t

*/

/* system include */
/* local include */
#include "neoip_scnx_full_recv.hpp"
#include "neoip_skey_pkcs5_derivkey.hpp"
#include "neoip_assert.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"


NEOIP_NAMESPACE_BEGIN;

// definition of \ref scnx_full_recv_t constant
/** \brief The number of packet accepted by the antireplay window.
 * 
 * - The necessary width depends on the possible packet reordering by the underlying
 *   network. 
 */
const size_t	scnx_full_recv_t::AREPLAY_WIN_SIZE_DFL	= 32;
// end of constants definition

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
scnx_full_recv_t::scnx_full_recv_t(scnx_full_t *scnx_full, const skey_ciph_type_t &ciph_type
				, const skey_auth_type_t &auth_type
				, const datum_t &base_key)	throw()
				: scnx_full(scnx_full), skey_ciph(ciph_type)
				, ciph_iv(&skey_ciph), skey_auth(auth_type)
{
	datum_t	base_salt;
	// sanity check - the base_key datum MUST be secure memory
	DBG_ASSERT( base_key.is_secmem() );

// INIT the AUTH_KEY
	// allocate secure memory for the auth_key
	auth_key	= datum_t(skey_auth.get_key_len(), datum_t::SECMEM);
	// build the key 
	base_salt = datum_t("scnxauth");
	skey_pkcs5_derivkey( "sha1/nokey/20", auth_key, base_key, base_salt, 1 );
	DBG_ASSERT( skey_auth.get_key_len() < 20 );

// INIT the CIPH_KEY
	// allocate secure memory for the ciph_key
	datum_t	ciph_key= datum_t(skey_ciph.get_key_len(), datum_t::SECMEM);
	// derive the key
	base_salt = datum_t("scnxciph");
	skey_pkcs5_derivkey( "sha1/nokey/20", ciph_key, base_key, base_salt, 1 );
	DBG_ASSERT( skey_ciph.get_key_len() < 20 );
	// init the skey_ciph key
	skey_ciph.init_key(ciph_key);

	// log to debug
	KLOG_DBG("base_key="  << base_key );
	KLOG_DBG("skey_auth=" << skey_auth << " auth_key=" << auth_key);
	KLOG_DBG("skey_ciph=" << skey_ciph << " ciph_key=" << ciph_key);

	// open the anti replay window
	areplay_window	= nipmem_new slidwin_t<bool, uint32_t>(AREPLAY_WIN_SIZE_DFL, false);
}

/** \brief Destructor
 */
scnx_full_recv_t::~scnx_full_recv_t()					throw()
{
	// delete the anti replay window
	nipmem_delete	areplay_window;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   PROCESS PACKET
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief process the packet
 * 
 * @param	pkt the packet to process, it will contained the processed packet if no error occured
 * @return false if no error occured, true otherwise
 */
scnx_err_t	scnx_full_recv_t::pkt_from_lower( pkt_t &pkt )	throw()
{
	uint32_t	seqnb;
	datum_t		remote_mac;
// check the MAC
	// back up the remote mac
	// - dont consume it until it is verified as if the recv fails with the 
	//   main receive key, the packet must be left intact to try the alternate
	//   receive key.
	try {
		remote_mac	= pkt.tail_peek(skey_auth.get_output_len());
	}catch(serial_except_t &e){
		return scnx_err_t(scnx_err_t::BOGUS_PKT, "Cant read the FULL_AUTH due to " + e.what());
	}
	// recompute the mac of the remote packet
	skey_auth.init_key(auth_key);
	skey_auth.update( pkt.get_data(), pkt.get_len() - skey_auth.get_output_len() );
	datum_t generated_mac	= skey_auth.get_output();
	// check if both are equal
	if( remote_mac != generated_mac ){
		KLOG_ERR("Received incoming packet containing invalid MAC");
		return scnx_err_t::BAD_FULL_AUTH;
	}
	// now consume the mac
	pkt.tail_remove(skey_auth.get_output_len());
	// NOTE: here the MAC of the incoming packet is considered valid
	
// check the antireplay
	try {
		pkt >> seqnb;
	}catch(serial_except_t &e){
		return scnx_err_t(scnx_err_t::BOGUS_PKT, "Cant read the sequence number due to " + e.what());
	}

	// check if the incoming sequence number is acceptable by the anti-replay
	// - aka is it past the oldest packet in the window, it is assumed already received
	// - aka is it inside the current window and is equal to true, it has been already received
	// - if it is newest that the last element of the window, it has never been received
	if( seqnb < areplay_window->get_index_first() || areplay_window->get_default(seqnb, false) )
		return scnx_err_t::REPLAYED_PKT;
	// mark the seqnb as accepted
	areplay_window->set(seqnb, true);
	// NOTE: here the antireplay accepted the packet

// decrypt the incoming packet
	// setup the iv
	ciph_iv.set_iv_data( &seqnb, sizeof(seqnb) );
	// allocate memory for the plain text
	size_t		plaintxt_len		= pkt.get_len();
	void *		plaintxt_ptr		= nipmem_alloca(plaintxt_len);
	// decipher the data
	plaintxt_len = skey_ciph.decipher(ciph_iv, pkt.get_data(), pkt.get_len()
							, plaintxt_ptr, plaintxt_len);
	// check if the decipher returned an error
	if( plaintxt_len < 0 ){
		KLOG_ERR("failed to decipher a incoming packet");
		return scnx_err_t::BAD_FULL_CIPH;
	}

// build the outgoing packet
	pkt = pkt_t( plaintxt_ptr, plaintxt_len );	
	
	// log to debug
	KLOG_DBG("end packet is " << pkt );
	// return no error
	return scnx_err_t::OK;
}



NEOIP_NAMESPACE_END

