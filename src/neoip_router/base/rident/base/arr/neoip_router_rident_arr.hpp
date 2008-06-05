/*! \file
    \brief Declaration of the router_rident_arr_t
    
*/


#ifndef __NEOIP_ROUTER_RIDENT_ARR_HPP__ 
#define __NEOIP_ROUTER_RIDENT_ARR_HPP__ 
/* system include */
/* local include */
#include "neoip_router_rident.hpp"
#include "neoip_item_arr.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_ITEM_ARR_DECLARATION_START(router_rident_arr_t, router_rident_t);
public:
	/*************** query function	***************************************/
	const router_rident_t *	find_by_peerid(const router_peerid_t &peerid)	const throw();
	const router_rident_t *	find_by_name(const router_name_t &remote_name
					, const router_profile_t &profile)	const throw();
	const router_rident_t &	get_by_peerid(const router_peerid_t &peerid)	const throw();
	
NEOIP_ITEM_ARR_DECLARATION_END(router_rident_arr_t, router_rident_t);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_RIDENT_ARR_HPP__  */



