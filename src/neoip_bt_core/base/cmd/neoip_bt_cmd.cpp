/*! \file
    \brief Definition of the \ref bt_cmd_t

*/

/* system include */
#include <iomanip>
/* local include */
#include "neoip_bt_cmd.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_pkt.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                              ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
bt_cmd_t::bt_cmd_t()	throw()
{
	// zero some field
	piece_idx_val	= 0;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     main compare function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief main compare function - retuning result ala memcmp
 */
int	bt_cmd_t::compare(const bt_cmd_t &other)		const throw()
{		
	// compare the cmdtype
	if( cmdtype() < other.cmdtype() )		return -1;
	if( cmdtype() > other.cmdtype() )		return +1;
	
	// compare the piece_idx
	if( cmdtype() == bt_cmdtype_t::PIECE_ISAVAIL ){
		if( piece_idx() < other.piece_idx() )	return -1;
		if( piece_idx() > other.piece_idx() )	return +1;
	}
	
	// compare the bt_prange_t
	if( cmdtype() == bt_cmdtype_t::BLOCK_REQ|| cmdtype() == bt_cmdtype_t::BLOCK_REP
						|| cmdtype() == bt_cmdtype_t::BLOCK_DEL){
		if( prange() < other.prange() )		return -1;
		if( prange() > other.prange() )		return +1;
	}

	// compare the bt_prange_t
	if( cmdtype() == bt_cmdtype_t::UTMSG_PAYL ){
		if(utmsg_datum() < other.utmsg_datum())	return -1;
		if(utmsg_datum() > other.utmsg_datum())	return +1;
	}
	
	// NOTE: both are considered equal
	return 0;
}

	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//               static builder
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a bt_cmd_t of bt_cmdtype_t::KEEPALIVE
 */
bt_cmd_t	bt_cmd_t::build_keepalive()			throw()
{
	bt_cmd_t	bt_cmd;
	// build the bt_cmd
	bt_cmd.cmdtype_val	= bt_cmdtype_t::KEEPALIVE;
	// return the just built bt_cmd;
	return bt_cmd;
}

/** \brief Build a bt_cmd_t of bt_cmdtype_t::UNAUTH_REQ
 */
bt_cmd_t	bt_cmd_t::build_unauth_req()			throw()
{
	bt_cmd_t	bt_cmd;
	// build the bt_cmd
	bt_cmd.cmdtype_val	= bt_cmdtype_t::UNAUTH_REQ;
	// return the just built bt_cmd;
	return bt_cmd;
}

/** \brief Build a bt_cmd_t of bt_cmdtype_t::DOAUTH_REQ
 */
bt_cmd_t	bt_cmd_t::build_doauth_req()			throw()
{
	bt_cmd_t	bt_cmd;
	// build the bt_cmd
	bt_cmd.cmdtype_val	= bt_cmdtype_t::DOAUTH_REQ;
	// return the just built bt_cmd;
	return bt_cmd;
}

/** \brief Build a bt_cmd_t of bt_cmdtype_t::DOWANT_REQ
 */
bt_cmd_t	bt_cmd_t::build_dowant_req()			throw()
{
	bt_cmd_t	bt_cmd;
	// build the bt_cmd
	bt_cmd.cmdtype_val	= bt_cmdtype_t::DOWANT_REQ;
	// return the just built bt_cmd;
	return bt_cmd;
}

/** \brief Build a bt_cmd_t of bt_cmdtype_t::UNWANT_REQ
 */
bt_cmd_t	bt_cmd_t::build_unwant_req()			throw()
{
	bt_cmd_t	bt_cmd;
	// build the bt_cmd
	bt_cmd.cmdtype_val	= bt_cmdtype_t::UNWANT_REQ;
	// return the just built bt_cmd;
	return bt_cmd;
}


/** \brief Build a bt_cmd_t of bt_cmdtype_t::PIECE_ISAVAIL
 */
bt_cmd_t	bt_cmd_t::build_piece_isavail(size_t piece_idx)			throw()
{
	bt_cmd_t	bt_cmd;
	// build the bt_cmd
	bt_cmd.cmdtype_val	= bt_cmdtype_t::PIECE_ISAVAIL;
	bt_cmd.piece_idx_val	= piece_idx;
	// return the just built bt_cmd;
	return bt_cmd;
}

/** \brief Build a bt_cmd_t of bt_cmdtype_t::PIECE_BFIELD
 * 
 * - the actual content of the piece_bfield is built on the fly later
 */
bt_cmd_t	bt_cmd_t::build_piece_bfield()			throw()
{
	bt_cmd_t	bt_cmd;
	// build the bt_cmd
	bt_cmd.cmdtype_val	= bt_cmdtype_t::PIECE_BFIELD;
	// return the just built bt_cmd;
	return bt_cmd;
}

/** \brief Build a bt_cmd_t of bt_cmdtype_t::BLOCK_REQ
 */
bt_cmd_t	bt_cmd_t::build_block_req(const bt_prange_t &bt_prange)		throw()
{
	bt_cmd_t	bt_cmd;
	// build the bt_cmd
	bt_cmd.cmdtype_val	= bt_cmdtype_t::BLOCK_REQ;
	bt_cmd.prange_val	= bt_prange;
	// return the just built bt_cmd;
	return bt_cmd;
}

/** \brief Build a bt_cmd_t of bt_cmdtype_t::BLOCK_REP
 */
bt_cmd_t	bt_cmd_t::build_block_rep(const bt_prange_t &bt_prange)		throw()
{
	bt_cmd_t	bt_cmd;
	// build the bt_cmd
	bt_cmd.cmdtype_val	= bt_cmdtype_t::BLOCK_REP;
	bt_cmd.prange_val	= bt_prange;
	// return the just built bt_cmd;
	return bt_cmd;
}

/** \brief Build a bt_cmd_t of bt_cmdtype_t::BLOCK_DEL
 */
bt_cmd_t	bt_cmd_t::build_block_del(const bt_prange_t &bt_prange)		throw()
{
	bt_cmd_t	bt_cmd;
	// build the bt_cmd
	bt_cmd.cmdtype_val	= bt_cmdtype_t::BLOCK_DEL;
	bt_cmd.prange_val	= bt_prange;
	// return the just built bt_cmd;
	return bt_cmd;
}

/** \brief Build a bt_cmd_t of bt_cmdtype_t::UTMSG_PAYL
 */
bt_cmd_t	bt_cmd_t::build_utmsg_payl(const datum_t &utmsg_datum)		throw()
{
	bt_cmd_t	bt_cmd;
	// build the bt_cmd
	bt_cmd.cmdtype_val	= bt_cmdtype_t::UTMSG_PAYL;
	bt_cmd.utmsg_datum_val	= utmsg_datum;
	// return the just built bt_cmd;
	return bt_cmd;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  convertion to datum
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the length of this bt_cmd_t encoded as datum_t
 * 
 * - compute the length without actually having the data
 *   - important as the data may be coming from the disk
 */
size_t	bt_cmd_t::get_datum_len(const bt_swarm_t &bt_swarm)	const throw()
{
	// sanity check - the bt_cmd_t MUST NOT be null
	DBG_ASSERT( !is_null() );
	// handle it depending on the bt_cmdtype_t
	switch( cmdtype_val.get_value() ){
	case bt_cmdtype_t::KEEPALIVE:		return sizeof(uint32_t);
	case bt_cmdtype_t::UNAUTH_REQ:		return sizeof(uint32_t) + sizeof(uint8_t);
	case bt_cmdtype_t::DOAUTH_REQ:		return sizeof(uint32_t) + sizeof(uint8_t);
	case bt_cmdtype_t::DOWANT_REQ:		return sizeof(uint32_t) + sizeof(uint8_t);
	case bt_cmdtype_t::UNWANT_REQ:		return sizeof(uint32_t) + sizeof(uint8_t);
	case bt_cmdtype_t::PIECE_ISAVAIL:	return sizeof(uint32_t) + sizeof(uint8_t)
								+ sizeof(uint32_t);
	case bt_cmdtype_t::PIECE_BFIELD:	return sizeof(uint32_t) + sizeof(uint8_t)
								+ bt_swarm.local_pavail().nb_byte_needed();
	case bt_cmdtype_t::BLOCK_REQ:		return sizeof(uint32_t) + sizeof(uint8_t)
								+ sizeof(uint32_t)
								+ sizeof(uint32_t)
								+ sizeof(uint32_t);
	case bt_cmdtype_t::BLOCK_REP:		return sizeof(uint32_t) + sizeof(uint8_t)
								+ sizeof(uint32_t)
								+ sizeof(uint32_t)
								+ prange().length();
	case bt_cmdtype_t::BLOCK_DEL:		return sizeof(uint32_t) + sizeof(uint8_t)
								+ sizeof(uint32_t)
								+ sizeof(uint32_t)
								+ sizeof(uint32_t);
	case bt_cmdtype_t::UTMSG_PAYL:		return sizeof(uint32_t) + sizeof(uint8_t)
								+ utmsg_datum().length();
	default:	break;
	}
	// NOTE: this point MUST NEVER be reached
	DBG_ASSERT( 0 );
	return 0;
}

/** \brief Convert the bt_cmd_t to a datum_t
 */
datum_t	bt_cmd_t::to_datum(const bt_swarm_t &bt_swarm)		const throw()
{
	pkt_t	pkt;
	// sanity check - the bt_cmd_t MUST NOT be null
	DBG_ASSERT( !is_null() );
	// handle it depending on the bt_cmdtype_t
	switch( cmdtype_val.get_value() ){
	case bt_cmdtype_t::KEEPALIVE:		pkt << uint32_t(0);	// length of the payload
						break;
	case bt_cmdtype_t::UNAUTH_REQ:		pkt << uint32_t(1);	// length of the payload
						pkt << cmdtype_val;	// command type
						break;
	case bt_cmdtype_t::DOAUTH_REQ:		pkt << uint32_t(1);	// length of the payload
						pkt << cmdtype_val;	// command type
						break;
	case bt_cmdtype_t::DOWANT_REQ:		pkt << uint32_t(1);	// length of the payload
						pkt << cmdtype_val;	// command type
						break;
	case bt_cmdtype_t::UNWANT_REQ:		pkt << uint32_t(1);	// length of the payload
						pkt << cmdtype_val;	// command type
						break;
	case bt_cmdtype_t::PIECE_ISAVAIL:	pkt << uint32_t(5);	// length of the payload
						pkt << cmdtype_val;	// command type
						pkt << uint32_t(piece_idx());
						break;
	case bt_cmdtype_t::PIECE_BFIELD:	pkt << uint32_t(1 + bt_swarm.local_pavail().nb_byte_needed());
						pkt << cmdtype_val;
						bt_swarm.local_pavail().btformat_to_pkt(pkt);
						break;
	case bt_cmdtype_t::BLOCK_REQ:		pkt << uint32_t(13);	// length of the payload
						pkt << cmdtype_val;	// command type
						pkt << uint32_t(prange().piece_idx());
						pkt << uint32_t(prange().offset());
						pkt << uint32_t(prange().length());
						break;
	case bt_cmdtype_t::BLOCK_REP:		// NOTE: block_rep is manually built elsewhere
						// - in the read callback in fact as it needs the data
						//   from the disk.
						DBG_ASSERT( 0 );
	case bt_cmdtype_t::BLOCK_DEL:		pkt << uint32_t(13);	// length of the payload
						pkt << cmdtype_val;	// command type
						pkt << uint32_t(prange().piece_idx());
						pkt << uint32_t(prange().offset());
						pkt << uint32_t(prange().length());
						break;
	case bt_cmdtype_t::UTMSG_PAYL:		pkt << uint32_t(1 + utmsg_datum().length());
						pkt << cmdtype_val;
						pkt.append(utmsg_datum());
						break;
	default:	DBG_ASSERT( 0 );
	}
	// return the built datum
	return pkt.to_datum();
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                   to_string() function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string bt_cmd_t::to_string()			const throw()
{
	std::ostringstream	oss;
	// handle the null case
	if( is_null() )	return "null";
	// handle it depending on the bt_cmdtype_t
	switch( cmdtype_val.get_value() ){
	case bt_cmdtype_t::KEEPALIVE:		oss << "keepalive";				break;
	case bt_cmdtype_t::UNAUTH_REQ:		oss << "unauth_req";				break;
	case bt_cmdtype_t::DOAUTH_REQ:		oss << "doauth_req";				break;
	case bt_cmdtype_t::DOWANT_REQ:		oss << "dowant_req";				break;
	case bt_cmdtype_t::UNWANT_REQ:		oss << "unwant_req";				break;
	case bt_cmdtype_t::PIECE_ISAVAIL:	oss << "piece_isavail["	<< piece_idx()	<< "]";	break;
	case bt_cmdtype_t::PIECE_BFIELD:	oss << "piece_bfield";				break;
	case bt_cmdtype_t::BLOCK_REQ:		oss << "block_req["	<< prange()	<< "]";	break;
	case bt_cmdtype_t::BLOCK_REP:		oss << "block_rep["	<< prange()	<< "]";	break;
	case bt_cmdtype_t::BLOCK_DEL:		oss << "block_del["	<< prange()	<< "]";	break;
	case bt_cmdtype_t::UTMSG_PAYL:		oss << "utmsg_payl["	<< utmsg_datum()<< "]";	break;
	default:	DBG_ASSERT( 0 );
	}
	// return the just built string
	return oss.str();
}


NEOIP_NAMESPACE_END;






