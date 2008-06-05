/*! \file
    \brief Definition of the \ref router_rootca_arr_t
    
*/

/* system include */
/* local include */
#include "neoip_router_rootca_arr.hpp"
#include "neoip_router_name.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the pointer on router_rootca_t matching the dnsname domain - or NULL if none matches
 */
const router_rootca_t *	router_rootca_arr_t::find_by_dnsname(const router_name_t &dnsname)	const throw()
{
	// sanity check - the dnsname MUST be fully_qualified
	DBG_ASSERT( dnsname.is_fully_qualified() );
	// go thru the whole router_rootca_arr_t	
	for(size_t i = 0; i < size(); i++){
		const router_rootca_t &	router_rootca	= (*this)[i];
		// if this router_rootca_t matches this dnsname, return its pointer
		if( router_rootca.match_dnsname(dnsname) )	return &router_rootca;
	}
	// if this point is reached, no matching router_rootca_t have been found, return NULL
	return NULL;
}

NEOIP_NAMESPACE_END;






