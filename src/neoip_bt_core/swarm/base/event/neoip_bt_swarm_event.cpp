/*! \file
    \brief Implementation of the bt_swarm_event_t

*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_bt_swarm_event.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief constructor
 */
bt_swarm_event_t::bt_swarm_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
bt_swarm_event_t::~bt_swarm_event_t() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       display function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object to a std::string
 */
std::string	bt_swarm_event_t::to_string()	const throw()
{
	std::ostringstream	oss;
	// build the string
	switch( get_value() ){
	case bt_swarm_event_t::NONE:			oss << "NONE";				break;
	case bt_swarm_event_t::PIECE_NEWLY_AVAIL:	oss << "PIECE_NEWLY_AVAIL("
									<< piece_idx<<")";	break;
	case bt_swarm_event_t::DISK_ERROR:		oss << "DISK_ERROR(" << bt_err << ")";	break;
	default: 	DBG_ASSERT(0);
	}
	// return the just built string
	return oss.str();
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    PIECE_NEWLY_AVAIL
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is PIECE_NEWLY_AVAIL, false otherwise
 */
bool	bt_swarm_event_t::is_piece_newly_avail()				const throw()
{
	return type_val == PIECE_NEWLY_AVAIL;
}

/** \brief build a bt_swarm_event_t to PIECE_NEWLY_AVAIL
 */
bt_swarm_event_t bt_swarm_event_t::build_piece_newly_avail(size_t piece_idx)	throw()
{
	bt_swarm_event_t	swarm_event;
	// set the type_val
	swarm_event.type_val	= PIECE_NEWLY_AVAIL;
	swarm_event.piece_idx	= piece_idx;
	// return the built object
	return swarm_event;
}

/** \brief return the PIECE_NEWLY_AVAIL data
 */
size_t	bt_swarm_event_t::get_piece_newly_avail() 		const throw()
{
	// sanity check - the event MUST be PIECE_NEWLY_AVAIL
	DBG_ASSERT( is_piece_newly_avail() );
	// return the piece_idx
	return piece_idx;	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    DISK_ERROR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is DISK_ERROR, false otherwise
 */
bool	bt_swarm_event_t::is_disk_error() const throw()
{
	return type_val == DISK_ERROR;
}

/** \brief build a bt_swarm_event_t to DISK_ERROR (with a possible reason)
 */
bt_swarm_event_t bt_swarm_event_t::build_disk_error(const bt_err_t &bt_err)	throw()
{
	bt_swarm_event_t	swarm_event;
	// set the type_val
	swarm_event.type_val	= DISK_ERROR;
	swarm_event.bt_err	= bt_err;
	// return the built object
	return swarm_event;
}

/** \brief return the DISK_ERROR data
 */
const bt_err_t &	bt_swarm_event_t::get_disk_error() const throw()
{
	// sanity check - the event MUST be DISK_ERROR
	DBG_ASSERT( is_disk_error() );
	// return the bt_err
	return bt_err;	
}


NEOIP_NAMESPACE_END

