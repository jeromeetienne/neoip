/*! \file
    \brief Definition of the nlay_pktcomp_t
    
*/

/* system include */
/* local include */
#include "neoip_nlay_pktcomp.hpp"
#include "neoip_nlay_profile.hpp"
#include "neoip_pktcomp_pkttype.hpp"
#include "neoip_serial.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                          ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
nlay_pktcomp_t::nlay_pktcomp_t()	throw()
{
	// zero some field
	pktcomp	= NULL;
}

/** \brief Destructor
 */
nlay_pktcomp_t::~nlay_pktcomp_t()	throw()
{
	nipmem_zdelete	pktcomp;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                           nlay_full_api_t functions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief register a nlay_full_api_t
 */
void	nlay_pktcomp_t::register_handler(nlay_regpkt_t *regpkt_lower)	throw()
{
	pktcomp_pkttype_t	pkttype;
	// register a packet type for full connection
	pkttype	= pktcomp_pkttype_t::COMPRESSED_PKT;
	regpkt_lower->register_handler(pkttype.get_value(), this);
}


/** \brief Start the operation for this brick
 */
nlay_err_t	nlay_pktcomp_t::set_parameter(const nlay_profile_t *nlay_profile
					, const nlay_nego_result_t *nego_result)	throw()
{
	// init the compression based on the connection negociation
	pktcomp	= nipmem_new pktcomp_t(nego_result->pktcomp_compress_type);
	// return no error
	return nlay_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//              packet processing for full connection
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief handle packet from the upper network layer for full connection
 */
nlay_err_t	nlay_pktcomp_t::pkt_from_upper(pkt_t &pkt)		throw()
{
	pktcomp_err_t	pktcomp_err	= pktcomp->pkt_from_upper(pkt);
	// handle the error
	if( !pktcomp_err.succeed() ){
		// special case for WOULD_EXPAND
		// - if the compression would increase the size, just return the packet asis
		if( pktcomp_err == pktcomp_err_t::WOULD_EXPAND )	return nlay_err_t::OK;	
		// else convert and report it
		return nlay_err_from_pktcomp(pktcomp_err);
	}
	// return no error
	return nlay_err_t::OK;
}

/** \brief handle packet from the lower network layer for full connection
 */
nlay_err_t	nlay_pktcomp_t::pkt_from_lower(pkt_t &pkt)		throw()
{
	pktcomp_err_t	pktcomp_err;
	// process the packet
	pktcomp_err	= pktcomp->pkt_from_lower(pkt);
	if( !pktcomp_err.succeed() )	return nlay_err_from_pktcomp(pktcomp_err);
	// return no error
	return nlay_err_t::OK;
}



NEOIP_NAMESPACE_END


