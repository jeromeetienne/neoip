/*! \file
    \brief Declaration of the kad_clicnx_t


*/


#ifndef __NEOIP_KAD_CLICNX_HPP__ 
#define __NEOIP_KAD_CLICNX_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_clicnx_cb.hpp"
#include "neoip_kad_clicnx_profile.hpp"
#include "neoip_kad_clicnx_wikidbg.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_pkt.hpp"
#include "neoip_timeout.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_udp_client.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	ipport_addr_t;
class	kad_stat_t;
class	kad_peer_t;

/** \brief do a client connection for the kad layer
 */
class kad_clicnx_t : NEOIP_COPY_CTOR_DENY, private udp_client_cb_t, private timeout_cb_t
			, private wikidbg_obj_t<kad_clicnx_t, kad_clicnx_wikidbg_init>
			{
private:
	/*************** internal data	***************************************/
	kad_peer_t *		kad_peer;	//!< backpointer on the kad_peer_t
	pkt_t			pkt_to_send;	//!< the packet to send
	kad_clicnx_profile_t	profile;	//!< the profile attached to this object
	kad_stat_t *		kad_stat;	//!< to update the byte counter (if == NULL, do nothing)

	/*************** rxmit timer	***************************************/
	delaygen_t	rxmit_delaygen;	//!< the delay_t generator for the rxmit of packet
	timeout_t	rxmit_timeout;	//!< to periodically send packet over the udp_client_t
	bool 		rxmit_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw();
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	/*************** expire_timeout	***************************************/
	timeout_t	expire_timeout;	
	bool 		expire_timeout_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	
	/*************** udp_client	***************************************/
	udp_client_t *	udp_client;		//!< the udp_client_t on which the protocol occurs
	bool 		neoip_udp_client_event_cb(void *cb_userptr, udp_client_t &cb_udp_client
							, const udp_event_t &udp_event)	throw();
	/*************** process udp_event_t	*******************************/
	bool		handle_recved_data(pkt_t &pkt)				throw();
	bool		handle_reset_reply(pkt_t &pkt)				throw();

	/*************** Callback	***************************************/
	kad_clicnx_cb_t *	callback;	//!< the callback to notify result
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(const kad_event_t &kad_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	kad_clicnx_t()		throw();
	~kad_clicnx_t()		throw();
	
	/*************** Start function	***************************************/
	kad_clicnx_t &	set_profile(const kad_clicnx_profile_t &profile)	throw();
	kad_err_t	start(kad_peer_t *kad_peer, const pkt_t &pkt, const delay_t &expire_delay
					, const ipport_addr_t &remote_oaddr, kad_stat_t *kad_stat
					, kad_clicnx_cb_t *callback, void *userptr)	throw();

	/*************** Query function	***************************************/
	const ipport_addr_t &get_local_oaddr()	const throw()	{ return udp_client->get_local_addr();	}
	const ipport_addr_t &get_remote_oaddr()const throw()	{ return udp_client->get_remote_addr();	}

	/*************** List of friend classes	*******************************/
	friend class	kad_clicnx_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif // __NEOIP_KAD_CLICNX_HPP__ 



