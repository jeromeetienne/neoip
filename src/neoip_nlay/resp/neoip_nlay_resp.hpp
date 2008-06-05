/*! \file
    \brief Header of the \ref nlay_resp_t

- see \ref neoip_nlay_resp.cpp
*/


#ifndef __NEOIP_NLAY_RESP_HPP__ 
#define __NEOIP_NLAY_RESP_HPP__ 
/* system include */
/* local include */
#include "neoip_nlay_err.hpp"
#include "neoip_nlay_event.hpp"
#include "neoip_nlay_type.hpp"
#include "neoip_scnx_resp.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	nlay_profile_t;
class	nlay_nego_t;

/** \brief to handle the resp in the nlay stack
 */
class nlay_resp_t : public scnx_resp_auxnego_cb_t, NEOIP_COPY_CTOR_DENY {
private:
	nlay_type_t	inner_type;	//!< the inner type of this nlay
	nlay_type_t	outter_type;	//!< the outter type of this nlay
	nlay_profile_t *nlay_profile;	//!< pointer on the profile

	// callback for scnx auxilariy negociation	
	scnx_resp_t *	scnx_resp;
	nlay_nego_t *	nlay_nego;
	scnx_err_t	neoip_scnx_resp_auxnego_cb(void *cb_userptr, scnx_resp_t &cb_scnx_resp
				, datum_t &auxnego_datum, void **auxnego_ptr)	throw();
public:
	/*************** ctor/dtor	***************************************/
	nlay_resp_t(nlay_profile_t *nlay_profile, const nlay_type_t &inner_type
					, const nlay_type_t &outter_type)	throw();
	nlay_resp_t()	throw();
	~nlay_resp_t()	throw();
	
	/*************** Setup function	***************************************/
	nlay_err_t	start()			throw();

	/*************** Query function	***************************************/
	bool		is_null()		const throw()	{ return inner_type.is_null();	}

	/*************** packet processing	*******************************/
	nlay_err_t	pkt_from_lower(pkt_t &pkt, const datum_t &pathid, nlay_event_t &nlay_event)throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NLAY_RESP_HPP__  */



