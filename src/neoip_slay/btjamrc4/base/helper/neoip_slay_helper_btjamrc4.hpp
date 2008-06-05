/*! \file
    \brief Declaration of static helper functions for the btjamrc4 layer
    
*/


#ifndef __NEOIP_SLAY_HELPER_BTJAMRC4_HPP__ 
#define __NEOIP_SLAY_HELPER_BTJAMRC4_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	slay_domain_t;
class	slay_full_t;
class	bt_jamrc4_full_t;

/** \brief static helpers for the slay stuff with slay_domain_t::BTJAMRC4
 */
class slay_helper_btjamrc4_t {
public:	/////////////////////// constant declaration ///////////////////////////
	static const slay_domain_t	DOMAIN_VAR;
	
public:
	static slay_full_t *	build_slay_full(bt_jamrc4_full_t *p_btjamrc4_full)	throw();
	static void		factory_domain_insert()					throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SLAY_HELPER_BTJAMRC4_HPP__  */










