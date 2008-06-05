/*! \file
    \brief Header of the \ref nlay_nego_t

- see \ref neoip_nlay_nego.cpp
*/


#ifndef __NEOIP_NLAY_NEGO_HPP__ 
#define __NEOIP_NLAY_NEGO_HPP__ 
/* system include */
#include <map>
/* local include */
#include "neoip_nlay_nego_pkttype.hpp"
#include "neoip_nlay_nego_result.hpp"
#include "neoip_nlay_nego_cb.hpp"
#include "neoip_nlay_err.hpp"
#include "neoip_datum.hpp"
#include "neoip_nlay_type.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	nlay_pktcomp_nego_t;

/** \brief to handle the parameter's negociation of the nlay bricks between the 2 peers
 * 
 * - used by nlay_itor_t and nlay_resp_t
 */
class nlay_nego_t : NEOIP_COPY_CTOR_DENY {
private:
	nlay_pktcomp_nego_t *	pktcomp_nego;	//!< context for the pktcomp negociation

	/************** database of pkttype+associated callback	***************/
	std::map<nlay_nego_pkttype_t, nlay_nego_req_builder_cb_t *>	req_builder_db;
	std::map<nlay_nego_pkttype_t, nlay_nego_req_parser_cb_t *>	req_parser_db;
	std::map<nlay_nego_pkttype_t, nlay_nego_ans_parser_cb_t *>	ans_parser_db;
public:
	/*************** ctor/dtor	***************************************/
	nlay_nego_t(const nlay_type_t &inner_type, const nlay_type_t &outter_type)	throw();
	~nlay_nego_t()									throw();

	/*************** register function	*******************************/
	void register_req_builder(nlay_nego_pkttype_t pkttype, nlay_nego_req_builder_cb_t *callback)
										throw();
	void register_req_parser(nlay_nego_pkttype_t pkttype, nlay_nego_req_parser_cb_t *callback)
										throw();
	void register_ans_parser(nlay_nego_pkttype_t pkttype, nlay_nego_ans_parser_cb_t *callback)
										throw();

	/*************** processing function	*******************************/
	nlay_err_t	build_request(datum_t &payl_req, nlay_profile_t *nlay_profile)	throw();
	nlay_err_t	parse_request(datum_t &payl_reqans, nlay_profile_t *nlay_profile
						, nlay_nego_result_t &nego_result)	throw();
	nlay_err_t	parse_answer(const datum_t &payl_ans, nlay_profile_t *nlay_profile
						, nlay_nego_result_t &nego_result)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NLAY_NEGO_HPP__  */



