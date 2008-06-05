/*! \file
    \brief Header of the \ref nlay_full_api_t
*/


#ifndef __NEOIP_NLAY_FULL_UPAPI_HPP__ 
#define __NEOIP_NLAY_FULL_UPAPI_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	nlay_scnx_full_api_t;

/** \brief The virtual upper API for nlay_full_t
 * 
 * - TODO find a better name "upapi" is crap
 *   - to call this one nlay_full_api_t and rename the other nlay_full_brick_api_t ?
 *   - seems much better
 */
class nlay_full_upapi_t {
public:
	/*************** access to nlay_scnx_full_api_t	***********************/
	virtual	const nlay_scnx_full_api_t &	scnx()	const throw()	= 0;
	virtual	nlay_scnx_full_api_t &		scnx()	throw()		= 0;
	
	//! virtual destructor
	virtual ~nlay_full_upapi_t() {};
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NLAY_FULL_UPAPI_HPP__  */



