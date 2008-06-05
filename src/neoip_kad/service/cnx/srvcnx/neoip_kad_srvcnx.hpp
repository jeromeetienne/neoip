/*! \file
    \brief Declaration of the kad_srvcnx_t

*/


#ifndef __NEOIP_KAD_SRVCNX_HPP__ 
#define __NEOIP_KAD_SRVCNX_HPP__ 
/* system include */
#include <iostream>
#include <string>
#include <list>
/* local include */
#include "neoip_kad_srvcnx_cb.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_pkttype_profile.hpp"
#include "neoip_udp_vresp_cb.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	udp_vresp_t;
class	kad_peer_t;
class	ipport_addr_t;

/** \brief do a kad srvcnx
 */
class kad_srvcnx_t : public udp_vresp_cb_t, NEOIP_COPY_CTOR_DENY {
private:
	pkttype_profile_t	pkttype_profile;	//!< pkttype_profile_t to register on udp_vresp_t

	/*************** udp_vresp_t	***************************************/
	udp_vresp_t *		udp_vresp;
	bool			neoip_inet_udp_vresp_event_cb(void *userptr, udp_vresp_t &cb_udp_vresp
							, const udp_event_t &udp_event) throw();
	/*************** list of incoming connections	***********************/
	class				cnx_t;
	std::list<cnx_t *>		cnx_list;
	void cnx_dolink(cnx_t *cnx)	throw()	{ cnx_list.push_back(cnx);	}
	void cnx_unlink(cnx_t *cnx)	throw()	{ cnx_list.remove(cnx);		}
	
	/*************** Callback	***************************************/
	kad_srvcnx_cb_t *	callback;	//!< the callback to notify result
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(pkt_t &pkt, const ipport_addr_t &local_oaddr
						, const ipport_addr_t &remote_oaddr)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	kad_srvcnx_t()		throw();
	~kad_srvcnx_t()		throw();
	
	/*************** Setup function	***************************************/
	kad_err_t	start(udp_vresp_t *udp_vresp, const pkttype_profile_t &pkttype_profile
					, kad_srvcnx_cb_t *callback, void *userptr)	throw();

	/*************** Query function	***************************************/		
	const ipport_addr_t &	get_listen_oaddr()	const throw();
};

NEOIP_NAMESPACE_END

#endif // __NEOIP_KAD_SRVCNX_HPP__ 



