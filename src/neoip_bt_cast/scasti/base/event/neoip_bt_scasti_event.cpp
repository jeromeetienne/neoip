/*! \file
    \brief Implementation of the bt_scasti_event_t

*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_bt_scasti_event.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief constructor
 */
bt_scasti_event_t::bt_scasti_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
bt_scasti_event_t::~bt_scasti_event_t() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       display function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object to a std::string
 */
std::string	bt_scasti_event_t::to_string()	const throw()
{
	std::ostringstream	oss;
	// build the string
	switch( get_value() ){
	case bt_scasti_event_t::NONE:		oss << "NONE";				break;
	case bt_scasti_event_t::CHUNK_AVAIL:	oss << "CHUNK_AVAIL("<< chunk_len<<")";	break;
	case bt_scasti_event_t::ERROR:		oss << "ERROR(" << bt_err << ")";	break;
	case bt_scasti_event_t::MOD_UPDATED:	oss << "MOD_UPDATED";			break;
	default:	DBG_ASSERT(0);
	}
	// return the just built string
	return oss.str();
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    CHUNK_AVAIL
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is CHUNK_AVAIL, false otherwise
 */
bool	bt_scasti_event_t::is_chunk_avail()				const throw()
{
	return type_val == CHUNK_AVAIL;
}

/** \brief build a bt_scasti_event_t to CHUNK_AVAIL
 */
bt_scasti_event_t bt_scasti_event_t::build_chunk_avail(const file_size_t &chunk_len)	throw()
{
	bt_scasti_event_t	scasti_event;
	// set the type_val
	scasti_event.type_val	= CHUNK_AVAIL;
	scasti_event.chunk_len	= chunk_len;
	// return the built object
	return scasti_event;
}

/** \brief return the CHUNK_AVAIL data
 */
const file_size_t &	bt_scasti_event_t::get_chunk_avail() 		const throw()
{
	// sanity check - the event MUST be CHUNK_AVAIL
	DBG_ASSERT( is_chunk_avail() );
	// return the chunk_len
	return chunk_len;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			ERROR
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is ERROR, false otherwise
 */
bool	bt_scasti_event_t::is_error() const throw()
{
	return type_val == ERROR;
}

/** \brief build a bt_scasti_event_t to ERROR (with a possible reason)
 */
bt_scasti_event_t bt_scasti_event_t::build_error(const bt_err_t &bt_err)	throw()
{
	bt_scasti_event_t	scasti_event;
	// set the type_val
	scasti_event.type_val	= ERROR;
	scasti_event.bt_err	= bt_err;
	// return the built object
	return scasti_event;
}

/** \brief return the ERROR data
 */
const bt_err_t &	bt_scasti_event_t::get_error() const throw()
{
	// sanity check - the event MUST be ERROR
	DBG_ASSERT( is_error() );
	// return the bt_err
	return bt_err;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			MOD_UPDATED
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is MOD_UPDATED, false otherwise
 */
bool	bt_scasti_event_t::is_mod_updated() 					const throw()
{
	return type_val == MOD_UPDATED;
}

/** \brief build a bt_scasti_event_t to MOD_UPDATED (with a possible reason)
 */
bt_scasti_event_t bt_scasti_event_t::build_mod_updated()				throw()
{
	bt_scasti_event_t	scasti_event;
	// set the type_val
	scasti_event.type_val	= MOD_UPDATED;
	// return the built object
	return scasti_event;
}

NEOIP_NAMESPACE_END

