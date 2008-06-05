/*! \file
    \brief Implementation of the ntudp_npos_event
*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_ntudp_npos_event.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief constructor
 */
ntudp_npos_event_t::ntudp_npos_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
ntudp_npos_event_t::~ntudp_npos_event_t() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       OSTREAM redirection
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief ostream redirection
 */
std::ostream & operator << (std::ostream & os, const ntudp_npos_event_t & event) throw()
{
	switch( event.type_val ){
	case ntudp_npos_event_t::NONE:		
		os << "NONE";
		break;
	case ntudp_npos_event_t::COMPLETED:
		os << "COMPLETED (result: " << (event.completed_result ? "true" : "false") << ")";
		break;
	case ntudp_npos_event_t::NETWORK_ERROR:	
		os << "NETWORK_ERROR (reason: " << event.reason << ")";
		break;
	case ntudp_npos_event_t::TIMEDOUT:	
		os << "TIMEDOUT (reason: " << event.reason << ")";
		break;
	default: 	DBG_ASSERT(0);
	}
	return os;
}

/** \brief convert the object into a string
 */
std::string	ntudp_npos_event_t::to_string()		const throw()
{
	std::ostringstream      oss;
	oss << *this;
	return oss.str();
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    COMPLETED
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is COMPLETED, false otherwise
 */
bool	ntudp_npos_event_t::is_completed()					const throw()
{
	return type_val == COMPLETED;
}

/** \brief set event to completed (with a bool as a result)
 */
ntudp_npos_event_t ntudp_npos_event_t::build_completed(bool result)	throw()
{
	ntudp_npos_event_t	ntudp_npos_event;
	// set the type_val
	ntudp_npos_event.type_val		= COMPLETED;
	ntudp_npos_event.completed_result	= result;
	// return the built object
	return ntudp_npos_event;
}

/** \brief return the completed result
 */
bool ntudp_npos_event_t::get_completed_result()			const throw()
{
	// sanity check - the event MUST be COMPLETED
	DBG_ASSERT( is_completed() );
	// return the udp_event_t
	return completed_result;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    NETWORK_ERROR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is NETWORK_ERROR, false otherwise
 */
bool	ntudp_npos_event_t::is_network_error()					const throw()
{
	return type_val == NETWORK_ERROR;
}

/** \brief set event to network_error (with the udp_event which reported the faillure)
 */
ntudp_npos_event_t ntudp_npos_event_t::build_network_error(const std::string &reason)	throw()
{
	ntudp_npos_event_t	ntudp_npos_event;
	// set the type_val
	ntudp_npos_event.type_val	= NETWORK_ERROR;
	ntudp_npos_event.reason	= reason;
	// return the built object
	return ntudp_npos_event;
}

/** \brief return the network_error reason
 */
const std::string &ntudp_npos_event_t::get_network_error_reason()		const throw()
{
	// sanity check - the event MUST be NETWORK_ERROR
	DBG_ASSERT( is_network_error() );
	// return the reason
	return reason;
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    TIMEDOUT
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is TIMEDOUT, false otherwise
 */
bool	ntudp_npos_event_t::is_timedout()					const throw()
{
	return type_val == TIMEDOUT;
}

/** \brief set event to timedout (with the udp_event which reported the faillure)
 */
ntudp_npos_event_t ntudp_npos_event_t::build_timedout(const std::string &reason)	throw()
{
	ntudp_npos_event_t	ntudp_npos_event;
	// set the type_val
	ntudp_npos_event.type_val	= TIMEDOUT;
	ntudp_npos_event.reason	= reason;
	// return the built object
	return ntudp_npos_event;
}

/** \brief return the timedout reason
 */
const std::string &ntudp_npos_event_t::get_timedout_reason()		const throw()
{
	// sanity check - the event MUST be TIMEDOUT
	DBG_ASSERT( is_timedout() );
	// return the reason
	return reason;
}


NEOIP_NAMESPACE_END



