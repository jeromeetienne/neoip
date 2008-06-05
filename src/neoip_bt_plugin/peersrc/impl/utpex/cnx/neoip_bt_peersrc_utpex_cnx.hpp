/*! \file
    \brief Header of the bt_peersrc_utpex_cnx_t
    
*/


#ifndef __NEOIP_BT_PEERSRC_UTPEX_CNX_HPP__ 
#define __NEOIP_BT_PEERSRC_UTPEX_CNX_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_peersrc_utpex_cnx_wikidbg.hpp"
#include "neoip_bt_utmsg_cnx_vapi.hpp"
#include "neoip_bt_peersrc_utpex.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_peersrc_utpex_t;
class	bt_swarm_full_utmsg_t;
class	bt_cmd_t;

/** \brief class definition for bt_peersrc_utpex_cnx_t
 */
class bt_peersrc_utpex_cnx_t : NEOIP_COPY_CTOR_DENY
		, private bt_utmsg_cnx_vapi_t, private timeout_cb_t
		, private wikidbg_obj_t<bt_peersrc_utpex_cnx_t, bt_peersrc_utpex_cnx_wikidbg_init, timeout_cb_t, bt_utmsg_cnx_vapi_t>
		{
private:
	bt_peersrc_utpex_t *	peersrc_utpex;	//!< backpointer on the bt_peersrc_utpex_t
	bt_swarm_full_utmsg_t *	full_utmsg;	//!< backpointer on the bt_swarm_full_utmsg_t
	uint32_t		next_seqnb;	//!< the next sequence number to use

	/*************** internal function	*******************************/
	bt_cmd_t		generate_xmit_cmd()		throw();
	
	/*************** xmit_timeout_t	***************************************/
	delaygen_t		xmit_delaygen;	//!< the delay_t generator for xmit	
	timeout_t		xmit_timeout;	//!< to periodically send a packet
	bool 			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_peersrc_utpex_cnx_t() 		throw();
	~bt_peersrc_utpex_cnx_t()		throw();
	
	/*************** Setup function	***************************************/
	bt_err_t	start(bt_peersrc_utpex_t *peersrc_utpex
					, bt_swarm_full_utmsg_t *full_utmsg)	throw();
	
	/*************** query function	***************************************/
	uint32_t	get_next_seqnb()	const throw()	{ return next_seqnb;	}

	/*************** bt_utmsg_cnx_vapi_t	*******************************/
	bool		parse_pkt(const bt_utmsgtype_t &bt_utmsgtype, const pkt_t &pkt)	throw();
	bt_utmsg_vapi_t *utmsg_vapi()	throw()	{ return peersrc_utpex;	}
	/*************** List of friend class	*******************************/
	friend class	bt_peersrc_utpex_cnx_wikidbg_t;
	friend class	bt_peersrc_utpex_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PEERSRC_UTPEX_CNX_HPP__  */



