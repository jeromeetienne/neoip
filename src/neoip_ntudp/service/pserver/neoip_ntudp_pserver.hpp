/*! \file
    \brief Declaration of the ntudp_pserver_t
    
*/


#ifndef __NEOIP_NTUDP_PSERVER_HPP__ 
#define __NEOIP_NTUDP_PSERVER_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_ntudp_pserver_wikidbg.hpp"
#include "neoip_ntudp_pserver_profile.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_kad_publish_cb.hpp"
#include "neoip_udp_vresp_cb.hpp"
#include "neoip_slotpool.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ntudp_pserver_tunnel_t;
class	ntudp_pserver_extcnx_t;
class	ntudp_pserver_reverse_t;
class	ntudp_dircnx_server_t;
class	ntudp_peerid_t;
class	ntudp_peer_t;

/** \brief class definition for ntudp_pserver_t
 */
class ntudp_pserver_t : NEOIP_COPY_CTOR_DENY, private udp_vresp_cb_t, private kad_publish_cb_t
				, private wikidbg_obj_t<ntudp_pserver_t, ntudp_pserver_wikidbg_init>	
				{
private:
	/*************** internal data	***************************************/
	ntudp_peer_t *		ntudp_peer;	//!< backpoint on the ntudp_peer
	ntudp_dircnx_server_t *	dircnx_server;	//!< the direct connection server to accept direct cnx
	ntudp_pserver_profile_t	profile;	//!< the profile attached to this object

	/*************** ntudp_pserver_rec_t	*******************************/
	ntudp_err_t	pserver_rec_publish()	throw();
	kad_publish_t *	kad_publish;		//!< the kad_publish_t for the pserver record
	bool		neoip_kad_publish_cb(void *cb_userptr, kad_publish_t &cb_kad_publish
						, const kad_event_t &cb_kad_event) throw();

	/*************** Handle the udp_vresp_t	*******************************/
	bool neoip_inet_udp_vresp_event_cb(void *cb_userptr, udp_vresp_t &cb_udp_vresp
							, const udp_event_t &udp_event)	throw();	

	/*************** List of tunnel	***************************************/
	std::list<ntudp_pserver_tunnel_t *>		tunnel_db;
	void tunnel_link(ntudp_pserver_tunnel_t *tunnel)	throw()	{ tunnel_db.push_back(tunnel);	}
	void tunnel_unlink(ntudp_pserver_tunnel_t *tunnel)	throw()	{ tunnel_db.remove(tunnel);	}

	/*************** List of external connection	***********************/
	std::list<ntudp_pserver_extcnx_t *>		extcnx_db;
	void extcnx_link(ntudp_pserver_extcnx_t *extcnx)	throw()	{ extcnx_db.push_back(extcnx);	}
	void extcnx_unlink(ntudp_pserver_extcnx_t *extcnx)	throw()	{ extcnx_db.remove(extcnx);	}
	slotpool_t	extcnx_slotpool;	//!< slotpool to allocate slot_id for each extcnx

	/*************** List of reverse_cnx_req connection	***************/
	std::list<ntudp_pserver_reverse_t *>		reverse_db;
	void reverse_link(ntudp_pserver_reverse_t *reverse)	throw()	{ reverse_db.push_back(reverse);}
	void reverse_unlink(ntudp_pserver_reverse_t *reverse)	throw()	{ reverse_db.remove(reverse);	}
public:
	/*************** ctor/dtor	***************************************/
	ntudp_pserver_t()		throw();
	~ntudp_pserver_t()		throw();

	/*************** Setup Function	***************************************/
	ntudp_pserver_t &set_profile(const ntudp_pserver_profile_t &profile)	throw();
	ntudp_err_t	start(ntudp_peer_t *ntudp_peer)	throw();

	/*************** Query Function	***************************************/
	ntudp_pserver_tunnel_t *get_tunnel_from_peerid(const ntudp_peerid_t &peerid)	throw();
	ntudp_pserver_extcnx_t *get_extcnx_from_slotid(const slot_id_t &slotid)		throw();

	/*************** Friend functions	*******************************/
	friend class ntudp_pserver_tunnel_t;
	friend class ntudp_pserver_extcnx_t;
	friend class ntudp_pserver_reverse_t;
	friend class ntudp_pserver_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_PSERVER_HPP__  */



