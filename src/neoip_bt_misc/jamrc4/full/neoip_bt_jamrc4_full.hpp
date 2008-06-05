/*! \file
    \brief Header of the \ref bt_jamrc4_full_t

*/


#ifndef __NEOIP_BT_JAMRC4_FULL_HPP__ 
#define __NEOIP_BT_JAMRC4_FULL_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_jamrc4_full_wikidbg.hpp"
#include "neoip_bt_jamrc4_type.hpp"
#include "neoip_pkt.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_id_t;
class	skey_ciph_t;

/** \brief This object contains the full needed for a scrambled connection
 */
class bt_jamrc4_full_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<bt_jamrc4_full_t, bt_jamrc4_full_wikidbg_init> {
private:
	pkt_t		m_init_recvbuf;	//!< initial receive buffer (already decripted)
	skey_ciph_t *	m_xmit_ciph;	//!< skey_ciph_t to encrypt transmited data
	skey_ciph_t *	m_recv_ciph;	//!< skey_ciph_t to decrypt received data
public:
	/*************** ctor/dtor	***************************************/
	bt_jamrc4_full_t()	throw();
	~bt_jamrc4_full_t()	throw();
	
	/*************** Setup function	***************************************/
	bt_err_t	start(skey_ciph_t *m_xmit_ciph, skey_ciph_t *m_recv_ciph
						, const pkt_t &m_init_recvbuf)	throw();

	/*************** Query function	***************************************/
	bt_jamrc4_type_t type()	const throw()	{ return m_xmit_ciph ? bt_jamrc4_type_t::DOJAM : bt_jamrc4_type_t::NOJAM;	} 
	/*************** Action function	*******************************/
	bt_err_t	notify_recved_data(const pkt_t &pkt_inp, pkt_t &pkt_out)throw(); 
	bt_err_t	notify_toxmit_data(const pkt_t &pkt_inp, pkt_t &pkt_out)throw();
	
	/*************** List of friend class	*******************************/
	friend class	bt_jamrc4_full_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_JAMRC4_FULL_HPP__  */



