/*! \file
    \brief Header of the \ref scnx_resp_t
*/


#ifndef __NEOIP_SCNX_RESP_HPP__ 
#define __NEOIP_SCNX_RESP_HPP__ 
/* system include */
/* local include */
#include "neoip_scnx_resp_auxnego_cb.hpp"
#include "neoip_scnx_err.hpp"
#include "neoip_scnx_event.hpp"
#include "neoip_scnx_profile.hpp"
#include "neoip_scnx_nonceid.hpp"
#include "neoip_dh.hpp"
#include "neoip_x509.hpp"
#include "neoip_skey_auth.hpp"
#include "neoip_skey_ciph.hpp"
#include "neoip_cookie.hpp"
#include "neoip_pkt.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief This object contains the respeters needed for a secure connection
 */
class scnx_resp_t : NEOIP_COPY_CTOR_DENY {
private:
	scnx_profile_t *	scnx_profile;		//!< pointer on the scnx_profile_t
	dh_privkey_t		dh_privkey;		//!< the diffie-hellman private key for this resp
	cookie_db_t		cookie;			//!< the context to compute and check cookie
	bool			reliable_outter_cnx;	//!< true if the outter connection is reliable.
							//!< false otherwise.

	/*************** auth_payload generation+precomputation	***************/
	datum_t			generate_auth_payload()		throw();
	datum_t			auth_payload;	//!< contains the auth_r, dh public key, signature of
						//!< of the previous data by the auth_r

	/*************** auxnego callback	*******************************/
	scnx_resp_auxnego_cb_t*	auxnego_cb;		//!< the auxnego callback
	void *			auxnego_userptr;	//!< userptr associated with the auxnego callback

	/*************** Packet Building	*******************************/
	pkt_t 	build_cnx_reply(const scnx_nonceid_t &nonce_i, const cookie_id_t &cookie_id)	throw();
	pkt_t 	build_resp_auth(const scnx_nonceid_t &remote_nonce_i, const datum_t &master_key
				, const datum_t &data_r, skey_auth_type_t chosen_auth_type
				, skey_ciph_type_t chosen_ciph_type)				throw();

	/*************** Packet reception	*******************************/
	scnx_err_t recv_cnx_request(pkt_t &pkt, const datum_t &cookie_pathid, scnx_event_t &scnx_event)
										throw(serial_except_t);
	scnx_err_t recv_itor_auth(pkt_t &pkt, const datum_t &cookie_pathid, scnx_event_t &scnx_event)
										throw(serial_except_t);
public:
	/*************** ctor/dtor	***************************************/
	scnx_resp_t()						throw();
	scnx_resp_t(scnx_profile_t *scnx_profile)		throw();
	~scnx_resp_t()						throw();
	
	/*************** Setup function	***************************************/
	scnx_resp_t &	set_reliable_outter_cnx(bool value = true)			throw();
	scnx_resp_t &	set_auxnego_cb(scnx_resp_auxnego_cb_t *callback, void *userptr)	throw();
	scnx_err_t	start()								throw();
	

	/*************** Query function	***************************************/
	bool		is_null()				const throw();
	bool		is_reliable_outter_cnx()		const throw();

	/*************** Action function	*******************************/
	scnx_err_t	pkt_from_lower(pkt_t &pkt, const datum_t &pathid, scnx_event_t &scnx_event)
											throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SCNX_RESP_HPP__  */



