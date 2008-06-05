/*! \file
    \brief Declaration of the ntudp_npos_natlback_t
    
*/


#ifndef __NEOIP_NTUDP_NPOS_NATLBACK_HPP__ 
#define __NEOIP_NTUDP_NPOS_NATLBACK_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_ntudp_npos_natlback_cb.hpp"
#include "neoip_ntudp_npos_natlback_wikidbg.hpp"
#include "neoip_ntudp_npos_saddrecho_cb.hpp"
#include "neoip_ntudp_npos_cli_profile.hpp"
#include "neoip_ntudp_nonce.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_udp_resp_cb.hpp"
#include "neoip_udp_client_cb.hpp"
#include "neoip_timeout.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class ntudp_pserver_pool_t;

/** \brief Perform a inetreach request and notify the result
 */
class ntudp_npos_natlback_t : NEOIP_COPY_CTOR_DENY, private ntudp_npos_saddrecho_cb_t
				, private udp_resp_cb_t, private udp_client_cb_t
				, private timeout_cb_t
				, private wikidbg_obj_t<ntudp_npos_natlback_t, ntudp_npos_natlback_wikidbg_init>
				{
private:
	ipport_addr_t		pserver_addr;	//!< the public server address for the saddrecho
	ntudp_npos_cli_profile_t profile;	//!< the profile attached to this object
	ntudp_nonce_t		request_nonce;	//!< the nonce to authenticate the received packets

	/*************** callback for npos_saddrecho	***********************/
	ntudp_npos_saddrecho_t *npos_saddrecho;	//!< the saddrecho to discover a listening addr
						//!< lview/pview during the first stage
	bool 			neoip_ntudp_npos_saddrecho_event_cb(void *cb_userptr
						, ntudp_npos_saddrecho_t &cb_ntudp_npos_saddrecho
						, const ntudp_npos_event_t &ntudp_npos_event)	throw();

	/*************** udp_resp stuff	***************************************/
	udp_resp_t *	udp_resp;	//!< the udp_resp_t to receives the probe
	bool 		neoip_inet_udp_resp_event_cb(void *cb_userptr, udp_resp_t &cb_udp_resp
							, const udp_event_t &udp_event)	throw();

	/*************** List of incoming connection	***********************/
	class				cnx_t;
	std::list<cnx_t *>		cnx_list;
	void cnx_link(cnx_t *cnx)	throw()	{ cnx_list.push_back(cnx);	}
	void cnx_unlink(cnx_t *cnx)	throw()	{ cnx_list.remove(cnx);		}
	bool				notify_recved_nonce()	throw();	

	/*************** udp_client	***************************************/
	udp_client_t *	udp_client;	//!< the udp_client_t to send the probing
	bool		neoip_udp_client_event_cb(void *cb_userptr, udp_client_t &cb_udp_client
						, const udp_event_t &udp_event)	throw();

	/*************** rxmit timer	***************************************/
	delaygen_t	rxmit_delaygen;	//!< the delay_t generator for the rxmit of packet
	timeout_t	rxmit_timeout;	//!< to periodically send packet over the udp_client_t
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** Callback	***************************************/
	ntudp_npos_natlback_cb_t *	callback;	//!< the callback to notify result
	void *				userptr;	//!< the userptr associated with the callback
	bool				notify_callback(const ntudp_npos_event_t &ntudp_npos_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	ntudp_npos_natlback_t()		throw();
	~ntudp_npos_natlback_t()	throw();
	
	/*************** Setup function	***************************************/
	ntudp_npos_natlback_t &set_profile(const ntudp_npos_cli_profile_t &profile)	throw();	
	ntudp_err_t	start(const ipport_addr_t &pserver_addr, ntudp_pserver_pool_t *pserver_pool
				, ntudp_npos_natlback_cb_t *callback, void * userptr)		throw();

	/*************** Query function	***************************************/
	const ipport_addr_t &	get_pserver_addr()	const throw()	{ return pserver_addr;	}

	/*************** List of friend classes	*******************************/
	friend class	ntudp_npos_natlback_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_NPOS_NATLBACK_HPP__  */



