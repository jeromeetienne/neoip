/*! \file
    \brief Header of the \ref nlay_nego_t

- see \ref neoip_nlay_nego.cpp
*/


#ifndef __NEOIP_NLAY_NEGO_RESULT_HPP__ 
#define __NEOIP_NLAY_NEGO_RESULT_HPP__ 
/* system include */
/* local include */
#include "neoip_compress_type.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief This class contains the result of the parameters negociation between 
 *         the 2 peers.
 */
class nlay_nego_result_t : NEOIP_COPY_CTOR_ALLOW {
public:
	compress_type_t	pktcomp_compress_type;	//!< the compress_type_t for \ref pktcomp_t
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NLAY_NEGO_RESULT_HPP__  */



