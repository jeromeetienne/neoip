/*! \file
    \brief Header of the \ref scnx_itor_t

*/


#ifndef __NEOIP_SCNX_ITOR_HPP__ 
#define __NEOIP_SCNX_ITOR_HPP__ 
/* system include */
/* local include */
#include "neoip_scnx_itor_cb.hpp"
#include "neoip_scnx_itor_auxnego_cb.hpp"
#include "neoip_scnx_err.hpp"
#include "neoip_scnx_event.hpp"
#include "neoip_scnx_profile.hpp"
#include "neoip_dh.hpp"
#include "neoip_cookie.hpp"
#include "neoip_pkt.hpp"
#include "neoip_scnx_nonceid.hpp"
#include "neoip_timeout.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief This object contains the itor needed for a secure connection
 */
class scnx_itor_t : NEOIP_COPY_CTOR_DENY, public timeout_cb_t {
private:
	//! the list of state used by initiator
	enum state_t {
		NONE,
		CNX_REQ_SENT,	//!< when the CNX_REQ packet has been sent
		ITOR_AUTH_SENT,	//!< when the ITOR_AUTH packet has been sent
		MAX
	};

	scnx_nonceid_t	nonce_i;		//!< the nonce_i of the initiator
	state_t		state;			//!< the current state
	datum_t		master_key;		//!< the master key obtained from the exchange
	dh_privkey_t	dh_privkey;		//!< the diffie-hellman private key for this itor
	scnx_profile_t *scnx_profile;		//!< pointer to the scnx_profile_t
	bool		reliable_outter_cnx;	//!< true if the outter connection is reliable.
						//!< false otherwise.
	std::string	remote_idname;		//!< the remote idname
	/*************** auxnego callback	*******************************/
	scnx_itor_auxnego_cb_t*	auxnego_cb;	//!< the auxnego callback
	void *			auxnego_userptr;//!< userptr associated with the auxnego callback

						
	/*************** pkt rxmit	***************************************/
	delaygen_t	rxmit_delaygen;		//!< the delaygen_t for the packet rxmit
	timeout_t	rxmit_timeout;		//!< retransmition timer to rxmit REKEY_REQUEST
	pkt_t		rxmit_pkt;		//!< the packet to xmit/rxmit (may be CNX_REQUEST/ITOR_AUTH)
	bool		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw();
	
	/*************** Packet Building	*******************************/
	pkt_t		build_cnx_request()				throw();
	pkt_t		build_itor_auth(const cookie_id_t &cookie_id)	throw();
	
	/*************** Packet Reception	*******************************/
	scnx_err_t	recv_cnx_reply(pkt_t &pkt, scnx_event_t &scnx_event)	throw(serial_except_t);
	scnx_err_t	recv_resp_auth(pkt_t &pkt, scnx_event_t &scnx_event)	throw(serial_except_t);	

	/*************** Callback	***************************************/
	scnx_itor_cb_t *event_cb;		//!< the event callback
	void *		userptr;		//!< userptr associated with the event callback
	bool		notify_callback(const scnx_event_t &scnx_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	scnx_itor_t(scnx_profile_t *scnx_profile)				throw();
	scnx_itor_t()								throw();
	~scnx_itor_t()								throw();
	
	/*************** Setup function	***************************************/
	scnx_itor_t &	set_event_cb(scnx_itor_cb_t * callback, void *userptr)		throw();
	scnx_itor_t &	set_auxnego_cb(scnx_itor_auxnego_cb_t *callback, void *userptr)	throw();
	scnx_itor_t &	set_reliable_outter_cnx(bool value)				throw();
	scnx_err_t	start()								throw();

	/*************** Query function	***************************************/
	bool		is_null()						const throw();
	bool		is_reliable_outter_cnx()				const throw();

	/*************** Action function	*******************************/
	scnx_err_t	pkt_from_lower(pkt_t &pkt, scnx_event_t &scnx_event)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SCNX_ITOR_HPP__  */



