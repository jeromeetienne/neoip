/*! \file
    \brief Header of the nlay_pktcomp_t

*/


#ifndef __NEOIP_NLAY_PKTCOMP_FULL_HPP__
#define __NEOIP_NLAY_PKTCOMP_FULL_HPP__

/* system include */
/* local include */
#include "neoip_pktcomp.hpp"
#include "neoip_nlay_full_api.hpp"
#include "neoip_nlay_nego.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief nlay glue to \ref pktcomp_t
 */
class nlay_pktcomp_t : public nlay_full_api_t {
private:
	pktcomp_t *	pktcomp;
public:
	// ctor/dtor
	nlay_pktcomp_t()	throw();
	~nlay_pktcomp_t()	throw();
	
	// function inherited from nlay_full_api_t
	void		register_handler(nlay_regpkt_t *regpkt_lower)	throw();
	nlay_err_t	set_parameter(const nlay_profile_t *nlay_profile
				, const nlay_nego_result_t *nego_result)	throw();
	nlay_err_t	pkt_from_upper(pkt_t &pkt)				throw();
	nlay_err_t	pkt_from_lower(pkt_t &pkt)				throw();
	NEOIP_NLAY_FULL_API_CTORDTOR(nlay_pktcomp_t);
};

NEOIP_NAMESPACE_END


#endif // __NEOIP_NLAY_PKTCOMP_FULL_HPP__ 



