/*! \file
    \brief Declaration of the oload_mod_vapi_t

\par Brief Description
a \ref oload_mod_vapi_t is a object which handling the moding of previously
downloaded files.
    
*/

#ifndef __NEOIP_OLOAD_MOD_VAPI_HPP__ 
#define __NEOIP_OLOAD_MOD_VAPI_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	oload_mod_type_t;
class	oload_swarm_t;
class	bt_httpo_full_t;
class	bt_err_t;
class	datum_t;

/** \brief Virtual API for the oload_mod_t stuff
 * 
 * - various hook attached to bt_httpo_full_t depending on how they should 
 *   be processed.
 */
class oload_mod_vapi_t {
public:
	// return the oload_mod_type_t for this oload_mod_vapi_t
	virtual const oload_mod_type_t &	type()				const throw() = 0;
	//! hook called before launch oload_itor_t 
	//! - typically used to rewrite bt_thttpo_full_t http_reqhd
	//! - if return a error, the bt_httpo_full_t processing MUST be stopped
	virtual bt_err_t	pre_itor_hook(bt_httpo_full_t *httpo_full)	const throw() = 0;
	// return a datum_t containing prefix_header for this httpo_full_t on this oload_swarm_t
	virtual datum_t		prefix_header(const oload_swarm_t *oload_swarm
					, const bt_httpo_full_t *httpo_full)	const throw() = 0;
	// virtual destructor
	virtual ~oload_mod_vapi_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_OLOAD_MOD_VAPI_HPP__  */



