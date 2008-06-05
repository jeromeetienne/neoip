/*! \file
    \brief Header of the \ref bt_jamrc4_resp_t

*/


#ifndef __NEOIP_BT_JAMRC4_RESP_HPP__ 
#define __NEOIP_BT_JAMRC4_RESP_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_jamrc4_resp_wikidbg.hpp"
#include "neoip_bt_jamrc4_profile.hpp"
#include "neoip_bt_id.hpp"
#include "neoip_pkt.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_jamrc4_full_t;
class	bt_err_t;

/** \brief This object contains the resp needed for a scrambled connection
 */
class bt_jamrc4_resp_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<bt_jamrc4_resp_t, bt_jamrc4_resp_wikidbg_init> {
private:
	bt_jamrc4_profile_t	m_profile;	//!< the profile for this object
	bt_id_t			m_resp_nonce;	//!< the cnx nonce for resp

	pkt_t			m_recv_buffer;	//!< buffer of the data received and not yet consumed
	pkt_t			m_xmit_buffer;	//!< buffer of the data to send
public:
	/*************** ctor/dtor	***************************************/
	bt_jamrc4_resp_t()	throw();
	~bt_jamrc4_resp_t()	throw();
	
	/*************** Setup function	***************************************/
	bt_jamrc4_resp_t&	profile(const bt_jamrc4_profile_t &profile)	throw();
	bt_err_t		start()						throw();

	/*************** action function	*******************************/
	bt_err_t	notify_recved_data(const pkt_t &recved_pkt
					, bt_jamrc4_full_t **jamrc4_full_out)	throw(); 
	pkt_t &		xmit_buffer()	throw()	{ return m_xmit_buffer;	}
	
	/*************** List of friend class	*******************************/
	friend class	bt_jamrc4_resp_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_JAMRC4_RESP_HPP__  */



