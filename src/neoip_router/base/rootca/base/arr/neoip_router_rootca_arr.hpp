/*! \file
    \brief Declaration of the router_rootca_arr_t
    
*/


#ifndef __NEOIP_ROUTER_ROOTCA_ARR_HPP__ 
#define __NEOIP_ROUTER_ROOTCA_ARR_HPP__ 
/* system include */
/* local include */
#include "neoip_router_rootca.hpp"
#include "neoip_item_arr.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	router_name_t;

NEOIP_ITEM_ARR_DECLARATION_START(router_rootca_arr_t, router_rootca_t);
public:
	/*************** query function	***************************************/
	const router_rootca_t *	find_by_dnsname(const router_name_t &dnsname)	const throw();
	
NEOIP_ITEM_ARR_DECLARATION_END(router_rootca_arr_t, router_rootca_t);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_ROOTCA_ARR_HPP__  */



