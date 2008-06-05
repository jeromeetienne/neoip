/*! \file
    \brief Implementation of the bt_ecnx_event_t
*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_bt_ecnx_event.hpp"
#include "neoip_datum.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief constructor
 */
bt_ecnx_event_t::bt_ecnx_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
bt_ecnx_event_t::~bt_ecnx_event_t() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       display function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object to a std::string
 */
std::string	bt_ecnx_event_t::to_string()	const throw()
{
	std::ostringstream	oss;
	// build the string
	switch( get_value() ){
	case bt_ecnx_event_t::NONE:		oss << "NONE";		break;
	case bt_ecnx_event_t::CNX_OPENED:	oss << "CNX_OPENED";	break;
	case bt_ecnx_event_t::CNX_CLOSED:	oss << "CNX_CLOSED";	break;
	case bt_ecnx_event_t::BLOCK_REP:	oss << "BLOCK_REP(" << recved_data_ptr->size()
							<< "-byte)";	break;
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
bool	bt_ecnx_event_t::is_cnx_closed() const throw()
{
	return type_val == CNX_CLOSED;
}

/** \brief build a bt_ecnx_event_t to CNX_CLOSED
 */
bt_ecnx_event_t bt_ecnx_event_t::build_cnx_closed()	throw()
{
	bt_ecnx_event_t	full_event;
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
bool	bt_ecnx_event_t::is_cnx_opened() const throw()
{
	return type_val == CNX_OPENED;
}

/** \brief build a bt_ecnx_event_t to CNX_OPENED
 */
bt_ecnx_event_t bt_ecnx_event_t::build_cnx_opened()	throw()
{
	bt_ecnx_event_t	full_event;
	// set the type_val
	full_event.type_val	= CNX_OPENED;
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
bool	bt_ecnx_event_t::is_block_rep() const throw()
{
	return type_val == BLOCK_REP;
}

/** \brief build a bt_ecnx_event_t to BLOCK_REP
 */
bt_ecnx_event_t bt_ecnx_event_t::build_block_rep(bt_swarm_sched_request_t *sched_req_ptr
							, datum_t *recved_data_ptr)	throw()
{
	bt_ecnx_event_t	full_event;
	// set the type_val
	full_event.type_val		= BLOCK_REP;
	full_event.sched_req_ptr	= sched_req_ptr;
	full_event.recved_data_ptr	= recved_data_ptr;
	// return the built object
	return full_event;
}

/** \brief get the parameter a bt_ecnx_event_t to BLOCK_REP
 */
datum_t * bt_ecnx_event_t::get_block_rep(bt_swarm_sched_request_t **sched_req_out)	const throw()
{
	// sanity check - the event MUST be BLOCK_REP
	DBG_ASSERT( is_block_rep() );
	// copy the sched_req_ptr
	*sched_req_out	= sched_req_ptr;
	// return the recved_data_ptr
	return recved_data_ptr;
}

NEOIP_NAMESPACE_END

