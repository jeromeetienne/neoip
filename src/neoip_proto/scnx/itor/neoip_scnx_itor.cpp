/*! \file
    \brief Header of the \ref scnx_itor_t

*/

/* system include */
/* local include */
#include "neoip_scnx_itor.hpp"
#include "neoip_scnx_full.hpp"
#include "neoip_scnx_pkttype.hpp"
#include "neoip_cookie.hpp"
#include "neoip_pkt.hpp"
#include "neoip_serial.hpp"
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
scnx_itor_t::scnx_itor_t()	throw()
{
	scnx_profile		= NULL;
}

/** \brief Constructor with value
 */
scnx_itor_t::scnx_itor_t(scnx_profile_t *scnx_profile)		throw()
{
	// copy the parameter
	this->scnx_profile	= scnx_profile;

	// zero some parameter
	event_cb		= NULL;
	auxnego_cb		= NULL;
	reliable_outter_cnx	= false;
	state			= NONE;
	nonce_i			= scnx_nonceid_t::build_random();
	dh_privkey		= dh_privkey_t(scnx_profile->dh_param());

	// init the rxmit_delaygen
	// TODO put this constant in the profile
	rxmit_delaygen	= delaygen_t( delaygen_expboff_arg_t().min_delay(delay_t::from_sec(1))
								.max_delay(delay_t::from_sec(30))
								.random_range(0.2) );
}

/** \brief Destructor
 */
scnx_itor_t::~scnx_itor_t()				throw()
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief define if the outter connection is reliable or not
 */
scnx_itor_t & scnx_itor_t::set_reliable_outter_cnx(bool value)		throw()
{
	// copy the parameter
	reliable_outter_cnx	= value;
	// return the object itself
	return *this;
}

/** \brief set the event event_cb
 * 
 * - it is a MANDATORY parameter and is MUST be set before start()
 */
scnx_itor_t & scnx_itor_t::set_event_cb(scnx_itor_cb_t * callback, void *userptr)	throw()
{
	// copy the parameter
	this->event_cb	= callback;
	this->userptr	= userptr;
	// return the object itself
	return *this;
}

/** \brief set the auxnego callback
 */
scnx_itor_t &scnx_itor_t::set_auxnego_cb(scnx_itor_auxnego_cb_t *callback, void *userptr)	throw()
{
	// copy the parameter
	this->auxnego_cb	= callback;
	this->auxnego_userptr	= userptr;
	// return the object itself
	return *this;
}

/** \brief Start the action
 */
scnx_err_t scnx_itor_t::start()				throw()
{
	// sanity check - the rxmit_timeout MUST NOT be pending
	DBG_ASSERT( rxmit_timeout.is_running() == false );
	// start the packet retransmition
	rxmit_timeout.start(rxmit_delaygen.current(), this, NULL);
	// set up the state
	state		= CNX_REQ_SENT;
	rxmit_pkt	= build_cnx_request();
	// return no error
	return scnx_err_t::OK;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     Query function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true if the outter connection is reliable, false otherwise
 * 
 * - if the underlying connection is not reliable, packets that needs reliability
 *   (e.g scnx_pkttype_t::REKEY_REQUEST) need to be explictly retransmited.
 */
bool	scnx_itor_t::is_reliable_outter_cnx()			const throw()
{
	return reliable_outter_cnx;
}


/** \brief return true if the object is null
 */
bool	scnx_itor_t::is_null()				const throw()
{
	if( scnx_profile )	return false;
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   rxmit_timeout callback
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool	scnx_itor_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// send the packet
	scnx_event_t	scnx_event = scnx_event_t::build_pkt_to_lower(&rxmit_pkt);
	bool tokeep	= notify_callback(scnx_event);
	if( !tokeep )	return false;
	
	// if the outter connection is reliable, stop the timer
	if( is_reliable_outter_cnx() ){
		rxmit_timeout.stop();
	}else{	// if the outter connection is reliable, init the new delay
		rxmit_timeout.change_period( rxmit_delaygen.pre_inc() );
	}
	return true;
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        Packet Building
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief build the CNX_REQUEST
 */
pkt_t scnx_itor_t::build_cnx_request()	throw()
{
	pkt_t	pkt;
	// add the packet type
	pkt << scnx_pkttype_t(scnx_pkttype_t::CNX_REQUEST);
	// add the nonce
	pkt << nonce_i;
	// return the just built packet
	return pkt;
}

/** \brief build the ITOR_AUTH
 */
pkt_t scnx_itor_t::build_itor_auth(const cookie_id_t &remote_cookie)	throw()
{
	const x509_privkey_t &	ident_privkey	= scnx_profile->ident_privkey();
	datum_t			auxnego_payl;	
	pkt_t			pkt;
	
	// build the begining of the packet
	pkt << scnx_pkttype_t(scnx_pkttype_t::ITOR_AUTH);
	pkt << nonce_i;
	pkt << remote_cookie;

	// serialize the signed data appart to sign them later
	bytearray_t	bytearray;
	bytearray	<< scnx_profile->ident_cert();
	bytearray	<< dh_privkey.get_pubkey();
	bytearray	<< ident_privkey.sign_data( bytearray.to_datum(datum_t::NOCOPY) );
	pkt.tail_add( bytearray.to_datum(datum_t::NOCOPY) );

	// add the option negociation parg
	pkt << scnx_profile->skey_auth_optlist();
	pkt << scnx_profile->skey_ciph_optlist();

	// build the auxilary negociation payload if needed
	if( auxnego_cb )
		auxnego_payl = auxnego_cb->neoip_scnx_itor_build_auxnego_cb(auxnego_userptr, *this);
		
	// add the auxnego_payl
	pkt << auxnego_payl;

	// compute the proof of possession aka a MAC covering the whole packet using 
	// the computed shared secret as key
	skey_auth_t	skey_auth("sha1/hmac/20");
	skey_auth.init_key( master_key );
	skey_auth.update( pkt.to_datum(datum_t::NOCOPY) );
	// append the pop to the packet
	pkt.tail_add( skey_auth.get_output() );	
	// return the just built packet
	return pkt;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                        packet processing
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief handle packet from the lower network layer
 */
scnx_err_t scnx_itor_t::pkt_from_lower(pkt_t &pkt, scnx_event_t &scnx_event)		throw()
{
	KLOG_DBG("enter pkt=" << pkt);

	// zero the scnx_event_t
	scnx_event	= scnx_event_t();
	
	// parse the incoming packet	
	try {
		scnx_pkttype_t	pkttype;
		// read the pkttype (without consuming)
		pkt.unserial_peek( pkttype );
		KLOG_DBG("received a pkttype=" << std::hex << pkttype );
/////////////////////
/////////////////////
/////////////////////
/////////////////////
// TODO how to react to error using the error produced by recv_cnx_reply and recv_resp_auth
// - should i product a scnx_event CNX_REFUSED?
// - currently nobody product a scnx_event_t::refused
// - TODO when is a itor connection refused anyway ?
/////////////////////
/////////////////////
/////////////////////
/////////////////////
/////////////////////
		// handle the packet according to the pkttype
		switch( pkttype.get_value() ){
		case scnx_pkttype_t::CNX_REPLY:
				// if the itor is not in the proper state, return an error
				if( state != CNX_REQ_SENT )	return scnx_err_t::BAD_ITOR_STATE;
				// handle the incoming packet
				return recv_cnx_reply(pkt, scnx_event);
		case scnx_pkttype_t::RESP_AUTH:	
				// if the itor is not in the proper state, return an error
				if( state != ITOR_AUTH_SENT )	return scnx_err_t::BAD_ITOR_STATE;
				// handle the incoming packet
				return recv_resp_auth(pkt, scnx_event);
		default:	break;
		}
	} catch(serial_except_t &e) {
		return scnx_err_t(scnx_err_t::BOGUS_PKT, "Cant parse packet due to " + e.what());
	}
	
	// NOTE: if this point is reached, the pkttype is unknown
	return scnx_err_t(scnx_err_t::BOGUS_PKT, "Unknown packet type");
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        packet reception
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/** \brief handle the reception of a CNX_REQUEST
 */
scnx_err_t	scnx_itor_t::recv_cnx_reply(pkt_t &pkt, scnx_event_t &scnx_event) throw(serial_except_t)
{
	scnx_pkttype_t	pkttype;
	scnx_nonceid_t	remote_nonce_i;
	cookie_id_t	remote_cookie;
	x509_cert_t	remote_cert;
	dh_pubkey_t	remote_dh_pubkey;
	datum_t		remote_signature;	
	datum_t		orig_pkt_datum	= pkt.to_datum();
	const scnx_auth_ftor_t&	scnx_auth_ftor	= scnx_profile->scnx_auth_ftor();
	
	// sanity check - the state MUST be CNX_REQ_SENT
	DBG_ASSERT( state == CNX_REQ_SENT );

	// read the pkttype
	pkt >> pkttype;
	
	// read the incoming nonce
	pkt >> remote_nonce_i;
	// check if the incoming nonce_i is valid, if NOT
	if( remote_nonce_i != nonce_i )	return scnx_err_t::BAD_NONCE;

	// read the responder cookie;
	pkt >> remote_cookie;
	

// get and check the remote certificate
	// set a mark of where the signature coverage start
	size_t	sign_cover_start= orig_pkt_datum.get_len() - pkt.get_len();
	// get the remote certificate
	pkt >> remote_cert;

	// call the scnx_auth_ftor to determine if this remote certificate is allowed or not
	scnx_err_t	scnx_err;
	scnx_err	= scnx_auth_ftor( remote_cert );
	// if this certificate is not allowed, return now
	if( scnx_err.failed() )	return scnx_err;	
	// backup the remote_cert subject to pass it to scnx_full_t later
	remote_idname	= remote_cert.get_subject_name();	
	
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
	if( remote_cert.verify_data(sign_cover_data, remote_signature).failed() )
		return scnx_err_t::BAD_SIGNATURE;

	// compute the diffie-hellman shared secret
	dh_shsecret_t	dh_shsecret( dh_privkey, remote_dh_pubkey, scnx_profile->dh_param() );

	// compute the master symmetric key
	skey_auth_t	skey_auth("sha1/hmac/20");
	skey_auth.init_key( remote_nonce_i.to_datum() + remote_cookie.to_datum() );
	skey_auth.update( dh_shsecret.get_shsecret_datum() );
	master_key = skey_auth.get_output();
	KLOG_DBG("enter master_key=" << master_key);

	// change the state to ITOR_AUTH_SENT
	state = ITOR_AUTH_SENT;

	// build the ITOR_AUTH packet
	rxmit_pkt = build_itor_auth(remote_cookie);

	// handle the retransmition timer
	rxmit_delaygen.reset();
	rxmit_timeout.start(rxmit_delaygen.current(), this, NULL);
	
	// return no error
	return scnx_err_t::OK;	
}

/** \brief handle the reception of a RESP_AUTH
 */
scnx_err_t	scnx_itor_t::recv_resp_auth(pkt_t &pkt, scnx_event_t &scnx_event) throw(serial_except_t)
{
	scnx_pkttype_t		pkttype;
	scnx_nonceid_t		remote_nonce_i;
	skey_auth_type_t	chosen_auth_type;
	skey_ciph_type_t	chosen_ciph_type;
	datum_t			auxnego_payl;
	void *			auxnego_ptr	= NULL;
	skey_auth_t		skey_auth("sha1/hmac/20");	// TODO to put in the profile
	datum_t			orig_pkt_datum	= pkt.to_datum();
	size_t			pop_cover_start	= orig_pkt_datum.get_len() - pkt.get_len();

	// sanity check - the state MUST be ITOR_AUTH_SENT
	DBG_ASSERT( state == ITOR_AUTH_SENT );

	// read the pkttype
	pkt >> pkttype;
	DBG_ASSERT( pkttype == scnx_pkttype_t::RESP_AUTH );

// check the incoming nonce
	// read the incoming nonce
	pkt >> remote_nonce_i;
	// check if the incoming nonce_i is valid, if NOT
	if( remote_nonce_i != nonce_i )	return scnx_err_t::BAD_NONCE;

// handle the chosen_auth_type and chosen_ciph_type
	const skey_auth_type_arr_t &	skey_auth_optlist = scnx_profile->skey_auth_optlist();
	const skey_ciph_type_arr_t &	skey_ciph_optlist = scnx_profile->skey_ciph_optlist();
	// get the chosen auth_type
	pkt >> chosen_auth_type;
	// check the skey_auth_type_t chosen by the responder has been proposed
	if( !skey_auth_optlist.contain(chosen_auth_type) )	return scnx_err_t::AUTH_NEGO_FAILED;
	// get the chosen ciph_type
	pkt >> chosen_ciph_type;
	// check the skey_ciph_type_t chosen by the responder has been proposed
	if( !skey_ciph_optlist.contain(chosen_ciph_type) )	return scnx_err_t::CIPH_NEGO_FAILED;

// get the auxnego_payl
	pkt >> auxnego_payl;

// check the pop Proof Of Possession
	// set a mark of where the pop coverage end
	size_t	pop_cover_end	= orig_pkt_datum.get_len() - pkt.get_len();
	// get the covered data (aka the whole packet)
	datum_t	pop_cover_data 	= orig_pkt_datum.get_range(pop_cover_start
						, pop_cover_end-pop_cover_start, datum_t::NOCOPY);
	// get the remote_pop
	datum_t	remote_pop	= pkt.tail_consume(skey_auth.get_output_len());
	// compute the proof of possession
	skey_auth.init_key( master_key );
	skey_auth.update( pop_cover_data );
	// check if the received pop is equal to the generated one
	if( skey_auth.get_output() != remote_pop )	return scnx_err_t::BAD_POP;

	// NOTE: if this point is reached, the connection is accepted

	// stop the rxmit_timeout
	rxmit_timeout.stop();

	// handle the auxilary negociation
	if( auxnego_cb ){
		scnx_err_t	err;
		err = auxnego_cb->neoip_scnx_itor_recv_resp_auxnego_cb(auxnego_userptr, *this
							, auxnego_payl, &auxnego_ptr);
		if( !err.succeed() )	return err;
	}
	
	// log to debug
	KLOG_DBG("CONNECTION ESTABLISHED with auth_type=" << chosen_auth_type 
					<< " ciph_type=" << chosen_ciph_type
					<< " master_key=" << master_key );

	// build the scnx_full_t issued from this connection
	scnx_full_t *	scnx_full;
	scnx_full	= nipmem_new scnx_full_t(master_key, chosen_ciph_type, chosen_auth_type
						, true, is_reliable_outter_cnx()
						, scnx_profile->ident_cert().get_subject_name()
						, remote_idname, pkt_t(), pkt_t());
	// notify the event *synchronously*
	scnx_event	= scnx_event_t::build_cnx_established(scnx_full, auxnego_ptr);

	// return no error
	return scnx_err_t::OK;
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     main function to notify event to the caller
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief notify the caller callback
 * 
 * @return a tokeep
 */
bool 	scnx_itor_t::notify_callback(const scnx_event_t &scnx_event)	throw()
{
	// sanity check - the event_cb MUST NOT be NULL
	DBG_ASSERT( event_cb );
	// backup the tokey_check_t context to check after the event_cb notification
	TOKEEP_CHECK_BACKUP_DFL(*event_cb);
	// notify the caller
	bool tokeep = event_cb->neoip_scnx_itor_event_cb(userptr, *this, scnx_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END




