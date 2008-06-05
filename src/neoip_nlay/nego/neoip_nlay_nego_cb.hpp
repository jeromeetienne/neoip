/*! \file
    \brief callback for the \ref nlay_nego_t

*/


#ifndef __NEOIP_NLAY_NEGO_CB_HPP__ 
#define __NEOIP_NLAY_NEGO_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_nlay_nego_pkttype.hpp"
#include "neoip_nlay_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class nlay_nego_t;
class nlay_nego_result_t;
class nlay_profile_t;
class datum_t;

class nlay_nego_api_t {
public:	
	/** \brief callback called to build a nlay_nego_t request
	 */
	virtual nlay_err_t	nlay_nego_register(nlay_nego_t *nlay_nego)	throw() = 0;
	virtual ~nlay_nego_api_t() {};
};

class nlay_nego_req_builder_cb_t {
public:	
	/** \brief callback called to build a nlay_nego_t request
	 */
	virtual nlay_err_t neoip_nlay_nego_req_builder_cb(nlay_nego_pkttype_t pkttype
				, datum_t &payl_out, nlay_profile_t *nlay_profile)	throw() = 0;	
	virtual ~nlay_nego_req_builder_cb_t() {};
};

class nlay_nego_req_parser_cb_t {
public:	
	/** \brief callback called to parse a nlay_nego_t request and build an answer
	 */
	virtual nlay_err_t neoip_nlay_nego_req_parser_cb(nlay_nego_pkttype_t pkttype
					, datum_t &payl_reqans, nlay_profile_t *nlay_profile
					, nlay_nego_result_t &nego_result)	throw() = 0;	
	virtual ~nlay_nego_req_parser_cb_t() {};
};

class nlay_nego_ans_parser_cb_t {
public:	
	/** \brief callback called to parse a nlay_nego_t answer
	 */
	virtual nlay_err_t neoip_nlay_nego_ans_parser_cb(nlay_nego_pkttype_t pkttype
					, const datum_t &payl_ans, nlay_profile_t *nlay_profile
					, nlay_nego_result_t &nego_result)	throw() = 0;	
	virtual ~nlay_nego_ans_parser_cb_t() {};
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NLAY_NEGO_CB_HPP__  */



