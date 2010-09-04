/*! \file
    \brief Definition of the \ref router_rident_arr_t
    
*/

/* system include */
/* local include */
#include "neoip_router_rident_arr.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the pointer on router_rident_t matching this router_name_t - or NULL if none matches
 */
const router_rident_t *	router_rident_arr_t::find_by_name(const router_name_t &remote_name
					, const router_profile_t &profile)	const throw()
{
	// sanity check - the router_name_t MUST be fully_qualified
	DBG_ASSERT( remote_name.is_fully_qualified() );
	// go thru the whole router_rident_arr_t	
	for(size_t i = 0; i < size(); i++){
		const router_rident_t &	router_rident	= (*this)[i];
		// if this router_rident_t matches this peerid, return its pointer
		if( router_rident.dnsfqname(profile) == remote_name )
			return &router_rident;
	}
	// if this point is reached, no matching router_rident_t have been found, return NULL
	return NULL;	
}

/** \brief Return the pointer on router_rident_t matching this router_peerid_t - or NULL if none matches
 */
const router_rident_t *	router_rident_arr_t::find_by_peerid(const router_peerid_t &peerid) const throw()
{
	// sanity check - the router_peerid_t MUST be fully_qualified
	DBG_ASSERT( peerid.is_fully_qualified() );
	// go thru the whole router_rident_arr_tZ
	for(size_t i = 0; i < size(); i++){
		const router_rident_t &	router_rident	= (*this)[i];
		// if this router_rident_t matches this peerid, return its pointer
		if( router_rident.peerid() == peerid )	return &router_rident;
	}
	// if this point is reached, no matching router_rident_t have been found, return NULL
	return NULL;
}

/** \brief Return the router_rident_t matching the router_peerid_t
 * 
 * - WARNING: the router_peerid_t MUST be present in the router_rident_arr_t
 */
const router_rident_t &	router_rident_arr_t::get_by_peerid(const router_peerid_t &peerid)	const throw()
{
	// get the pointer on the router_rident_t
	const router_rident_t *	router_rident	= find_by_peerid(peerid);
	// sanity check - the router_rident_t MUST have been found
	DBG_ASSERT( router_rident );
	// return it
	return *router_rident;
}


NEOIP_NAMESPACE_END;






