/*! \file
    \brief Header of the nlay_pktcomp_nego_t

*/


#ifndef __NEOIP_NLAY_PKTCOMP_NEGO_HPP__
#define __NEOIP_NLAY_PKTCOMP_NEGO_HPP__

/* system include */
/* local include */
#include "neoip_nlay_nego.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief nlay_nego_t glue to \ref pktcomp_t
 */
class nlay_pktcomp_nego_t : private nlay_nego_api_t
				, private nlay_nego_req_builder_cb_t
				, private nlay_nego_req_parser_cb_t
				, private nlay_nego_ans_parser_cb_t {
private:
	// parameter negociation callback
	nlay_err_t	neoip_nlay_nego_req_builder_cb(nlay_nego_pkttype_t pkttype, datum_t &payl_out
					, nlay_profile_t *nlay_profile)		throw();
	nlay_err_t	neoip_nlay_nego_req_parser_cb(nlay_nego_pkttype_t pkttype, datum_t &payl_reqans
					, nlay_profile_t *nlay_profile
					, nlay_nego_result_t &nego_result)	throw();
	nlay_err_t	neoip_nlay_nego_ans_parser_cb(nlay_nego_pkttype_t pkttype, const datum_t &payl_ans
					, nlay_profile_t *nlay_profile
					, nlay_nego_result_t &nego_result)	throw();
public:
	// function inherited from nlay_nego_api_t
	nlay_err_t	nlay_nego_register(nlay_nego_t *nlay_nego)		throw();
};

NEOIP_NAMESPACE_END


#endif // __NEOIP_NLAY_PKTCOMP_NEGO_HPP__ 



