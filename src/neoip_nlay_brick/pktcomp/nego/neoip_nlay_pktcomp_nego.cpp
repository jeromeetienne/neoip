/*! \file
    \brief Definition of the nlay_pktcomp_nego_t

*/

/* system include */
/* local include */
#include "neoip_nlay_pktcomp_nego.hpp"
#include "neoip_nlay_profile.hpp"
#include "neoip_pktcomp_pkttype.hpp"
#include "neoip_serial.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                           register function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief register a nlay_full_api_t
 */
nlay_err_t	nlay_pktcomp_nego_t::nlay_nego_register(nlay_nego_t *nlay_nego)	throw()
{
	// register the pkttype
	nlay_nego->register_req_builder(nlay_nego_pkttype_t::PKTCOMP_PAYL, this);
	nlay_nego->register_req_parser(nlay_nego_pkttype_t::PKTCOMP_PAYL, this);
	nlay_nego->register_ans_parser(nlay_nego_pkttype_t::PKTCOMP_PAYL, this);
	// return noerror
	return nlay_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                          parameter negociation
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief callback called to build a nlay_nego_t request
 */
nlay_err_t	nlay_pktcomp_nego_t::neoip_nlay_nego_req_builder_cb(nlay_nego_pkttype_t pkttype
				, datum_t &payl_out, nlay_profile_t *nlay_profile)	throw()
{
	// sanity check
	DBG_ASSERT(pkttype == nlay_nego_pkttype_t::PKTCOMP_PAYL);
	// build the outgoing payload
	payl_out	= bytearray_t().serialize(nlay_profile->pktcomp().optlist()).to_datum();
	// return no error
	return nlay_err_t::OK;
}

/** \brief callback called to parse a nlay_nego_t request and build an answer
 */
nlay_err_t	nlay_pktcomp_nego_t::neoip_nlay_nego_req_parser_cb(nlay_nego_pkttype_t pkttype
					, datum_t &payl_reqans, nlay_profile_t *nlay_profile
					, nlay_nego_result_t &nego_result)	throw()
{
	const compress_type_arr_t &	local_optlist	= nlay_profile->pktcomp().optlist();
	compress_type_arr_t		remote_optlist;
	serial_t			serial(payl_reqans.get_data(), payl_reqans.get_len());
	// log to debug
	KLOG_DBG("PKTCOMP optlist=" << nlay_profile->pktcomp().optlist());
	// sanity check
	DBG_ASSERT(pkttype == nlay_nego_pkttype_t::PKTCOMP_PAYL);
	// read the remote pktcomp_optlist	
	try {
		serial >> remote_optlist;
	}catch(serial_except_t &e){
		return nlay_err_t(nlay_err_t::BOGUS_PKT, "Cant parse pktcomp nego payload due to " + e.what());
	}
	// some logging
	KLOG_DBG("pktcomp_optlist from itor=" << remote_optlist);
	
	// try to find a common value between the local and the remote
	const compress_type_t *	chosen_compress_type = local_optlist.find_first_common(remote_optlist);
	// if no matches have been found, return an error
	if( !chosen_compress_type )
		return nlay_err_t(nlay_err_t::NEGO_FAILED, "Cant negociate a compression algorithm.");

	// copy the chosen compres_type in nego_result
	nego_result.pktcomp_compress_type	= *chosen_compress_type;

	// log to debug
	KLOG_DBG("pktcomp has negociated compression with "<< nego_result.pktcomp_compress_type);

	// build the answer
	payl_reqans	= bytearray_t().serialize(*chosen_compress_type).to_datum( datum_t::NOCOPY );

	// return no error
	return nlay_err_t::OK;
}

/** \brief called to parse a nlay_nego_t answer
 */
nlay_err_t	nlay_pktcomp_nego_t::neoip_nlay_nego_ans_parser_cb(nlay_nego_pkttype_t pkttype
				, const datum_t &payl_ans, nlay_profile_t *nlay_profile
				, nlay_nego_result_t &nego_result)		throw()
{
	const compress_type_arr_t &	local_optlist	= nlay_profile->pktcomp().optlist();
	serial_t			serial(payl_ans.get_data(), payl_ans.get_len());
	compress_type_t			chosen_compress_type;
	// sanity check
	DBG_ASSERT(pkttype == nlay_nego_pkttype_t::PKTCOMP_PAYL);
	
	// read the remote pktcomp_optlist	
	try {
		serial >> chosen_compress_type;
	}catch(serial_except_t &e){
		return nlay_err_t(nlay_err_t::BOGUS_PKT, "Cant parse pktcomp nego payload due to " + e.what());
	}
	
	// check the compress_type_t chosen by the remote has been proposed
	if( !local_optlist.contain(chosen_compress_type) )
		return nlay_err_t(nlay_err_t::NEGO_FAILED, "Cant negociate a compression algorithm.");

	// copy the chosen_compress_type in nego_result
	nego_result.pktcomp_compress_type	= chosen_compress_type;
	
	// log to debug
	KLOG_DBG("pktcomp has negociated compression with "<< chosen_compress_type);

	// return no error
	return nlay_err_t::OK;
}

NEOIP_NAMESPACE_END


