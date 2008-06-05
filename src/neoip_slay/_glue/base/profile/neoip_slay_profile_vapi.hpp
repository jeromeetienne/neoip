/*! \file
    \brief Header of the \ref slay_profile_vapi_t
*/


#ifndef __NEOIP_SLAY_PROFILE_VAPI_HPP__ 
#define __NEOIP_SLAY_PROFILE_VAPI_HPP__ 

/* system include */
/* local include */
#include "neoip_slay_err.hpp"
#include "neoip_slay_domain.hpp"
#include "neoip_obj_factory.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief Definition of the virtual API for the slay_profile_t
 */
class slay_profile_vapi_t {
public:
	/*************** query function`***************************************/
	virtual slay_err_t		check()		const throw() = 0;
	virtual	const slay_domain_t &	domain()	const throw() = 0;

	// declaration for the factory
	FACTORY_BASE_CLASS_DECLARATION(slay_profile_vapi_t);
	//! virtual destructor
	virtual ~slay_profile_vapi_t() {};
};

// declaration of the factory type
FACTORY_PLANT_DECLARATION(slay_profile_factory, slay_domain_t, slay_profile_vapi_t);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SLAY_PROFILE_VAPI_HPP__  */



