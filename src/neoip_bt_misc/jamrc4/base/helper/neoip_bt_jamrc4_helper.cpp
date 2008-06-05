/*! \file
    \brief Definition of static helper functions for the tls layer

*/

/* system include */
/* local include */
#include "neoip_bt_jamrc4_helper.hpp"
#include "neoip_bt_id.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_skey_ciph.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return a skey_ciph_t based on the parameters
 */
skey_ciph_t *	bt_jamrc4_helper_t::build_xmit_ciph(const bt_id_t &itor_nonce
				, const bt_id_t &resp_nonce, bool is_itor)	throw()
{
	// compute xmit_key/recv_key
	datum_t shared_secret	= itor_nonce.to_datum() + resp_nonce.to_datum();
	datum_t	xmit_key_datum	= datum_t(is_itor?"i2r":"r2i") + shared_secret;
	datum_t	xmit_key	= bt_id_t(xmit_key_datum).to_datum();
	bool	keying_failed;
	
	// init m_xmit_ciph
	skey_ciph_t *	xmit_ciph;
	xmit_ciph	= nipmem_new skey_ciph_t("arcfour/stream");
	keying_failed	= xmit_ciph->init_key( xmit_key.void_ptr(), xmit_key.length() );
	if( keying_failed ){
		nipmem_zdelete	xmit_ciph;
		return NULL;
	}

	// discard 1024-byte on m_xmit_ciph
	// - as arcfour is known to have a very weak begining
	datum_t		discard_dummy(1024);
	skey_ciph_iv_t	ciph_iv( xmit_ciph );
	xmit_ciph->encipher(ciph_iv	, discard_dummy.void_ptr(), discard_dummy.length()
					, discard_dummy.void_ptr(), discard_dummy.length());
	// return the just-built xmit_ciph
	return xmit_ciph;
}

/** \brief return a skey_ciph_t based on the parameters
 */
skey_ciph_t *	bt_jamrc4_helper_t::build_recv_ciph(const bt_id_t &itor_nonce
				, const bt_id_t &resp_nonce, bool is_itor)	throw()
{
	// compute recv_key/recv_key
	datum_t shared_secret	= itor_nonce.to_datum() + resp_nonce.to_datum();
	datum_t	recv_key_datum	= datum_t(is_itor?"r2i":"i2r") + shared_secret;
	datum_t	recv_key	= bt_id_t(recv_key_datum).to_datum();
	bool	keying_failed;
	
	// init m_recv_ciph
	skey_ciph_t *	recv_ciph;
	recv_ciph	= nipmem_new skey_ciph_t("arcfour/stream");
	keying_failed	= recv_ciph->init_key( recv_key.void_ptr(), recv_key.length() );
	if( keying_failed ){
		nipmem_zdelete	recv_ciph;
		return NULL;
	}

	// discard 1024-byte on m_recv_ciph
	// - as arcfour is known to have a very weak begining
	datum_t		discard_dummy(1024);
	skey_ciph_iv_t	ciph_iv( recv_ciph );
	recv_ciph->encipher(ciph_iv	, discard_dummy.void_ptr(), discard_dummy.length()
					, discard_dummy.void_ptr(), discard_dummy.length());
	// return the just-built recv_ciph
	return recv_ciph;
}

NEOIP_NAMESPACE_END;






