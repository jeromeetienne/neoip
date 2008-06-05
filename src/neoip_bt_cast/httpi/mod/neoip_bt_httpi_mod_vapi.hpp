/*! \file
    \brief Declaration of the bt_httpi_mod_vapi_t

\par Brief Description
a \ref bt_httpi_mod_vapi_t is a object which handling the moding of previously
downloaded files.
    
*/

#ifndef __NEOIP_BT_HTTPI_MOD_VAPI_HPP__ 
#define __NEOIP_BT_HTTPI_MOD_VAPI_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_httpi_mod_type_t;
class	bt_cast_spos_t;
class	bt_err_t;
class	datum_t;

/** \brief Virtual API for the bt_httpi_mod_t stuff
 * 
 * - various hook attached to bt_httpo_full_t depending on how they should 
 *   be processed.
 */
class bt_httpi_mod_vapi_t {
public:
	//! return the bt_httpi_mod_type_t for this bt_httpi_mod_vapi_t
	virtual const bt_httpi_mod_type_t & type()		const throw() = 0;
	//! notify new data to the this bt_httpi_mod_vapi_t
	virtual void		notify_data(const datum_t &data)throw() = 0;
	//! pop up a allowed start position - or null one if none is available
	virtual bt_cast_spos_t	cast_spos_pop()			throw() = 0;
	// return a datum_t containing prefix_header
	virtual datum_t		prefix_header()			const throw() = 0;
	// virtual destructor
	virtual ~bt_httpi_mod_vapi_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HTTPI_MOD_VAPI_HPP__  */



