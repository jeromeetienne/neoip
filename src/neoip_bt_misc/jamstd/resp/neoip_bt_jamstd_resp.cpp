/*! \file
    \brief Header of the \ref bt_jamstd_resp_t

*/

/* system include */
/* local include */
#include "neoip_bt_jamstd_resp.hpp"
#include "neoip_bt_jamstd_full.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_dh.hpp"
#include "neoip_skey_ciph.hpp"
#include "neoip_random_pool.hpp"
#include "neoip_rand.hpp"
#include "neoip_base_oswarp.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                               CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Default Constructor
 */
bt_jamstd_resp_t::bt_jamstd_resp_t()	throw()
{
}

/** \brief Destructor
 */
bt_jamstd_resp_t::~bt_jamstd_resp_t()				throw()
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
bt_jamstd_resp_t &bt_jamstd_resp_t::profile(const bt_jamstd_profile_t &p_profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( p_profile.check() == bt_err_t::OK );	
	// copy the parameter
	this->m_profile	= p_profile;
	// return the object iself
	return *this;
}

/** \brief Start the action
 */
bt_err_t bt_jamstd_resp_t::start(const bt_id_t &m_cnx_shsecret)	throw()
{
	// copy the parameter
	this->m_cnx_shsecret	= m_cnx_shsecret;
	
	// compute the dh_privkey
	dh_param_t	dh_param(m_profile.dh_group(), m_profile.dh_generator());
	m_dh_privkey	= dh_privkey_t( dh_param );

	// put the cnxesta packet
	m_xmit_buffer.append( build_cnxesta_pkt().to_datum(datum_t::NOCOPY) );
	// set m_state to bt_jamstd_resp_state_t::CNXESTA_WAIT
	m_state		= bt_jamstd_resp_state_t::CNXESTA_WAIT;
	
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief used by the caller to notify recved_pkt
 * 
 * @param jamstd_full_out	if returned as non-null, it contains a bt_jamstd_full_t
 *			and the connection is considered established
 * 
 * @return a bt_err_t. if bt_err.failed() then the connection should be dropped
 */
bt_err_t bt_jamstd_resp_t::notify_recved_data(const pkt_t &recved_pkt
					, bt_jamstd_full_t **jamstd_full_out)	throw()
{
	// set jamstd_full_out to NULL by default
	*jamstd_full_out	= NULL;
	
	// put the recved_pkt into the m_recv_buffer
	m_recv_buffer.append(recved_pkt.to_datum(datum_t::NOCOPY));

	// handle the parsing differently depending on the current m_state
	while( true ){
		bool		stop_parsing	= false;
		bt_err_t	bt_err;
		// parse the m_recv_buffer with the proper handler depending on m_state
		switch( m_state.value() ){
		case bt_jamstd_resp_state_t::CNXESTA_WAIT:{
				bt_err	= parse_in_cnxesta_wait(jamstd_full_out, &stop_parsing);
				if( bt_err.failed() )	return bt_err;
				break;}
		case bt_jamstd_resp_state_t::CNXAUTH_WAIT:{
				bt_err	= parse_in_cnxauth_wait(jamstd_full_out, &stop_parsing);
				if( bt_err.failed() )	return bt_err;
				break;}				
		case bt_jamstd_resp_state_t::PADAUTH_WAIT:{
				bt_err	= parse_in_padauth_wait(jamstd_full_out, &stop_parsing);
				if( bt_err.failed() )	return bt_err;
				break;}	
		case bt_jamstd_resp_state_t::INITPKT_WAIT:{
				bt_err	= parse_in_initpkt_wait(jamstd_full_out, &stop_parsing);
				if( bt_err.failed() )	return bt_err;
				break;}	
		default:	DBG_ASSERT(0);
		}
		// if stop_parsing has been set to true, leave the loop
		if( stop_parsing )	break;
	}
	
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			parsing function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse the remote data when in bt_jamstd_resp_state_t::CNXESTA_WAIT
 */
bt_err_t	bt_jamstd_resp_t::parse_in_cnxesta_wait(bt_jamstd_full_t **jamstd_full_out
							, bool *stop_parsing)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check - m_state MUST be bt_jamstd_resp_state_t::CNXESTA_WAIT
	DBG_ASSERT( m_state == bt_jamstd_resp_state_t::CNXESTA_WAIT );

	
	/*************** try to detect plainbt_signature	***************/ 
	datum_t	plainbt_signature("\x13""BitTorrent protocol");
	KLOG_DBG("m_Recv_buffer=" << m_recv_buffer);
	if( m_recv_buffer.size() >= plainbt_signature.size() ){
		if(m_recv_buffer.head_peek(plainbt_signature.size()) == plainbt_signature){
			*stop_parsing	= true;
			return bt_err_t(bt_err_t::ERROR, "PLAINBT SIGNATURE DETECTED");	
		}
	}
	
	// if m_recv_buffer doesnt contain the remote dh_pubkey_t, return now and stop parsing
	if( m_recv_buffer.size() < m_profile.dh_pubkey_len() ){
		*stop_parsing	= true;
		return bt_err_t::OK;
	}
	
	// extract the itor_dh_pubkey from m_recv_buffer
	datum_t	tmp	= m_recv_buffer.head_consume(m_profile.dh_pubkey_len());
	dh_pubkey_t	itor_dh_pubkey	= dh_pubkey_t::from_datum(tmp);

	// compute the dh_shsecret_t
	dh_param_t	dh_param(m_profile.dh_group(), m_profile.dh_generator());
	dh_shsecret_t	dh_shsecret( m_dh_privkey, itor_dh_pubkey, dh_param);

	// log to debug
	KLOG_DBG("itor_dh_pubkey="	<< itor_dh_pubkey);
	KLOG_DBG("dh_shsecret="		<< dh_shsecret);

	// compute xmit_key/recv_key
	datum_t	xmit_key= bt_id_t(datum_t("keyB") + dh_shsecret.to_datum() + m_cnx_shsecret.to_datum()).to_datum();
	datum_t	recv_key= bt_id_t(datum_t("keyA") + dh_shsecret.to_datum() + m_cnx_shsecret.to_datum()).to_datum();
	bool	keying_failed;
	// init m_xmit_ciph
	m_xmit_ciph	= nipmem_new skey_ciph_t("arcfour/stream");
	keying_failed	= m_xmit_ciph->init_key( xmit_key.void_ptr(), xmit_key.length() );
	if( keying_failed )	return bt_err_t(bt_err_t::ERROR, "unable to init_key for xmit_ciph");	
	// init m_recv_ciph
	m_recv_ciph	= nipmem_new skey_ciph_t("arcfour/stream");
	keying_failed	= m_recv_ciph->init_key( recv_key.void_ptr(), recv_key.length() );
	if( keying_failed )	return bt_err_t(bt_err_t::ERROR, "unable to init_key for recv_ciph");

	// discard 1024-byte on m_xmit_ciph/m_recv_ciph
	// - as arcfour is known to have a very weak begining
	datum_t		discard_dummy(1024);
	skey_ciph_iv_t	ciph_iv( m_xmit_ciph );
	m_xmit_ciph->encipher(ciph_iv	, discard_dummy.void_ptr(), discard_dummy.length()
					, discard_dummy.void_ptr(), discard_dummy.length());
	m_recv_ciph->decipher(ciph_iv	, discard_dummy.void_ptr(), discard_dummy.length()
					, discard_dummy.void_ptr(), discard_dummy.length());
	// log to debug
	KLOG_DBG("xmit_key=" << xmit_key);
	KLOG_DBG("recv_key=" << recv_key);

	// set m_cnxauth_mark to both hash output found at the begining of cnxauth packet 
	// - HASH('req1', dh_shsecret) | (HASH('req2', cnx_shsecret) ^ HASH('req3', dh_shsecret)
	// - NOTE: both are stored thus it avoid keeping dh_shsecret around
	bt_id_t	hash_req1(datum_t("req1") + dh_shsecret.to_datum());
	bt_id_t	hash_req2(datum_t("req2") + m_cnx_shsecret.to_datum());
	bt_id_t	hash_req3(datum_t("req3") + dh_shsecret.to_datum());
	m_cnxauth_mark	= hash_req1.to_datum() + (hash_req2 ^ hash_req3).to_datum();

	// log to debug
	KLOG_DBG("m_cnxauth_mark=" << m_cnxauth_mark);

	// set m_state to bt_jamstd_resp_state_t::CNXAUTH_WAIT
	m_state		= bt_jamstd_resp_state_t::CNXAUTH_WAIT;

	// return no error
	return bt_err_t::OK;
}


/** \brief Parse the remote data when in bt_jamstd_resp_state_t::CNXAUTH_WAIT
 */
bt_err_t	bt_jamstd_resp_t::parse_in_cnxauth_wait(bt_jamstd_full_t **jamstd_full_out
							, bool *stop_parsing)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check - m_state MUST be bt_jamstd_resp_state_t::CNXAUTH_WAIT
	DBG_ASSERT( m_state == bt_jamstd_resp_state_t::CNXAUTH_WAIT );

	// try to find the m_cnxauth_mark in m_recv_buffer
	void * mark_ptr	= base_oswarp_t::memmem(m_recv_buffer.void_ptr(), m_recv_buffer.length()
					, m_cnxauth_mark.void_ptr(), m_cnxauth_mark.length());
	// if m_cnxauth_mark is not found, return now and stop parsing
	if( mark_ptr == NULL ){
		*stop_parsing	= true;
		return bt_err_t::OK;
	}
	
	// compute the cnxesta_padlen
	size_t	cnxesta_padlen	= (char *)mark_ptr - m_recv_buffer.char_ptr();
	
	// compute the length of the cnxauth packet - all up to and including len(cnxauth_pad)
	size_t	cnxauth_minlen	= bt_id_t::size()		// for hash1
				+ bt_id_t::size()		// for hash23
				+ m_profile.verif_cst().size()	// for verification_cst
				+ sizeof(bt_jamstd_negoflag_t)	// for negoflag_proposed
				+ sizeof(uint16_t);		// for len(cnxauth_pad)
	
	// if m_recv_buffer didnt receive the whole cnxauth packet, return now and stop parsing
	if( m_recv_buffer.length() < cnxesta_padlen + cnxauth_minlen ){
		*stop_parsing	= true;
		return bt_err_t::OK;
	}
	
	
	// consume all the padding up to the m_cnxauth_mark
	m_recv_buffer.head_free(cnxesta_padlen);
	
	// sanity check - at this point, m_recv_buffer MUST be at least cnxauth_minlen long
	DBG_ASSERT( m_recv_buffer.length() >= cnxauth_minlen);

	// sanity check - m_recv_buffer MUST start with m_cnxauth_mark
	DBG_ASSERT( m_recv_buffer.head_peek(m_cnxauth_mark.length()) == m_cnxauth_mark );

	// consume the m_cnxauth_mark
	m_recv_buffer.head_free(m_cnxauth_mark.length());
		
	// TODO crappy way to write this shit :)
	pkt_t	pkt_ciph( m_recv_buffer.head_consume(
					  m_profile.verif_cst().size()	// for verif_cst
					+ sizeof(bt_jamstd_negoflag_t)	// for negoflag_proposed
					+ sizeof(uint16_t)		// for len(cnxauth_pad)
					));
	// decrypt pkt_ciph
	skey_ciph_iv_t	ciph_iv(m_recv_ciph);
	m_recv_ciph->decipher(ciph_iv	, pkt_ciph.void_ptr(), pkt_ciph.length()
					, pkt_ciph.void_ptr(), pkt_ciph.length());
	
	KLOG_DBG("pkt_ciph=" << pkt_ciph);
	// check the verifcst
	datum_t	theo_verifcst	= datum_t(m_profile.verif_cst());
	datum_t	recv_verifcst	= pkt_ciph.head_consume(theo_verifcst.size());
	if( recv_verifcst != theo_verifcst )	return bt_err_t(bt_err_t::ERROR, "verifcst doesnt match, aborting");
	
	// extract the negoflag_proposed by remote peer 
	bt_jamstd_negoflag_t	negoflag_proposed;
	pkt_ciph	>> negoflag_proposed;
	KLOG_DBG("negoflag_proposed=" << negoflag_proposed);
	// compute negoflag_selected from negoflag_proposed and m_profile.negoflag_allowed()
	bt_jamstd_negoflag_t	negoflag_common;
	negoflag_common		= negoflag_proposed & m_profile.negoflag_allowed();
	if( (negoflag_common & bt_jamstd_negoflag_t::ARCFOUR).value() ){
		m_negoflag_selected	= bt_jamstd_negoflag_t::ARCFOUR;
	}else if( (negoflag_common & bt_jamstd_negoflag_t::PLAINTEXT).value() ){
		m_negoflag_selected	= bt_jamstd_negoflag_t::PLAINTEXT;
	}else{
		return bt_err_t(bt_err_t::ERROR, "no matching negoflag, aborting");
	}
	KLOG_DBG("m_negoflag_selected=" << m_negoflag_selected);
	
	// set m_cnxauth_padlen
	pkt_ciph	>> m_cnxauth_padlen;
	// log to debug
	KLOG_DBG("m_cnxauth_padlen=" << m_cnxauth_padlen);
	
	// sanity check - at this point, pkt_ciph MUST be empty
	DBG_ASSERT( pkt_ciph.length() == 0 );

	// put the cnxauth packet in m_xmit_buffer
	pkt_t	cnxauth_pkt	= build_cnxauth_pkt();
	m_xmit_buffer.append( cnxauth_pkt.to_datum(datum_t::NOCOPY) );

	// set m_state to bt_jamstd_resp_state_t::PADAUTH_WAIT
	m_state		= bt_jamstd_resp_state_t::PADAUTH_WAIT;
	
	// return no error
	return bt_err_t::OK;
}


/** \brief Parse the remote data when in bt_jamstd_resp_state_t::PADAUTH_WAIT
 */
bt_err_t	bt_jamstd_resp_t::parse_in_padauth_wait(bt_jamstd_full_t **jamstd_full_out
							, bool *stop_parsing)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check - m_state MUST be bt_jamstd_resp_state_t::PADAUTH_WAIT
	DBG_ASSERT( m_state == bt_jamstd_resp_state_t::PADAUTH_WAIT );
	
	// if m_recv_buffer didnt receive the whole cnxauth padding + len(initpkt), return now
	if( m_recv_buffer.length() < m_cnxauth_padlen + sizeof(uint16_t)){
		*stop_parsing	= true;
		return bt_err_t::OK;
	}
	
	
	// extract the pkt to decipher - m_cnxauth_padlen + uint16_t for the m_initpkt_totlen
	pkt_t	pkt_ciph( m_recv_buffer.head_consume(m_cnxauth_padlen + sizeof(uint16_t)) );
	// decrypt pkt_ciph
	skey_ciph_iv_t	ciph_iv(m_recv_ciph);
	m_recv_ciph->decipher(ciph_iv	, pkt_ciph.void_ptr(), pkt_ciph.length()
					, pkt_ciph.void_ptr(), pkt_ciph.length());

	// consume all the padding up to the len(initpkt)
	pkt_ciph.head_free(m_cnxauth_padlen);
	// zero m_cnxauth_padlen 
	m_cnxauth_padlen	= 0;

	// set m_initpkt_totlen from pkt_ciph
	pkt_ciph	>> m_initpkt_totlen;
	KLOG_DBG("m_initpkt_totlen="	<< m_initpkt_totlen);

	// set m_state to bt_jamstd_resp_state_t::INITPKT_WAIT
	m_state		= bt_jamstd_resp_state_t::INITPKT_WAIT;
	
	// return no error
	return bt_err_t::OK;
}

/** \brief Parse the remote data when in bt_jamstd_resp_state_t::INITPKT_WAIT
 */
bt_err_t	bt_jamstd_resp_t::parse_in_initpkt_wait(bt_jamstd_full_t **jamstd_full_out
							, bool *stop_parsing)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check - m_state MUST be bt_jamstd_resp_state_t::INITPKT_WAIT
	DBG_ASSERT( m_state == bt_jamstd_resp_state_t::INITPKT_WAIT );

	// if m_recv_buffer didnt receive the whole initpkt, return now and stop parsing
	if( m_recv_buffer.length() < m_initpkt_totlen ){
		*stop_parsing	= true;
		return bt_err_t::OK;
	}

	// extract and decipher the initpkt 
	// - NOTE: it must be done separatly from the rest of m_recv_buffer as
	//   initpkt is *always* encrypted, while the rest may not be depending
	//   on m_negoflag_selected
	pkt_t	pkt_ciph( m_recv_buffer.head_consume(m_initpkt_totlen) );
	skey_ciph_iv_t	ciph_iv(m_recv_ciph);
	m_recv_ciph->decipher(ciph_iv	, pkt_ciph.void_ptr(), pkt_ciph.length()
					, pkt_ciph.void_ptr(), pkt_ciph.length());
	KLOG_DBG("initpkt=" << pkt_ciph);
	
	
	// establish the skey_ciph_t to pass to bt_jamstd_full_t depending on m_negoflag_selected
	skey_ciph_t *	recv_ciph_full	= NULL;
	skey_ciph_t *	xmit_ciph_full	= NULL;
	if( m_negoflag_selected == bt_jamstd_negoflag_t::ARCFOUR ){
		// decrypt the remaining of m_recv_buffer inplace
		skey_ciph_iv_t	ciph_iv( m_recv_ciph );
		m_recv_ciph->decipher(ciph_iv	, m_recv_buffer.void_ptr(), m_recv_buffer.length()
						, m_recv_buffer.void_ptr(), m_recv_buffer.length());
		// change the ownership of m_recv_ciph/m_xmit_ciph to bt_jamstd_full_t
		recv_ciph_full	= m_recv_ciph;
		xmit_ciph_full	= m_xmit_ciph;
		m_recv_ciph	= NULL;
		m_xmit_ciph	= NULL;
	}else if( m_negoflag_selected == bt_jamstd_negoflag_t::PLAINTEXT ){
		// as it is PLAINTEXT, pass no skey_ciph_t to bt_jamstd_full_t
		recv_ciph_full	= NULL;
		xmit_ciph_full	= NULL;
	}else{	DBG_ASSERT( 0 );	}

	// TODO break this .prepend is ugly. clean this up
	m_recv_buffer.prepend(pkt_ciph.to_datum(datum_t::NOCOPY));

	// spawn a bt_jamstd_full_t and copy it to *jamstd_full_out
	bt_jamstd_full_t*	jamstd_full;
	jamstd_full	= nipmem_new bt_jamstd_full_t();
	bt_err_t bt_err	= jamstd_full->start(recv_ciph_full, xmit_ciph_full, m_recv_buffer);
	if( bt_err.failed() ){
		nipmem_zdelete	jamstd_full;
		return bt_err;
	}
	*jamstd_full_out	= jamstd_full;

	// stop parsing as the connection is now considered established
	*stop_parsing	= true;
	
	// return no error
	return bt_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Build packet
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build the pkt_esta
 */
pkt_t	bt_jamstd_resp_t::build_cnxesta_pkt()	throw()
{
	datum_t	dh_pubkey	= m_dh_privkey.get_pubkey().to_datum();
	pkt_t	pkt;
	// sanity check - m_state MUST be bt_jamstd_resp_state_t::NONE
	DBG_ASSERT( m_state == bt_jamstd_resp_state_t::NONE );
	
	// sanity check - the dh public key MUST be equal to m_profile.dh_pubkey_len()
	DBG_ASSERT( m_dh_privkey.get_pubkey().to_datum().length() == m_profile.dh_pubkey_len()); 

	// append the dh public key to the packet
	// - NOTE: no serialization because the length of the datum_t must not be included
	pkt.append( m_dh_privkey.get_pubkey().to_datum() );

	// log to debug
	KLOG_DBG("resp_dh_pubkey="	<< m_dh_privkey.get_pubkey());
	
	// allocate the padding_data and fill them with weak random  
	size_t	padding_len	= (size_t)neoip_rand(0, m_profile.padesta_maxlen());
	datum_t	padding_data(padding_len);
	random_pool_t::read_weak(padding_data.void_ptr(), padding_data.length());	
	// append the padding data
	pkt.append( padding_data );
	
	// return the just-built packet
	return pkt;
}

/** \brief Build the pkt_auth
 */
pkt_t	bt_jamstd_resp_t::build_cnxauth_pkt()	throw()
{
	pkt_t	pkt;
	// sanity check - m_state MUST be bt_jamstd_resp_state_t::CNXAUTH_WAIT
	DBG_ASSERT( m_state == bt_jamstd_resp_state_t::CNXAUTH_WAIT );
	// sanity check - m_negoflag_selected MUST NOT be 0
	DBG_ASSERT( m_negoflag_selected.value() != 0 );

	/*************** build pkt_ciph before encrypting it	***************/
	pkt_t	pkt_ciph;
	// put the verification_cst
	pkt_ciph.append(datum_t(m_profile.verif_cst()));
	// put m_negoflag_selected
	pkt_ciph	<< m_negoflag_selected;
	// allocate the padding_data and without filling them
	size_t	padding_len	= (size_t)neoip_rand(0, m_profile.padauth_maxlen());
	datum_t	padding_data	= datum_t(padding_len);
	memset(padding_data.char_ptr(), 0x99, padding_data.length());
	// append the padding data
	pkt_ciph	<< uint16_t(padding_len);
	pkt_ciph.append( padding_data );
	// log to debug
	KLOG_DBG("padding_len=" << padding_len);


	/*************** encrypt pkt_ciph and append it	***********************/	
	// encrypt pkt_ciph
	skey_ciph_iv_t	ciph_iv( m_xmit_ciph );
	m_xmit_ciph->encipher(ciph_iv	, pkt_ciph.void_ptr(), pkt_ciph.length()
					, pkt_ciph.void_ptr(), pkt_ciph.length());
	// append the encrypted pkt_ciph
	pkt.append( pkt_ciph.to_datum(datum_t::NOCOPY));

	// return the just-built packet
	return pkt;
}

NEOIP_NAMESPACE_END




