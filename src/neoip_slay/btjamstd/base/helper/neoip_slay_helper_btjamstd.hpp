/*! \file
    \brief Declaration of static helper functions for the btjamstd layer
    
*/


#ifndef __NEOIP_SLAY_HELPER_BTJAMSTD_HPP__ 
#define __NEOIP_SLAY_HELPER_BTJAMSTD_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	slay_domain_t;
class	slay_full_t;
class	bt_jamstd_full_t;

/** \brief static helpers for the slay stuff with slay_domain_t::BTJAMSTD
 */
class slay_helper_btjamstd_t {
public:	/////////////////////// constant declaration ///////////////////////////
	static const slay_domain_t	DOMAIN_VAR;
	
public:
	static slay_full_t *	build_slay_full(bt_jamstd_full_t *p_btjamstd_full)	throw();
	static void		factory_domain_insert()					throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SLAY_HELPER_BTJAMSTD_HPP__  */










