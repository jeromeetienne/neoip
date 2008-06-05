

#ifndef __NEOIP_ROUTER_PEER_HPP__ 
#define __NEOIP_ROUTER_PEER_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_router_profile.hpp"
#include "neoip_router_peer_wikidbg.hpp"
#include "neoip_router_err.hpp"
#include "neoip_router_lident.hpp"
#include "neoip_router_rident_arr.hpp"
#include "neoip_router_rootca_arr.hpp"
#include "neoip_router_acl.hpp"
#include "neoip_router_acache.hpp"
#include "neoip_dnsgrab_cb.hpp"
#include "neoip_kad_publish_cb.hpp"
#include "neoip_netif_vdev_cb.hpp"
#include "neoip_socket_resp_cb.hpp"
#include "neoip_expireset.hpp"
#include "neoip_ip_netaddr_arr.hpp"
#include "neoip_ip_addr_inval.hpp"
#include "neoip_scnx_auth_ftor.hpp"
#include "neoip_ntudp_rdvpt_ftor.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ntudp_peer_t;
class	kad_peer_t;
class	ipport_addr_t;
class	ipport_aview_t;

class	router_resp_t;
class	router_itor_t;
class	router_full_t;

class	ntudp_rdvpt_t;

/** \brief Class which implement neoip_router
 */
class router_peer_t : NEOIP_COPY_CTOR_DENY, private dnsgrab_cb_t, private kad_publish_cb_t
				, private netif_vdev_cb_t, private scnx_auth_ftor_cb_t
				, private ntudp_rdvpt_ftor_cb_t
				, private wikidbg_obj_t<router_peer_t, router_peer_wikidbg_init>
				{
private:	
	typedef	expireset_t<router_peerid_t>	itor_negcache_t;

	router_profile_t	profile;	//!< the profile for this router_peer_t
	router_acache_t		m_acache;	//!< the address cache
	ip_netaddr_arr_t	ip_netaddr_arr;	//!< the ip_netaddr_t available for the addresses
	itor_negcache_t		itor_negcache;	//!< the negative cache of the failed itor by
						//!< their remote peerid.
	router_lident_t		m_lident;	//!< the local identity
	router_rident_arr_t	m_rident_arr;	//!< the array of all the authorized selfsigned remote peer
	router_rootca_arr_t	m_rootca_arr;	//!< the array of all acceptable router_rootca_t
	router_acl_t		m_acl;		//!< the ACL for the router_name_t
			
	/*************** netif_vdev	***************************************/
	netif_vdev_t *		catchall_netif;		//!< the netif_vdev_t catchall
	ip_addr_t		catchall_iaddr;
	router_err_t		catchall_netif_ctor()					throw();
	bool			neoip_netif_vdev_cb(void *cb_userptr, netif_vdev_t &cb_netif_vdev
						, uint16_t ethertype, pkt_t &pkt)	throw();
								
	/*************** identity database management	***********************/
	bool			dnsname_is_dnsgrab_ok(const router_name_t &dnsfqname)	const throw();

	/*************** scnx_auth_ftor_cb_t	*******************************/
	scnx_err_t		scnx_auth_ftor_cb(void *userptr, const x509_cert_t &x509_cert) throw();
	scnx_err_t		scnx_auth_ftor_selfsigned_cb(void *userptr, const x509_cert_t &x509_cert) throw();
	scnx_err_t		scnx_auth_ftor_authsigned_cb(void *userptr, const x509_cert_t &x509_cert) throw();
	scnx_err_t		scnx_auth_ftor_nonesigned_cb(void *userptr, const x509_cert_t &x509_cert) throw();
	
	/*************** ntudp_peer_t stuff	*******************************/
	ntudp_peer_t *		ntudp_peer;
	kad_peer_t *		get_kad_peer()	const throw();
	bool			ntudp_rdvpt_ftor_cb(void *userptr, const ntudp_rdvpt_t &ntudp_rdvpt)throw();

	/*************** peer record stuff	*******************************/
	void			publish_peer_record()		throw();
	kad_publish_t *		kad_publish;	//!< the kad_publish_t for the peer record
	bool			neoip_kad_publish_cb(void *cb_userptr, kad_publish_t &cb_kad_publish
						, const kad_event_t &cb_kad_event) throw();

	/*************** dnsgrab_t stuff	*******************************/
	dnsgrab_t *		dnsgrab;
	bool			neoip_dnsgrab_cb(void *cb_userptr, dnsgrab_t &cb_dnsgrab, dnsgrab_request_t &request)	throw();
	bool			dnsgrab_byaddr_cb(dnsgrab_request_t &request)	throw();
	bool			dnsgrab_byname_cb(dnsgrab_request_t &request)	throw();
	dnsgrab_t *		get_dnsgrab()	throw()	{ return dnsgrab;	}
		
	/*************** dnsgrab reply building	*******************************/
	void			set_dnsreq_reply_lident(dnsgrab_request_t &request)	const throw();
	void			set_dnsreq_reply_cnxfull(dnsgrab_request_t &request
						, const router_full_t *router_full)	const throw();
	void			set_dnsreq_reply_notfound(dnsgrab_request_t &request)	const throw();
						
	/*************** router_resp_t	***************************************/
	router_resp_t *		router_resp;

	/*************** List of router_itor_t	*******************************/
	std::list<router_itor_t *>	itor_db;
	void itor_dolink(router_itor_t *itor)	throw()	{ itor_db.push_back(itor);	}
	void itor_unlink(router_itor_t *itor)	throw()	{ itor_db.remove(itor);		}
	router_itor_t *	itor_by_remote_peerid(const router_peerid_t &remote_peerid)	throw();

	/*************** List of router_full_t	*******************************/
	std::list<router_full_t *>	full_db;
	void full_dolink(router_full_t *full)	throw()	{ full_db.push_back(full);	}
	void full_unlink(router_full_t *full)	throw()	{ full_db.remove(full);		}
	router_full_t *	full_by_local_iaddr(const ip_addr_t &local_iaddr)		const throw();
	router_full_t *	full_by_remote_iaddr(const ip_addr_t &remote_iaddr)		const throw();
	router_full_t *	full_by_remote_peerid(const router_peerid_t &remote_peerid)	const throw();
public:
	/*************** ctor/dtor	***************************************/
	router_peer_t()		throw();
	~router_peer_t()	throw();
	
	/*************** Setup Function	***************************************/
	router_peer_t &	set_profile(const router_profile_t &profile)	throw();	
	router_peer_t &	set_acache(const router_acache_t &acache)	throw();	
	router_err_t	start(const ipport_aview_t &ipport_listen_aview, const router_lident_t &lident
					, const router_rident_arr_t &rident_arr
					, const router_rootca_arr_t &m_rootca_arr
					, const router_acl_t &m_acl
					, const ip_netaddr_arr_t &ip_netaddr_arr)	throw();

	/*************** Query Function	***************************************/
	const router_profile_t &	get_profile()	const throw()	{ return profile;	}
	const router_acache_t &		acache()	const throw()	{ return m_acache;	}
	const router_lident_t &		lident()	const throw()	{ return m_lident;	}
	const router_rident_arr_t &	rident_arr()	const throw()	{ return m_rident_arr;	}
	const router_rootca_arr_t &	rootca_arr()	const throw()	{ return m_rootca_arr;	}
	const router_acl_t &		acl()		const throw()	{ return m_acl;		}
	
	ip_addr_inval_t		get_avail_iaddr()					const throw();
	ip_addr_inval_t		get_used_iaddr()					const throw();
	router_peerid_t		dnsname2peerid(const router_name_t &dnsfqname)		const throw();
	static router_peerid_t	certname2peerid(const std::string &certname)		throw();
	router_name_t		certname2dnsname(const std::string &certname)		const throw();

	/*************** List of friend class	*******************************/
	friend class	router_peer_wikidbg_t;
	friend class	router_peer_http_t;
	friend class	router_resp_t;
	friend class	router_itor_t;
	friend class	router_itor_cnx_t;
	friend class	router_full_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_PEER_HPP__  */



