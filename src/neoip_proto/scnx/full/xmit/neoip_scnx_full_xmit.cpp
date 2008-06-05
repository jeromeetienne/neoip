/*! \file
    \brief Definition of \ref scnx_full_xmit_t

*/

/* system include */
/* local include */
#include "neoip_scnx_full_xmit.hpp"
#include "neoip_scnx_full.hpp"
#include "neoip_skey_pkcs5_derivkey.hpp"
#include "neoip_assert.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
scnx_full_xmit_t::scnx_full_xmit_t(scnx_full_t *scnx_full, const skey_ciph_type_t &ciph_type
				, const skey_auth_type_t &auth_type
				, const datum_t &base_key)	throw()
				: scnx_full(scnx_full), skey_ciph(ciph_type), ciph_iv(&skey_ciph)
				, skey_auth(auth_type), seqnb(0)
{
	datum_t		base_salt;
	// sanity check - the base_key datum MUST be secure memory
	DBG_ASSERT( base_key.is_secmem() );
	
// INIT the AUTH_KEY
	// allocate secure memory for the auth_key
	auth_key	= datum_t( skey_auth.get_key_len(), datum_t::SECMEM );
	// build the key 
	base_salt = datum_t("scnxauth");
	skey_pkcs5_derivkey( "sha1/nokey/20", auth_key, base_key, base_salt, 1 );
	DBG_ASSERT( skey_auth.get_key_len() < 20 );

// INIT the CIPH_KEY
	// allocate secure memory for the ciph_key
	datum_t	ciph_key	= datum_t( skey_ciph.get_key_len(), datum_t::SECMEM );
	// derive the key
	base_salt = datum_t("scnxciph");
	skey_pkcs5_derivkey( "sha1/nokey/20", ciph_key, base_key, base_salt, 1 );
	DBG_ASSERT( skey_ciph.get_key_len() < 20 );
	// init the skey_ciph key
	skey_ciph.init_key( ciph_key );

	// log to debug
	KLOG_DBG("base_key="  << base_key );
	KLOG_DBG("skey_auth=" << skey_auth << " auth_key=" << auth_key);
	KLOG_DBG("skey_ciph=" << skey_ciph << " ciph_key=" << ciph_key);
}

/** \brief Destructor
 */
scnx_full_xmit_t::~scnx_full_xmit_t()					throw()
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                           Query function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return the space overhead for data packet
 */
size_t	scnx_full_xmit_t::get_mtu_overhead()		const throw()
{
	size_t		overhead = 0;
	// get the seqnb overhead
	serial_t	serial;
	serial << seqnb;
	overhead += serial.get_len();
	// get the authentication overhead
	overhead += skey_auth.get_output_len();
	// get the encryption overhead
	// - it includes no iv, as it is in the seqnb
	// - the encryption overhead changes depending on the plaintext size due to the padding
	// - the overhead length depends on the padding policy and format
	// - because of this, if the encryption mode is padded, the mtu must assume the maximum length
	//   wasting space
	// => padded mode waste even more space (beyond the padding itself)
	overhead += skey_ciph.get_ciphertxt_len(1) - 1;
	// return the just computed mtu overhead
	return overhead;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                            PROCESS PACKET
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief process the packet
 * 
 * @param	pkt the packet to process, it will contained the processed packet if no error occured
 * @return a scnx_err_t
 */
scnx_err_t	scnx_full_xmit_t::pkt_from_upper( pkt_t &pkt )	throw()
{
	// setup the iv based on the seqnb
	ciph_iv.set_iv_data( &seqnb, sizeof(seqnb) );
	// encipher the data
	size_t		ciphertxt_len	= skey_ciph.get_ciphertxt_len(pkt.get_len());
	void *		ciphertxt_ptr	= nipmem_alloca(ciphertxt_len);	
	skey_ciph.encipher(ciph_iv, pkt.get_data(), pkt.get_len(), ciphertxt_ptr, ciphertxt_len);
	
	// build the packet
	pkt	= pkt_t();
	// put the sequence number first
	pkt	<< seqnb;
	// put the encrypted data 
	pkt.append( ciphertxt_ptr, ciphertxt_len );
	// compute the authentication over the sequence number and the encrypted data
	skey_auth.init_key( auth_key );
	skey_auth.update( pkt.to_datum(datum_t::NOCOPY) );
	// append the authentication output
	pkt.append( skey_auth.get_output() );

	// increment the anti-replay sequence number
	seqnb++;

	// return no error
	return scnx_err_t::OK;
}


NEOIP_NAMESPACE_END

