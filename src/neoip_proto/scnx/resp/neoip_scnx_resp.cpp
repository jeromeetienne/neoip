/*! \file
    \brief Header of the \ref scnx_resp_t

*/

/* system include */
/* local include */
#include "neoip_scnx_resp.hpp"
#include "neoip_scnx_full.hpp"
#include "neoip_scnx_pkttype.hpp"
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
scnx_resp_t::scnx_resp_t()	throw()
{
	scnx_profile		= NULL;
}
/** \brief Constructor
 */
scnx_resp_t::scnx_resp_t(scnx_profile_t *scnx_profile) throw()
{
	// zero some parameter
	auxnego_cb		= NULL;
	// store the pointer on scnx_profile_t
	this->scnx_profile	= scnx_profile;
	// set the default for reliable_outter_cnx
	reliable_outter_cnx	= false;	
	// generate the private key
	dh_privkey		= dh_privkey_t(scnx_profile->dh_param());
	// generate the auth_payload
	auth_payload		= generate_auth_payload();
}

/** \brief Destructor
 */
scnx_resp_t::~scnx_resp_t()		throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief define if the outter connection is reliable or not
 */
scnx_resp_t & scnx_resp_t::set_reliable_outter_cnx( bool value )		throw()
{
	// copy the parameter
	reliable_outter_cnx	= value;
	// return the object itself
	return *this;
}

/** \brief set the auxnego callback
 */
scnx_resp_t &scnx_resp_t::set_auxnego_cb(scnx_resp_auxnego_cb_t *callback, void *userptr)	throw()
{
	// copy the parameter
	this->auxnego_cb	= callback;
	this->auxnego_userptr	= userptr;
	// return the object itself
	return *this;
}

/** \brief Start the action
 */
scnx_err_t	scnx_resp_t::start()		throw()
{
	return scnx_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         Query function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief return true if the object is null
 */
bool	scnx_resp_t::is_null()				const throw()
{
	if( scnx_profile )	return false;
	return true;
}

/** \brief return true if the outter connection is reliable, false otherwise
 * 
 * - in the scnx_resp_t, this is used only to forward the state to \ref scnx_full_t
 */
bool	scnx_resp_t::is_reliable_outter_cnx()			const throw()
{
	return reliable_outter_cnx;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       auth_payload generation + precomputation
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief generate the auth_payload
 * 
 * - it is precomputed to prevent resource depletion ala syn-flood attack but CPU oriented
 */
datum_t	scnx_resp_t::generate_auth_payload()		throw()
{
	bytearray_t	bytearray;
	datum_t		signature;
	// serialize the auth_r 
	bytearray << scnx_profile->ident_cert();
	// serialize the diffie-hellman public key
	bytearray << dh_privkey.get_pubkey();
	// compute the signature
	signature = scnx_profile->ident_privkey().sign_data( bytearray.to_datum(datum_t::NOCOPY) );
	// serialize the signature
	bytearray << signature;
	// return the auth_payload
	return bytearray.to_datum();
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                             pkt from lower
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief handle packet from the lower network layer
 */
scnx_err_t scnx_resp_t::pkt_from_lower(pkt_t &pkt, const datum_t &cookie_pathid, scnx_event_t &scnx_event)
										throw()
{
	// logging
	KLOG_DBG("enter pkt=" << pkt);
	// sanity check - here the object MUST NOT be null
	DBG_ASSERT( !is_null() );
	// zero the scnx_event_t
	scnx_event	= scnx_event_t();
	
	// parse the incoming packet
	try {
		scnx_pkttype_t	pkttype;
		// read the pkttype (without consuming)
		pkt.unserial_peek( pkttype );
		// log to debug
		KLOG_DBG("received a pkttype=" << std::hex << pkttype );
		
		// handle the packet according to the pkttype
		switch( pkttype.get_value() ){
		case scnx_pkttype_t::CNX_REQUEST:	
				return recv_cnx_request(pkt, cookie_pathid, scnx_event);
		case scnx_pkttype_t::ITOR_AUTH:
				return recv_itor_auth(pkt, cookie_pathid, scnx_event);
		default:	break;
		}
	}catch(serial_except_t &e){
		return scnx_err_t(scnx_err_t::BOGUS_PKT, "Cant parse packet due to " + e.what());
	}
	// NOTE: if this point is reached, the pkttype is unknown		
	return scnx_err_t(scnx_err_t::BOGUS_PKT, "Unknown packet type");
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet RECV
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief handle the reception of a CNX_REQUEST
 */
scnx_err_t scnx_resp_t::recv_cnx_request(pkt_t &pkt, const datum_t &cookie_pathid
					, scnx_event_t &scnx_event)		throw(serial_except_t)
{
	scnx_pkttype_t	pkttype;
	cookie_id_t	cookie_id;
	scnx_nonceid_t	remote_nonce_i;
	// read the pkttype
	pkt >> pkttype;
	// read the incoming nonce
	pkt >> remote_nonce_i;
	// generate the cookie
	cookie_id	= cookie.generate(cookie_pathid + remote_nonce_i.to_datum());
	// build the CNX_REPLY
	pkt		= build_cnx_reply( remote_nonce_i, cookie_id );
	// return noerror
	return scnx_err_t::OK;
}

/** \brief handle the reception of a ITOR_AUTH
 */
scnx_err_t scnx_resp_t::recv_itor_auth(pkt_t &pkt, const datum_t &cookie_pathid, scnx_event_t &scnx_event)
										throw(serial_except_t)
{
	scnx_nonceid_t		remote_nonce_i;
	cookie_id_t		remote_cookie;
	x509_cert_t		remote_cert;
	dh_pubkey_t		remote_dh_pubkey;
	skey_auth_type_arr_t	remote_auth_optlist;
	skey_ciph_type_arr_t	remote_ciph_optlist;
	datum_t			auxnego_payl;
	void *			auxnego_ptr = NULL;
	datum_t			remote_signature;
	scnx_pkttype_t		pkttype;
	skey_auth_t		skey_auth("sha1/hmac/20");
	datum_t			orig_pkt_datum	= pkt.to_datum();
	const scnx_auth_ftor_t&	scnx_auth_ftor	= scnx_profile->scnx_auth_ftor();
	
	// set a mark of where the pop coverage start
	size_t			pop_cover_start	= orig_pkt_datum.get_len() - pkt.get_len();

	// read the pkttype
	pkt >> pkttype;

	// read the incoming nonce
	pkt >> remote_nonce_i;
	// read the remote_cookie
	pkt >> remote_cookie;

	// if the received cookie is not valid, return an error
	if( cookie.check(cookie_pathid + remote_nonce_i.to_datum(), remote_cookie ) == false )
		return scnx_err_t::BAD_COOKIE;

// get and check the certificate
	// set a mark of where the signature coverage start
	size_t	sign_cover_start= orig_pkt_datum.get_len() - pkt.get_len();
	// get the remote certificate
	pkt >> remote_cert;

	// call the scnx_auth_ftor to determine if this remote certificate is allowed or not
	scnx_err_t	scnx_err;
	scnx_err	= scnx_auth_ftor( remote_cert );
	// if this certificate is not allowed, return now
	if( scnx_err.failed() )	return scnx_err;

	
// get the remote diffie-hellman public key
	pkt >> remote_dh_pubkey;

// check the remote_signature
	// set a mark of where the signature coverage end
	size_t	sign_cover_end	= orig_pkt_datum.get_len() - pkt.get_len();
	// get the signed data (aka remote_cert + dh_pubkey)
	datum_t sign_cover_data	= orig_pkt_datum.get_range(sign_cover_start
								, sign_cover_end - sign_cover_start
								, datum_t::NOCOPY);
	// get the remote signature
	pkt >> remote_signature;
	// check if the signature is valid
	if( remote_cert.verify_data(sign_cover_data, remote_signature).failed() ){
		KLOG_ERR("Received a itor_auth with an invalid signature.");
		return scnx_err_t::BAD_SIGNATURE;
	}
	KLOG_DBG("signature OK");
	
// get the auth and ciph option list
	pkt >> remote_auth_optlist;
	pkt >> remote_ciph_optlist;

// get the auxnego_payl
	pkt >> auxnego_payl;	

// compute the diffie-hellman shared secret
	dh_shsecret_t	dh_shsecret( dh_privkey, remote_dh_pubkey, scnx_profile->dh_param() );
	
// compute the master symmetric key	
	skey_auth.init_key( remote_nonce_i.to_datum() + remote_cookie.to_datum() );
	skey_auth.update( dh_shsecret.get_shsecret_datum() );
	datum_t	master_key = skey_auth.get_output();
	// log to debug
	KLOG_DBG("enter master_key=" << master_key);

	
// check the pop proof-of-possession
	// set a mark of where the pop coverage end
	size_t	pop_cover_end	= orig_pkt_datum.get_len() - pkt.get_len();
	// get the covered data (aka the whole packet)
	datum_t	pop_cover_data 	= orig_pkt_datum.get_range(pop_cover_start, pop_cover_end-pop_cover_start
								, datum_t::NOCOPY);
	// compute the proof of possession
	skey_auth.init_key( master_key );
	skey_auth.update( pop_cover_data );
	// get the remote_pop
	datum_t	remote_pop	= pkt.tail_consume(skey_auth.get_output_len());
	// check if the received pop is equal to the generated one
	if( skey_auth.get_output() != remote_pop ){
		KLOG_ERR("Received a itor_auth with an invalid pop.");
		return scnx_err_t::BAD_POP;
	}
	// NOTE: if this point is reached, the security is ok

// negociate the auth/type based on the local and remote option list
	const skey_auth_type_arr_t &	skey_auth_optlist = scnx_profile->skey_auth_optlist();
	const skey_ciph_type_arr_t &	skey_ciph_optlist = scnx_profile->skey_ciph_optlist();
	// if there are no match for the negociation return AUTH_NEGO_FAILED
	if( !skey_auth_optlist.find_first_common(remote_auth_optlist) )
		return scnx_err_t::AUTH_NEGO_FAILED;
	if( !skey_ciph_optlist.find_first_common(remote_ciph_optlist) )
		return scnx_err_t::AUTH_NEGO_FAILED;
	// choose the negociated type
	skey_auth_type_t chosen_auth_type = *skey_auth_optlist.find_first_common(remote_auth_optlist);
	skey_ciph_type_t chosen_ciph_type = *skey_ciph_optlist.find_first_common(remote_ciph_optlist);
	// log to debug
	KLOG_DBG("skey_auth_optlist=" << skey_auth_optlist << " chosen=" << chosen_auth_type);
	KLOG_DBG("skey_ciph_optlist=" << skey_ciph_optlist << " chosen=" << chosen_ciph_type);

	// handle the auxilary negociation
	if( auxnego_cb ){
		scnx_err_t	err;
		err = auxnego_cb->neoip_scnx_resp_auxnego_cb(auxnego_userptr, *this
							, auxnego_payl, &auxnego_ptr);
		if( !err.succeed() )	return err;
		// NOTE: after this point, auxnego_ptr contained a nipmem_new buffer 
		// which is of an unknown type. so it MUST be reported in order to be freed
		// => MUST be done last and no other check should occurs after that
		// - TODO maybe it is replacable by a datum and thus avoid this risk
		//   of having a nipmem_new buffer in the air
	}else{
		auxnego_payl	= datum_t();
	}

	// put the cookie in the accepted database
	cookie.accept(cookie_pathid + remote_nonce_i.to_datum(), remote_cookie );


	// logging
	KLOG_DBG("CONNECTION ESTABLISHED with auth_type=" << chosen_auth_type 
					<< " ciph_type=" << chosen_ciph_type
					<< " master_key=" << master_key );

	// build a RESP_AUTH
	pkt = build_resp_auth(remote_nonce_i, master_key, auxnego_payl
					, chosen_auth_type, chosen_ciph_type);

	// log to debug
	KLOG_DBG("remote subject name=" << remote_cert.get_subject_name());
	KLOG_DBG("remote issuer name=" << remote_cert.get_issuer_name());	

	// build the CNX_ESTABLISHED event with the scnx_full_t issued from this connection
	scnx_full_t *	scnx_full;
	scnx_full	= nipmem_new scnx_full_t(master_key, chosen_ciph_type, chosen_auth_type
					, false, is_reliable_outter_cnx()
					, scnx_profile->ident_cert().get_subject_name()
					, remote_cert.get_subject_name()
					, pkt_t(orig_pkt_datum), pkt);
	scnx_event	= scnx_event_t::build_cnx_established(scnx_full, auxnego_ptr);

	return scnx_err_t::OK;
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet BUILD
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief build a CNX_REPLY packet
 */
pkt_t 	scnx_resp_t::build_cnx_reply(const scnx_nonceid_t &remote_nonce_i, const cookie_id_t &cookie_id)
										throw()
{
	pkt_t	pkt;
	// add the packet type
	pkt << scnx_pkttype_t(scnx_pkttype_t::CNX_REPLY);
	// add the remote_nonce_i
	pkt << remote_nonce_i;
	// add the cookie
	pkt << cookie_id;
	// add the signed data in the packet
	// - it is precomputed to avoid resource depletion ala syn-flood attack but CPU oriented
	pkt.tail_add( auth_payload.get_data(), auth_payload.get_len() );
	// return the generated packet
	return pkt;
}

/** \brief build a CNX_REPLY packet
 */
pkt_t 	scnx_resp_t::build_resp_auth(const scnx_nonceid_t &remote_nonce_i, const datum_t &master_key
						, const datum_t &data_r
						, skey_auth_type_t chosen_auth_type
						, skey_ciph_type_t chosen_ciph_type)
						throw()
{
	pkt_t	pkt;
	// add the packet type
	pkt << scnx_pkttype_t(scnx_pkttype_t::RESP_AUTH);
	// add the remote nonce_i
	pkt << remote_nonce_i;
	// add the type of packet authentication for the full connection
	pkt << chosen_auth_type;
	// add the type of packet encryption for the full connection
	pkt << chosen_ciph_type;
	// add the data_r
	pkt << data_r;
	
	// compute the proof of possession aka a MAC covering the whole packet using 
	// the computed shared secret as key
	skey_auth_t	skey_auth("sha1/hmac/20");
	skey_auth.init_key( master_key );
	skey_auth.update( pkt.get_data(), pkt.get_len() );
	datum_t		generated_pop = skey_auth.get_output();
	// append the proof of possession to the packet
	pkt.tail_add( generated_pop.get_data(), generated_pop.get_len() );
		
	// return the generated packet
	return pkt;
}

NEOIP_NAMESPACE_END




