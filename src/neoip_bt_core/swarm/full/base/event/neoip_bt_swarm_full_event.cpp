/*! \file
    \brief Implementation of the bt_swarm_full_event_t
*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_bt_swarm_full_event.hpp"
#include "neoip_bitfield.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief constructor
 */
bt_swarm_full_event_t::bt_swarm_full_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
bt_swarm_full_event_t::~bt_swarm_full_event_t() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       display function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object to a std::string
 */
std::string	bt_swarm_full_event_t::to_string()	const throw()
{
	std::ostringstream	oss;
	// build the string
	switch( get_value() ){
	case bt_swarm_full_event_t::NONE:		oss << "NONE";		break;
	case bt_swarm_full_event_t::CNX_OPENED:		oss << "CNX_OPENED";	break;
	case bt_swarm_full_event_t::CNX_CLOSED:		oss << "CNX_CLOSED";	break;
	case bt_swarm_full_event_t::REQAUTH_TIMEDOUT:	oss << "REQAUTH_TIMEDOUT";
										break;
	case bt_swarm_full_event_t::UNAUTH_REQ:		oss << "UNAUTH_REQ";	break;
	case bt_swarm_full_event_t::DOAUTH_REQ:		oss << "DOAUTH_REQ";	break;
	case bt_swarm_full_event_t::UNWANT_REQ:		oss << "UNWANT_REQ";	break;
	case bt_swarm_full_event_t::DOWANT_REQ:		oss << "DOWANT_REQ";	break;
	case bt_swarm_full_event_t::PIECE_ISAVAIL:	oss << "PIECE_ISAVAIL(" << piece_idx <<")";break;
	case bt_swarm_full_event_t::PIECE_UNAVAIL:	oss << "PIECE_UNAVAIL(" << piece_idx <<")";break;
	case bt_swarm_full_event_t::PIECE_BFIELD:	oss << "PIECE_BFIELD";	break;
	case bt_swarm_full_event_t::BLOCK_REP:		oss << "BLOCK_REP(" << bt_cmd <<")";
										break;
	case bt_swarm_full_event_t::PWISH_DOINDEX:	oss << "PWISH_DOINDEX(" << piece_idx <<")";break;
	case bt_swarm_full_event_t::PWISH_NOINDEX:	oss << "PWISH_NOINDEX(" << piece_idx <<")";break;
	case bt_swarm_full_event_t::PWISH_DOFIELD:	oss << "PWISH_DOFIELD(" << *old_bitfield
								<< ", " << *new_bitfield <<")";	break;
	default: 	DBG_ASSERT(0);
	}
	// return the just built string
	return oss.str();
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    CNX_CLOSED
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is CNX_CLOSED, false otherwise
 */
bool	bt_swarm_full_event_t::is_cnx_closed() const throw()
{
	return type_val == CNX_CLOSED;
}

/** \brief build a bt_swarm_full_event_t to CNX_CLOSED
 */
bt_swarm_full_event_t bt_swarm_full_event_t::build_cnx_closed()	throw()
{
	bt_swarm_full_event_t	full_event;
	// set the type_val
	full_event.type_val	= CNX_CLOSED;
	// return the built object
	return full_event;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    CNX_OPENED
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is CNX_OPENED, false otherwise
 */
bool	bt_swarm_full_event_t::is_cnx_opened() const throw()
{
	return type_val == CNX_OPENED;
}

/** \brief build a bt_swarm_full_event_t to CNX_OPENED
 */
bt_swarm_full_event_t bt_swarm_full_event_t::build_cnx_opened()	throw()
{
	bt_swarm_full_event_t	full_event;
	// set the type_val
	full_event.type_val	= CNX_OPENED;
	// return the built object
	return full_event;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    REQAUTH_TIMEDOUT
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is REQAUTH_TIMEDOUT, false otherwise
 */
bool	bt_swarm_full_event_t::is_reqauth_timedout() const throw()
{
	return type_val == REQAUTH_TIMEDOUT;
}

/** \brief build a bt_swarm_full_event_t to REQAUTH_TIMEDOUT
 */
bt_swarm_full_event_t bt_swarm_full_event_t::build_reqauth_timedout()	throw()
{
	bt_swarm_full_event_t	full_event;
	// set the type_val
	full_event.type_val	= REQAUTH_TIMEDOUT;
	// return the built object
	return full_event;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    UNAUTH_REQ
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is UNAUTH_REQ, false otherwise
 */
bool	bt_swarm_full_event_t::is_unauth_req() const throw()
{
	return type_val == UNAUTH_REQ;
}

/** \brief build a bt_swarm_full_event_t to UNAUTH_REQ
 */
bt_swarm_full_event_t bt_swarm_full_event_t::build_unauth_req()	throw()
{
	bt_swarm_full_event_t	full_event;
	// set the type_val
	full_event.type_val	= UNAUTH_REQ;
	// return the built object
	return full_event;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    DOAUTH_REQ
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is DOAUTH_REQ, false otherwise
 */
bool	bt_swarm_full_event_t::is_doauth_req() const throw()
{
	return type_val == DOAUTH_REQ;
}

/** \brief build a bt_swarm_full_event_t to DOAUTH_REQ
 */
bt_swarm_full_event_t bt_swarm_full_event_t::build_doauth_req()	throw()
{
	bt_swarm_full_event_t	full_event;
	// set the type_val
	full_event.type_val	= DOAUTH_REQ;
	// return the built object
	return full_event;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    UNWANT_REQ
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is UNWANT_REQ, false otherwise
 */
bool	bt_swarm_full_event_t::is_unwant_req() const throw()
{
	return type_val == UNWANT_REQ;
}

/** \brief build a bt_swarm_full_event_t to UNWANT_REQ
 */
bt_swarm_full_event_t bt_swarm_full_event_t::build_unwant_req()	throw()
{
	bt_swarm_full_event_t	full_event;
	// set the type_val
	full_event.type_val	= UNWANT_REQ;
	// return the built object
	return full_event;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    DOWANT_REQ
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is DOWANT_REQ, false otherwise
 */
bool	bt_swarm_full_event_t::is_dowant_req() const throw()
{
	return type_val == DOWANT_REQ;
}

/** \brief build a bt_swarm_full_event_t to DOWANT_REQ
 */
bt_swarm_full_event_t bt_swarm_full_event_t::build_dowant_req()	throw()
{
	bt_swarm_full_event_t	full_event;
	// set the type_val
	full_event.type_val	= DOWANT_REQ;
	// return the built object
	return full_event;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    PIECE_ISAVAIL
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is PIECE_ISAVAIL, false otherwise
 */
bool	bt_swarm_full_event_t::is_piece_isavail() const throw()
{
	return type_val == PIECE_ISAVAIL;
}

/** \brief build a bt_swarm_full_event_t to PIECE_ISAVAIL (with a possible reason)
 */
bt_swarm_full_event_t bt_swarm_full_event_t::build_piece_isavail(const size_t &piece_idx)	throw()
{
	bt_swarm_full_event_t	full_event;
	// set the type_val
	full_event.type_val	= PIECE_ISAVAIL;
	full_event.piece_idx	= piece_idx;
	// return the built object
	return full_event;
}

/** \brief return the PIECE_ISAVAIL data
 */
const size_t &	bt_swarm_full_event_t::get_piece_isavail() const throw()
{
	// sanity check - the event MUST be PIECE_ISAVAIL
	DBG_ASSERT( is_piece_isavail() );
	// return the piece_idx
	return piece_idx;	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    PIECE_UNAVAIL
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is PIECE_UNAVAIL, false otherwise
 */
bool	bt_swarm_full_event_t::is_piece_unavail() const throw()
{
	return type_val == PIECE_UNAVAIL;
}

/** \brief build a bt_swarm_full_event_t to PIECE_UNAVAIL (with a possible reason)
 */
bt_swarm_full_event_t bt_swarm_full_event_t::build_piece_unavail(const size_t &piece_idx)	throw()
{
	bt_swarm_full_event_t	full_event;
	// set the type_val
	full_event.type_val	= PIECE_UNAVAIL;
	full_event.piece_idx	= piece_idx;
	// return the built object
	return full_event;
}

/** \brief return the PIECE_UNAVAIL data
 */
const size_t &	bt_swarm_full_event_t::get_piece_unavail() const throw()
{
	// sanity check - the event MUST be PIECE_UNAVAIL
	DBG_ASSERT( is_piece_unavail() );
	// return the piece_idx
	return piece_idx;	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    PIECE_BFIELD
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is PIECE_BFIELD, false otherwise
 */
bool	bt_swarm_full_event_t::is_piece_bfield() const throw()
{
	return type_val == PIECE_BFIELD;
}

/** \brief build a bt_swarm_full_event_t to PIECE_BFIELD
 */
bt_swarm_full_event_t bt_swarm_full_event_t::build_piece_bfield()	throw()
{
	bt_swarm_full_event_t	full_event;
	// set the type_val
	full_event.type_val	= PIECE_BFIELD;
	// return the built object
	return full_event;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    BLOCK_REP
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is BLOCK_REP, false otherwise
 */
bool	bt_swarm_full_event_t::is_block_rep() const throw()
{
	return type_val == BLOCK_REP;
}

/** \brief build a bt_swarm_full_event_t to BLOCK_REP (with a possible reason)
 */
bt_swarm_full_event_t bt_swarm_full_event_t::build_block_rep(const bt_cmd_t &bt_cmd, pkt_t *pkt) throw()
{
	bt_swarm_full_event_t	full_event;
	// set the type_val
	full_event.type_val	= BLOCK_REP;
	full_event.bt_cmd	= bt_cmd;
	full_event.pkt		= pkt;
	// return the built object
	return full_event;
}

/** \brief return the BLOCK_REP reason
 */
pkt_t *	bt_swarm_full_event_t::get_block_rep(bt_cmd_t &bt_cmd_out) const throw()
{
	// sanity check - the event MUST be BLOCK_REP
	DBG_ASSERT( is_block_rep() );
	// copy the bt_cmd
	bt_cmd_out	= bt_cmd;
	// return the pkt pointer
	return pkt;	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    PWISH_DOINDEX
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is PWISH_DOINDEX, false otherwise
 */
bool	bt_swarm_full_event_t::is_pwish_doindex() const throw()
{
	return type_val == PWISH_DOINDEX;
}

/** \brief build a bt_swarm_full_event_t to PWISH_DOINDEX (with a possible reason)
 */
bt_swarm_full_event_t bt_swarm_full_event_t::build_pwish_doindex(const size_t &piece_idx)	throw()
{
	bt_swarm_full_event_t	full_event;
	// set the type_val
	full_event.type_val	= PWISH_DOINDEX;
	full_event.piece_idx	= piece_idx;
	// return the built object
	return full_event;
}

/** \brief return the PWISH_DOINDEX data
 */
const size_t &	bt_swarm_full_event_t::get_pwish_doindex() const throw()
{
	// sanity check - the event MUST be PWISH_DOINDEX
	DBG_ASSERT( is_pwish_doindex() );
	// return the piece_idx
	return piece_idx;	
}
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    PWISH_NOINDEX
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is PWISH_NOINDEX, false otherwise
 */
bool	bt_swarm_full_event_t::is_pwish_noindex() const throw()
{
	return type_val == PWISH_NOINDEX;
}

/** \brief build a bt_swarm_full_event_t to PWISH_NOINDEX (with a possible reason)
 */
bt_swarm_full_event_t bt_swarm_full_event_t::build_pwish_noindex(const size_t &piece_idx)	throw()
{
	bt_swarm_full_event_t	full_event;
	// set the type_val
	full_event.type_val	= PWISH_NOINDEX;
	full_event.piece_idx	= piece_idx;
	// return the built object
	return full_event;
}

/** \brief return the PWISH_NOINDEX data
 */
const size_t &	bt_swarm_full_event_t::get_pwish_noindex() const throw()
{
	// sanity check - the event MUST be PWISH_NOINDEX
	DBG_ASSERT( is_pwish_noindex() );
	// return the piece_idx
	return piece_idx;	
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    PWISH_DOFIELD
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is PWISH_DOFIELD, false otherwise
 */
bool	bt_swarm_full_event_t::is_pwish_dofield() const throw()
{
	return type_val == PWISH_DOFIELD;
}

/** \brief build a bt_swarm_full_event_t to PWISH_DOFIELD (with a possible reason)
 */
bt_swarm_full_event_t bt_swarm_full_event_t::build_pwish_dofield(const bitfield_t *old_bitfield
					, const bitfield_t *new_bitfield)	throw()
{
	bt_swarm_full_event_t	full_event;
	// set the type_val
	full_event.type_val	= PWISH_DOFIELD;
	full_event.old_bitfield	= old_bitfield;
	full_event.new_bitfield	= new_bitfield;
	// return the built object
	return full_event;
}

/** \brief return the PWISH_DOFIELD reason
 */
const bitfield_t *	bt_swarm_full_event_t::get_pwish_dofield(const bitfield_t **old_field_out) const throw()
{
	// sanity check - the event MUST be PWISH_DOFIELD
	DBG_ASSERT( is_pwish_dofield() );
	// copy the old_bitfield
	if( old_field_out )	*old_field_out	= old_bitfield;
	// return the new_bitfield pointer
	return new_bitfield;	
}
NEOIP_NAMESPACE_END

