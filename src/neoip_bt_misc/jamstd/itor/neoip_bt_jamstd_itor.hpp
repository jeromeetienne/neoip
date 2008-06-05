/*! \file
    \brief Header of the \ref bt_jamstd_itor_t

*/


#ifndef __NEOIP_BT_JAMSTD_ITOR_HPP__ 
#define __NEOIP_BT_JAMSTD_ITOR_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_jamstd_itor_wikidbg.hpp"
#include "neoip_bt_jamstd_itor_state.hpp"
#include "neoip_bt_jamstd_profile.hpp"
#include "neoip_bt_id.hpp"
#include "neoip_dh_privkey.hpp"
#include "neoip_pkt.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_jamstd_full_t;
class	bt_err_t;
class	skey_ciph_t;
class	dh_shsecret_t;

/** \brief This object contains the itor needed for a scrambled connection
 */
class bt_jamstd_itor_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<bt_jamstd_itor_t, bt_jamstd_itor_wikidbg_init> {
private:
	bt_jamstd_profile_t	m_profile;	//!< the profile for this object
	bt_id_t			m_cnx_shsecret;	//!< the connection shared-secret
	dh_privkey_t		m_dh_privkey;	//!< the diffie-hellman private key
	bt_jamstd_itor_state_t	m_state;	//!< the current state

	pkt_t			m_recv_buffer;	//!< buffer of the data received and not yet consumed
	pkt_t			m_xmit_buffer;	//!< buffer of the data to send
	
	skey_ciph_t *		m_recv_ciph;	//!< skey_ciph_t to decrypt m_recv_buffer
	skey_ciph_t *		m_xmit_ciph;	//!< skey_ciph_t to encrypt m__buffer
	datum_t			m_cnxauth_mark;	//!< the marker to sync with remote stream
	uint16_t		m_cnxauth_padlen;//!< the cnxauth_padlen (used in PADAUTH_WAIT)
	bt_jamstd_negoflag_t 	m_negoflag_selected;	//!< the selected bt_jamstd_negoflag_t

	/*************** parse packet *****************************************/
	bt_err_t	parse_in_cnxesta_wait(bt_jamstd_full_t **jamstd_full_out
							, bool *stop_parsing)	throw();
	bt_err_t	parse_in_cnxauth_wait(bt_jamstd_full_t **jamstd_full_out
							, bool *stop_parsing)	throw();
	bt_err_t	parse_in_padauth_wait(bt_jamstd_full_t **jamstd_full_out
							, bool *stop_parsing)	throw();

	/*************** build packet *****************************************/
	pkt_t		build_cnxesta_pkt()					throw();
	pkt_t		build_cnxauth_pkt(const dh_shsecret_t &dh_shsecret
				, const bt_jamstd_negoflag_t &negoflag_proposed)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_jamstd_itor_t()	throw();
	~bt_jamstd_itor_t()	throw();
	
	/*************** Setup function	***************************************/
	bt_jamstd_itor_t&	profile(const bt_jamstd_profile_t &profile)		throw();
	bt_err_t	start(const bt_id_t &m_cnx_shsecret)			throw();

	/*************** action function	*******************************/
	bt_err_t	notify_recved_data(const pkt_t &recved_pkt
					, bt_jamstd_full_t **jamstd_full_out)	throw(); 
	pkt_t &		xmit_buffer()	throw()	{ return m_xmit_buffer;	}
	
	/*************** List of friend class	*******************************/
	friend class	bt_jamstd_itor_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_JAMSTD_ITOR_HPP__  */



