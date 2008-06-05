/*! \file
    \brief Header of the \ref slay_resp_vapi_t
*/


#ifndef __NEOIP_SLAY_RESP_VAPI_HPP__ 
#define __NEOIP_SLAY_RESP_VAPI_HPP__ 

/* system include */
/* local include */
#include "neoip_slay_resp_arg.hpp"
#include "neoip_slay_err.hpp"
#include "neoip_slay_domain.hpp"
#include "neoip_obj_factory.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	pkt_t;
class	slay_full_t;

/** \brief Definition of the virtual API for the slay_resp_t
 */
class slay_resp_vapi_t {
public:
	/*************** setup function	***************************************/
	virtual slay_err_t	start(const slay_resp_arg_t &arg)	throw() = 0;

	/*************** query function`***************************************/
	virtual	const slay_domain_t &	domain()	const throw() = 0;
	virtual const slay_profile_t &	profile()	const throw() = 0;
	virtual pkt_t &			xmit_buffer()	throw()	= 0;

	/*************** Action function	*******************************/
	virtual slay_err_t		notify_recved_data(const pkt_t &pkt_inp
						, slay_full_t **slay_full_out)	throw() = 0;
	
	/*************** display function	*******************************/
	virtual std::string		to_string()	const throw() = 0;

	// declaration for the factory
	FACTORY_BASE_CLASS_DECLARATION(slay_resp_vapi_t);
	//! virtual destructor
	virtual ~slay_resp_vapi_t() {};
};

// declaration of the factory type
FACTORY_PLANT_DECLARATION(slay_resp_factory, slay_domain_t, slay_resp_vapi_t);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SLAY_RESP_VAPI_HPP__  */



