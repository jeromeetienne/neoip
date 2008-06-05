/*! \file
    \brief Header of the nlay_pktfrag_t

*/


#ifndef __NEOIP_NLAY_PKTFRAG_HPP__
#define __NEOIP_NLAY_PKTFRAG_HPP__

/* system include */
#include <list>
/* local include */
#include "neoip_pktfrag.hpp"
#include "neoip_nlay_nego.hpp"
#include "neoip_nlay_full_api.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

class nlay_pktfrag_t : public nlay_full_api_t {
private:
	pktfrag_t	pktfrag;
public:
	nlay_pktfrag_t()	throw();
	~nlay_pktfrag_t()	throw();
	
	// function inherited from nlay_full_api_t
	void		register_handler(nlay_regpkt_t *regpkt_lower)		throw();
	nlay_err_t	set_parameter(const nlay_profile_t *nlay_profile
				, const nlay_nego_result_t *nego_result)	throw();
	nlay_err_t	pkt_from_upper(pkt_t &pkt)				throw();
	nlay_err_t	pkt_from_lower(pkt_t &pkt)				throw();
	nlay_err_t	set_outter_mtu(size_t outter_mtu)			throw();
	NEOIP_NLAY_FULL_API_CTORDTOR(nlay_pktfrag_t);
};

NEOIP_NAMESPACE_END


#endif // __NEOIP_NLAY_PKTFRAG_HPP__ 


