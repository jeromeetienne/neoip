/*! \file
    \brief Definition of the \ref nlay_nego_t

*/


/* system include */
/* local include */
#include "neoip_nlay_nego.hpp"
#include "neoip_nlay_pktcomp_nego.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_serial.hpp"
#include "neoip_log.hpp"
#include "neoip_pkt.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                           ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
nlay_nego_t::nlay_nego_t(const nlay_type_t &inner_type, const nlay_type_t &outter_type)	throw()
{
	// create the nlay_pktcomp_nego
	pktcomp_nego	= nipmem_new nlay_pktcomp_nego_t();
	// register the nlay_pktcomp_nego
	nlay_err_t	nlay_err;
	nlay_err	= pktcomp_nego->nlay_nego_register(this);
	DBG_ASSERT( nlay_err.succeed() );	// it has no reason to fail, but test just in case
}

/** \brief Destructor
 */
nlay_nego_t::~nlay_nego_t()	throw()
{
	nipmem_zdelete pktcomp_nego;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                           processing function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief build a request
 */
nlay_err_t	nlay_nego_t::build_request(datum_t &payl_req, nlay_profile_t *nlay_profile)	throw()
{
	std::map<nlay_nego_pkttype_t, nlay_nego_req_builder_cb_t *>::iterator	iter;
	nlay_err_t	nlay_err;
	datum_t		datum;
	bytearray_t	bytearray;
	// scan over all builder
	for( iter = req_builder_db.begin(); iter != req_builder_db.end(); iter++ ){
		nlay_nego_pkttype_t		pkttype	= (*iter).first;
		nlay_nego_req_builder_cb_t *	callback= (*iter).second;
		// reset the datum
		datum		= datum_t();
		// build the datum of this builder for the request
		nlay_err	= callback->neoip_nlay_nego_req_builder_cb(pkttype, datum, nlay_profile);
		if( !nlay_err.succeed() )	return nlay_err;
		// build the payload 
		bytearray	= bytearray_t().serialize(pkttype).serialize(datum);
		// log to debug
		KLOG_DBG("pkttype=" << pkttype << " datum=" << bytearray.to_datum());
		// add the resulting datum_t to the main one
		payl_req	+= bytearray.to_datum(datum_t::NOCOPY);
	}
	// return noerror
	return nlay_err_t::OK;
}

/** \brief parse a request
 */
nlay_err_t	nlay_nego_t::parse_request(datum_t &payl_reqans, nlay_profile_t *nlay_profile
					, nlay_nego_result_t &nego_result)	throw()
{
	std::map<nlay_nego_pkttype_t, nlay_nego_req_parser_cb_t *>::iterator	iter;
	nlay_nego_pkttype_t	pkttype;
	pkt_t		pkt_in(payl_reqans);
	datum_t		datum, payl_ans;
	nlay_err_t	nlay_err;
	bytearray_t	bytearray;


	// try/catch to handle the parsing errors
	try {
		// loop until pkt_in is empty
		while( pkt_in.get_len() ){
			// read the packet type
			pkt_in	>> pkttype;
			// find the matching callback if present
			iter	= req_parser_db.find(pkttype);
			// if not found, return an error
			if( iter == req_parser_db.end() )
				return nlay_err_t(nlay_err_t::BOGUS_PKT, "Unknown pkttype");
			// notify the matching callback with the pkttype and the following datum_t
			pkt_in		>> datum;
			nlay_err	= iter->second->neoip_nlay_nego_req_parser_cb(pkttype, datum
									, nlay_profile, nego_result);
			// return error if the callback failed
			if( !nlay_err.succeed() )	return nlay_err;
			// build the payload 
			bytearray	= bytearray_t().serialize(pkttype).serialize(datum);	
			// log to debug
			KLOG_DBG("pkttype=" << pkttype << " datum=" << bytearray.to_datum());
			// add the resulting datum_t to the main one
			payl_ans	+= bytearray.to_datum(datum_t::NOCOPY);
		}
	}catch(serial_except_t &e){
		return nlay_err_t(nlay_err_t::BOGUS_PKT, "Cant parse packet due to " + e.what());
	}

	// copy the payl_ans
	payl_reqans = payl_ans;
	// return noerror
	return nlay_err_t::OK;
}

/** \brief parse a answer
 */
nlay_err_t	nlay_nego_t::parse_answer(const datum_t &payl_ans, nlay_profile_t *nlay_profile
						, nlay_nego_result_t &nego_result)	throw()
{
	std::map<nlay_nego_pkttype_t, nlay_nego_ans_parser_cb_t *>::iterator	iter;
	nlay_nego_pkttype_t	pkttype;
	pkt_t		pkt_in(payl_ans);
	nlay_err_t	nlay_err;
	datum_t		datum;

	// try/catch to handle the parsing errors
	try {
		// loop until pkt_in is empty
		while( pkt_in.get_len() ){
			// read the packet type
			pkt_in	>> pkttype;
			// read the associated datum
			pkt_in	>> datum;
			// find the matching callback if present
			iter	= ans_parser_db.find(pkttype);
			// if not found, return an error
			if( iter == ans_parser_db.end() )
				return nlay_err_t(nlay_err_t::BOGUS_PKT, "Unknown pkttype");
			// notify the matching callback with the pkttype and the following datum_t
			nlay_err= iter->second->neoip_nlay_nego_ans_parser_cb(pkttype, datum
								, nlay_profile, nego_result);
			// return error if the callback failed
			if( !nlay_err.succeed() )	return nlay_err;
		}
	}catch(serial_except_t &e){
		return nlay_err_t(nlay_err_t::BOGUS_PKT, "Cant parse packet due to " + e.what());
	}
	// return no error
	return nlay_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                           register callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief register a req_builder callback
 */
void nlay_nego_t::register_req_builder(nlay_nego_pkttype_t pkttype
				, nlay_nego_req_builder_cb_t *callback)	throw()
{
	bool	succeed = req_builder_db.insert(std::make_pair(pkttype, callback)).second;
	DBG_ASSERT( succeed );
}

/** \brief register a req_parser callback
 */
void nlay_nego_t::register_req_parser(nlay_nego_pkttype_t pkttype
				, nlay_nego_req_parser_cb_t *callback)	throw()
{
	bool	succeed = req_parser_db.insert(std::make_pair(pkttype, callback)).second;
	DBG_ASSERT( succeed );
}

/** \brief register a ans_parser callback
 */
void nlay_nego_t::register_ans_parser(nlay_nego_pkttype_t pkttype
				, nlay_nego_ans_parser_cb_t *callback)	throw()
{
	bool	succeed = ans_parser_db.insert(std::make_pair(pkttype, callback)).second;
	DBG_ASSERT( succeed );
}

NEOIP_NAMESPACE_END

