/*! \file
    \brief Implementation of the kad_event
*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_kad_event.hpp"
#include "neoip_kad_addr_arr.hpp"
#include "neoip_kad_caddr_arr.hpp"
#include "neoip_kad_recdups.hpp"
#include "neoip_cookie.hpp"
#include "neoip_pkt.hpp"
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
kad_event_t::kad_event_t() throw()
{
	type_val = NONE;
}

/** \brief desstructor
 */
kad_event_t::~kad_event_t() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       OSTREAM redirection
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object to a std::string
 */
std::string	kad_event_t::to_string()	const throw()
{
	std::ostringstream	oss;
	// build the string
	switch( get_value() ){
	case kad_event_t::NONE:		oss << "NONE";
					break;
	case kad_event_t::CNX_CLOSED:	oss << "CNX_CLOSED (reason: " << get_cnx_closed_reason() << ")";
					break;
	case kad_event_t::CNX_REFUSED:	oss << "CNX_REFUSED (reason: " << get_cnx_refused_reason() << ")";
					break;
	case kad_event_t::TIMEDOUT:	oss << "TIMEDOUT (reason: " << get_timedout_reason() << ")";
					break;
	case kad_event_t::RECVED_DATA:	oss << "RECVED_DATA (length=" << get_recved_data()->get_len() << "-byte)";
					break;	
	case kad_event_t::COMPLETED:	oss << "COMPLETED";
					break;	
	case kad_event_t::RECDUPS:	oss << "RECDUPS (recdups=" << kad_recdups
						<< " has_more_record=" << has_more_record_val << ")";	
					break;	
	case kad_event_t::ADDR_ARR:	oss << "ADDR_ARR (addr_arr=" << kad_addr_arr
						<< " resp_cookie=" << resp_cookie << ")";
					break;	
	case kad_event_t::CADDR_ARR:	oss << "CADDR_ARR (" << kad_caddr_arr << ")";
					break;
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
bool	kad_event_t::is_cnx_closed()						const throw()
{
	return type_val == CNX_CLOSED;
}

/** \brief set event to cnx_closed (with a possible reason)
 */
kad_event_t kad_event_t::build_cnx_closed(const std::string &reason)		throw()
{
	kad_event_t	kad_event;
	// set the type_val
	kad_event.type_val	= CNX_CLOSED;
	kad_event.reason_str	= reason;
	// return the built object
	return kad_event;
}

/** \brief return the cnx_closed reason
 */
const std::string &kad_event_t::get_cnx_closed_reason()				const throw()
{
	// sanity check - the event MUST be CNX_CLOSED
	DBG_ASSERT( is_cnx_closed() );
	// return the reasons
	return reason_str;
	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    CNX_REFUSED
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is CNX_REFUSED, false otherwise
 */
bool	kad_event_t::is_cnx_refused()						const throw()
{
	return type_val == CNX_REFUSED;
}

/** \brief set event to cnx_refused (with a possible reason)
 */
kad_event_t kad_event_t::build_cnx_refused(const std::string &reason)		throw()
{
	kad_event_t	kad_event;
	// set the type_val
	kad_event.type_val	= CNX_REFUSED;
	kad_event.reason_str	= reason;
	// return the built object
	return kad_event;
}

/** \brief return the cnx_refused reason
 */
const std::string &kad_event_t::get_cnx_refused_reason()			const throw()
{
	// sanity check - the event MUST be CNX_REFUSED
	DBG_ASSERT( is_cnx_refused() );
	// return the reasons
	return reason_str;
	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    TIMEDOUT
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is TIMEDOUT, false otherwise
 */
bool	kad_event_t::is_timedout()						const throw()
{
	return type_val == TIMEDOUT;
}

/** \brief set event to timedout (with a possible reason)
 */
kad_event_t kad_event_t::build_timedout(const std::string &reason)		throw()
{
	kad_event_t	kad_event;
	// set the type_val
	kad_event.type_val	= TIMEDOUT;
	kad_event.reason_str	= reason;
	// return the built object
	return kad_event;
}

/** \brief return the timedout reason
 */
const std::string &kad_event_t::get_timedout_reason()			const throw()
{
	// sanity check - the event MUST be TIMEDOUT
	DBG_ASSERT( is_timedout() );
	// return the reasons
	return reason_str;
	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    RECVED_DATA
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is RECVED_DATA, false otherwise
 */
bool	kad_event_t::is_recved_data()						const throw()
{
	return type_val == RECVED_DATA;
}

/** \brief Build a kad_event_t to RECVED_DATA
 */
kad_event_t kad_event_t::build_recved_data(pkt_t *pkt) 				throw()
{
	kad_event_t	kad_event;
	// set the type_val
	kad_event.type_val		= RECVED_DATA;
	kad_event.pkt_ptr		= pkt;
	// return the built object
	return kad_event;
}

/** \brief return the udp_full_t when RECVED_DATA
 */
pkt_t *	kad_event_t::get_recved_data()						const throw()
{
	// sanity check - the event MUST be RECVED_DATA
	DBG_ASSERT( is_recved_data() );
	// return the pkt_ptr
	return pkt_ptr;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    COMPLETED
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is COMPLETED, false otherwise
 */
bool	kad_event_t::is_completed()						const throw()
{
	return type_val == COMPLETED;
}

/** \brief Build a kad_event_t to COMPLETED
 */
kad_event_t kad_event_t::build_completed() 				throw()
{
	kad_event_t	kad_event;
	// set the type_val
	kad_event.type_val		= COMPLETED;
	// return the built object
	return kad_event;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    RECDUPS
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is RECDUPS, false otherwise
 */
bool	kad_event_t::is_recdups()						const throw()
{
	return type_val == RECDUPS;
}

/** \brief Build a kad_event_t to RECDUPS
 */
kad_event_t kad_event_t::build_recdups(const kad_recdups_t &recdups, bool has_more_record)	throw()
{
	kad_event_t	kad_event;
	// set the type_val
	kad_event.type_val		= RECDUPS;
	kad_event.kad_recdups		= recdups;
	kad_event.has_more_record_val	= has_more_record;
	// return the built object
	return kad_event;
}

/** \brief return the udp_full_t when RECDUPS
 */
const kad_recdups_t &kad_event_t::get_recdups(bool *has_more_record_out)		const throw()
{
	// sanity check - the event MUST be RECDUPS
	DBG_ASSERT( is_recdups() );
	// copy the has_more_record
	if( has_more_record_out )	*has_more_record_out	= has_more_record_val;
	// return the pkt_ptr
	return kad_recdups;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    ADDR_ARR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is ADDR_ARR, false otherwise
 */
bool	kad_event_t::is_addr_arr()						const throw()
{
	return type_val == ADDR_ARR;
}

/** \brief Build a kad_event_t to ADDR_ARR
 * 
 * - the cookie_id is here to allow kad_rpccli_t to report the cookie token
 *   replied by the remote node
 */
kad_event_t kad_event_t::build_addr_arr(const kad_addr_arr_t &kad_addr_arr
					, const cookie_id_t &resp_cookie)	throw()
{
	kad_event_t	kad_event;
	// set the type_val
	kad_event.type_val	= ADDR_ARR;
	kad_event.kad_addr_arr	= kad_addr_arr;
	kad_event.resp_cookie	= resp_cookie;
	// return the built object
	return kad_event;
}

/** \brief return the udp_full_t when ADDR_ARR
 */
const kad_addr_arr_t &kad_event_t::get_addr_arr(cookie_id_t *resp_cookie_out)	const throw()
{
	// sanity check - the event MUST be ADDR_ARR
	DBG_ASSERT( is_addr_arr() );
	// copy the resp_cookie_ptr
	if( resp_cookie_out )	*resp_cookie_out = resp_cookie;	
	// return the pkt_ptr
	return kad_addr_arr;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    CADDR_ARR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is CADDR_ARR, false otherwise
 */
bool	kad_event_t::is_caddr_arr()						const throw()
{
	return type_val == CADDR_ARR;
}

/** \brief Build a kad_event_t to CADDR_ARR
 */
kad_event_t kad_event_t::build_caddr_arr(const kad_caddr_arr_t &kad_caddr_arr)	throw()
{
	kad_event_t	kad_event;
	// set the type_val
	kad_event.type_val	= CADDR_ARR;
	kad_event.kad_caddr_arr	= kad_caddr_arr;
	// return the built object
	return kad_event;
}

/** \brief return the udp_full_t when CADDR_ARR
 */
const kad_caddr_arr_t &	kad_event_t::get_caddr_arr()				const throw()
{
	// sanity check - the event MUST be CADDR_ARR
	DBG_ASSERT( is_caddr_arr() );
	// return the kad_caddr_arr
	return kad_caddr_arr;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                          xmlrpc kad_event_t
// - TODO relativement half backed
//   1. no xmlrpc_build_t for kad_event_t::RECVED_DATA (useless as it is internal to kad layer)
//   2. no xmlrpc_parse_t whatsoever (useless currently as i dont parse received event from xmlrpc
//      - i dont even see a case where i could do that
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Put a kad_event_t into a xmlrpc
 */
xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const kad_event_t &kad_event)	throw()
{
	KLOG_ERR("enter kad_event=" << kad_event);
	xmlrpc_build << xmlrpc_build_t::STRUCT_BEG;
	xmlrpc_build	<< xmlrpc_build_t::MEMBER_BEG("type");
	// encode the type string
	// - TODO may be done a lot cleaner - currently this is duplicated with the to_tring()
	switch( kad_event.get_value() ){
	case kad_event_t::CNX_CLOSED:	xmlrpc_build << std::string("CNX_CLOSED");	break;
	case kad_event_t::CNX_REFUSED:	xmlrpc_build << std::string("CNX_REFUSED");	break;
	case kad_event_t::TIMEDOUT:	xmlrpc_build << std::string("TIMEDOUT");	break;
	case kad_event_t::COMPLETED:	xmlrpc_build << std::string("COMPLETED");	break;
	case kad_event_t::RECDUPS:	xmlrpc_build << std::string("RECDUPS");		break;
	case kad_event_t::ADDR_ARR:	xmlrpc_build << std::string("ADDR_ARR");	break;
	case kad_event_t::CADDR_ARR:	xmlrpc_build << std::string("CADDR_ARR");	break;
	default:	DBG_ASSERT( 0 );
	} 
	xmlrpc_build	<< xmlrpc_build_t::MEMBER_END;

	
	xmlrpc_build << xmlrpc_build_t::MEMBER_BEG("param");
	xmlrpc_build << xmlrpc_build_t::STRUCT_BEG;

	// add the members specific to the type
	switch( kad_event.get_value() ){
	// encode the kad_event_t using no parameter
	case kad_event_t::COMPLETED:
		break;
	// encode the kad_event_t using the reason_str
	case kad_event_t::CNX_CLOSED:	case kad_event_t::CNX_REFUSED:
	case kad_event_t::TIMEDOUT:	xmlrpc_build << xmlrpc_build_t::MEMBER_BEG("reason");
					xmlrpc_build	<< kad_event.reason_str;
					xmlrpc_build << xmlrpc_build_t::MEMBER_END;
					break;
	// encode the kad_event_t using the kad_event.kad_recdups
	case kad_event_t::RECDUPS:	xmlrpc_build << xmlrpc_build_t::MEMBER_BEG("recdups");
					xmlrpc_build	<< kad_event.kad_recdups;
					xmlrpc_build << xmlrpc_build_t::MEMBER_END;
					break;
	// encode the kad_event_t using the kad_event.kad_addr_arr + kad_event.resp_cookie
	case kad_event_t::ADDR_ARR:	xmlrpc_build << xmlrpc_build_t::MEMBER_BEG("addr_arr");
					xmlrpc_build	<< kad_event.kad_addr_arr;
					xmlrpc_build << xmlrpc_build_t::MEMBER_END;
					xmlrpc_build << xmlrpc_build_t::MEMBER_BEG("resp_cookie");
					xmlrpc_build	<< kad_event.resp_cookie;
					xmlrpc_build << xmlrpc_build_t::MEMBER_END;
					break;
	// encode the kad_event_t using the kad_event.kad_caddr_arr
	case kad_event_t::CADDR_ARR:	xmlrpc_build << xmlrpc_build_t::MEMBER_BEG("caddr_arr");
					xmlrpc_build	<< kad_event.kad_caddr_arr;
					xmlrpc_build << xmlrpc_build_t::MEMBER_END;
					break;
					
	default:	DBG_ASSERT( 0 );
	}
	xmlrpc_build << xmlrpc_build_t::STRUCT_END;
	xmlrpc_build << xmlrpc_build_t::MEMBER_END;

	// end the main struct
	xmlrpc_build << xmlrpc_build_t::STRUCT_END;

	// return the object itself
	return xmlrpc_build;
}

/** \brief Get a kad_event_t into a xmlrpc
 */
xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, kad_event_t &kad_event)	throw(xml_except_t)
{
	// TODO to code
	DBG_ASSERT( 0 );
	// return the object itself
	return xmlrpc_parse;
}

NEOIP_NAMESPACE_END



