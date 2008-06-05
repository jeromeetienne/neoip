/*! \file
    \brief Header of the \ref nlay_full_api_t
*/


#ifndef __NEOIP_NLAY_SCNX_FULL_API_HPP__ 
#define __NEOIP_NLAY_SCNX_FULL_API_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration

/** \brief The virtual API for nlay_scnx_full_t
 */
class nlay_scnx_full_api_t {
public:
	//! return the local_idname
	virtual	const std::string &	get_local_idname()	const throw() = 0;
	//! return the local_idname
	virtual	const std::string &	get_remote_idname()	const throw() = 0;
	//! virtual destructor
	virtual ~nlay_scnx_full_api_t() {};
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NLAY_SCNX_FULL_API_HPP__  */



