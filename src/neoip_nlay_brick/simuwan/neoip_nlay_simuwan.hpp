/*! \file
    \brief Header of the nlay_simuwan_t

*/


#ifndef __NEOIP_NLAY_SIMUWAN_HPP__
#define __NEOIP_NLAY_SIMUWAN_HPP__

/* system include */
/* local include */
#include "neoip_simuwan.hpp"
#include "neoip_nlay_nego.hpp"
#include "neoip_nlay_full_api.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

class nlay_simuwan_t : private simuwan_cb_t, public nlay_full_api_t {
private:
	simuwan_t	simuwan;
	
	bool		neoip_simuwan_event_cb(void *cb_userptr, simuwan_t &cb_simuwan
					, const simuwan_event_t &simuwan_event ) throw();
public:
	nlay_simuwan_t()	throw();
	~nlay_simuwan_t()	throw();
	
	// function inherited from nlay_full_api_t
	nlay_err_t	set_parameter(const nlay_profile_t *nlay_profile
				, const nlay_nego_result_t *nego_result)	throw();
	nlay_err_t	start()					throw();
	nlay_err_t	pkt_from_upper(pkt_t &pkt)		throw();
	NEOIP_NLAY_FULL_API_CTORDTOR(nlay_simuwan_t);
};

NEOIP_NAMESPACE_END


#endif // __NEOIP_NLAY_SIMUWAN_HPP__ 


