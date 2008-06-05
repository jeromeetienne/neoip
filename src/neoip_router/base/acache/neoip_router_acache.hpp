/*! \file
    \brief Header of the \ref router_acache_t

*/


#ifndef __NEOIP_ROUTER_ACACHE_HPP__ 
#define __NEOIP_ROUTER_ACACHE_HPP__ 
/* system include */
/* local include */
#include "neoip_router_acache_wikidbg.hpp"
#include "neoip_router_acache_item.hpp"
#include "neoip_expireset2.hpp"
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	router_peer_t;

/** \brief An item of the router_acache_t
 */
class router_acache_t : public expireset2_t<router_acache_item_t>
			, private wikidbg_obj_t<router_acache_t, router_acache_wikidbg_init> {
private:
public:
	/*************** Update function	*******************************/
	void		update(const router_name_t &remote_dnsname, const ip_addr_t &local_iaddr
				, const ip_addr_t &remote_iaddr, const delay_t &expire_delay)	throw();

	/*************** Query function	***************************************/
	const router_acache_item_t * find_by_remote_dnsname(const router_name_t &dnsname)	const throw();
	const router_acache_item_t * find_by_remote_iaddr(const ip_addr_t &addr)	 const throw();

	/*************** Friend class	***************************************/
	friend class	router_acache_wikidbg_t;
	friend class	router_acache_item_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_ACACHE_HPP__  */



