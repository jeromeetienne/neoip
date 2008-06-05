/*! \file
    \brief Definition of the nlay_dataize_t

\par Implementation Notes
- this layer is specific to nlay_full_t so it isntcoded externally to nlay_full_t

*/

/* system include */
/* local include */
#include "neoip_nlay_dataize.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

NEOIP_STRTYPE_DEFINITION_START(dataize_pkttype_t, uint8_t)
NEOIP_STRTYPE_DEFINITION_ITEM(dataize_pkttype_t	, DATAIZE	, "DATAIZE")
NEOIP_STRTYPE_DEFINITION_END(dataize_pkttype_t)


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                           pkt_from_upper/lower
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief register a nlay_full_api_t
 */
void	nlay_dataize_t::register_handler(nlay_regpkt_t *regpkt_lower)	throw()
{
	dataize_pkttype_t	pkttype;
	pkttype	= dataize_pkttype_t::DATAIZE;
	regpkt_lower->register_handler(pkttype.get_value(), this);
}


/** \brief return the space overhead due to this brick in the mtu
 */
size_t		nlay_dataize_t::get_mtu_overhead()				const throw()
{
	serial_t	serial;
	serial << dataize_pkttype_t(dataize_pkttype_t::DATAIZE);
	return serial.get_len();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//              packet processing for full connection
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief handle packet from the upper network layer for full connection
 */
nlay_err_t	nlay_dataize_t::pkt_from_upper(pkt_t &pkt)		throw()
{
	// prepend the pkttype
	// - all this dirty code because i need to prepend a serial, maybe to do a function for it ? :)
	serial_t	serial;
	serial << dataize_pkttype_t(dataize_pkttype_t::DATAIZE);
	pkt.prepend( serial.get_data(), serial.get_len() );
	// return no error
	return nlay_err_t::OK;
}

/** \brief handle packet from the lower network layer for full connection
 */
nlay_err_t	nlay_dataize_t::pkt_from_lower(pkt_t &pkt)		throw()
{
	dataize_pkttype_t	pkttype;
	try {	// read the pkttype
		pkt >> pkttype;
	}catch(serial_except_t &e){
		return nlay_err_t(nlay_err_t::BOGUS_PKT, "Cant read the packet type due to " + e.what());
	}
	DBG_ASSERT( pkttype == dataize_pkttype_t::DATAIZE);
	return nlay_err_t::OK;
}

NEOIP_NAMESPACE_END


