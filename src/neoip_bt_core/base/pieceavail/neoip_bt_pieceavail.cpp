/*! \file
    \brief Definition of the \ref bt_pieceavail_t
  
- TODO replace the kludgy btformat_to_pkt by the bitfield_t from/to_datum stuff
  
*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_bt_pieceavail.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_pkt.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//          special serialization for btformat (aka dont include the length)
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Special serialization with no length
 * 
 * - used to serialize bt_pieceavail_t within the bittorrent protocol
 * - doesnt support null bt_pieceavail_t
 * - doesnt include the bt_pieceavail.size() in the serialization
 */
pkt_t &	bt_pieceavail_t::btformat_to_pkt(pkt_t &pkt)				const throw()
{
	datum_t		datum	= datum_t(nb_byte_needed());
	uint8_t*	p	= (uint8_t *)datum.get_ptr();
	// set the whole datum_t to 0
	memset(p, 0, datum.size());
	// convert the datum_t to a bt_pieceavail_t bit by bit
	for(size_t i = 0; i < nb_piece(); i++){
		if( is_avail(i) )	p[i/8]	|= 0x80 >> (i%8);
	}
	// append the datum_t data
	pkt.append( datum );
	// return the bytearray_t
	return pkt;
}

/** \brief unserialize when no length is included
 * 
 * - the bt_pieceavail MUST NOT be null. the size() MUST be already init
 * - doesnt support null bt_pieceavail_t
 * - doesnt include the bt_pieceavail.size() in the serialization
 */
bt_pieceavail_t	bt_pieceavail_t::btformat_from_pkt(pkt_t &pkt, size_t nb_piece)	throw(serial_except_t)
{
	bt_pieceavail_t	bt_pieceavail( nb_piece );
	// consume the bitfield in the bytearray_t
	datum_t		datum	= pkt.head_consume(bt_pieceavail.nb_byte_needed());
	// convert the datum_t to a bt_pieceavail_t bit by bit
	uint8_t *	p	= (uint8_t *)datum.get_ptr();
	for(size_t i = 0; i < nb_piece; i++){
		if( p[i/8] & (0x80 >> (i%8)) )	bt_pieceavail.mark_isavail(i);
	}
	// return the resulting bt_pieceavail_t
	return bt_pieceavail;
}


NEOIP_NAMESPACE_END;






