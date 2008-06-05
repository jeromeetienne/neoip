/*! \file
    \brief Class to handle the bt_utmsg_piecewish_cnx_t

\par Brief Description
\ref bt_peersc_piecewish_cnx_t is the context for all bt_swarm_full_t which declares
supporting bt_utmsgtype_t::PIECEWISH.

*/

/* system include */
/* local include */
#include "neoip_bt_utmsg_piecewish_cnx.hpp"
#include "neoip_bt_utmsg_piecewish.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_full.hpp"
#include "neoip_bt_swarm_full_utmsg.hpp"
#include "neoip_bt_cmd.hpp"
#include "neoip_bencode.hpp"
#include "neoip_dvar.hpp"
#include "neoip_pkt.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_utmsg_piecewish_cnx_t::bt_utmsg_piecewish_cnx_t()				throw()
{
	// zero some fields
	utmsg_piecewish	= NULL;
	full_utmsg	= NULL;
}

/** \brief Destructor
 */
bt_utmsg_piecewish_cnx_t::~bt_utmsg_piecewish_cnx_t()				throw()
{
	// unlink this object from the bt_utmsg_piecewish_t and bt_swarm_full_utmsg_t
	if( utmsg_piecewish ){		
		full_utmsg->cnx_vapi_unlink(this);
		utmsg_piecewish->cnx_unlink(this);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_utmsg_piecewish_cnx_t::start(bt_utmsg_piecewish_t *utmsg_piecewish
					, bt_swarm_full_utmsg_t *full_utmsg)	throw()
{
	// log to debug
	KLOG_WARN("enter");
	// copy the parameters
	this->utmsg_piecewish	= utmsg_piecewish;
	this->full_utmsg	= full_utmsg;

	// link this object to bt_utmsg_piecewish_t
	utmsg_piecewish->cnx_dolink(this);
	// link this object in the bt_swarm_full_utmsg_t
	full_utmsg->cnx_vapi_dolink(this);

	// send the full bitfield_t local_pwish thru the bt_swarm_full_t
	send_cmd( utmsg_piecewish->build_cmd_dowish_field(utmsg_piecewish->local_pwish()) );
	
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Send bt_cmd_t thru this connection
 */
void	bt_utmsg_piecewish_cnx_t::send_cmd(const bt_cmd_t &bt_cmd)	throw()
{
	full_utmsg->get_swarm_full()->send_cmd( bt_cmd );
}

/** \brief function to call in case of error while parsing incoming payload
 */
bool	bt_utmsg_piecewish_cnx_t::parsing_error(const std::string &reason)	throw()
{
	// if there is a reason, log it
	if( !reason.empty() )	KLOG_ERR(reason);
	// return tokeep
	return true;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_utmsg_cnx_vapi_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify a utmsg packet
 * 
 * @return a tokeep from the bt_swarm_full_t notification (if no notification, return true)
 */
bool	bt_utmsg_piecewish_cnx_t::parse_pkt(const bt_utmsgtype_t &bt_utmsgtype
							, const pkt_t &pkt)	throw()
{
	// log to debug
	KLOG_DBG("enter bt_utmsgtype=" << bt_utmsgtype << " pkt=" << pkt);
	// sanity check - bt_utmsgtype MUST be the one of the bt_utmsg_vapi_t
	DBG_ASSERT( utmsg_piecewish->utmsgtype() == bt_utmsgtype );

	// convert the piecewish message body to a dvar_t
	dvar_t	dvar	= bencode_t::to_dvar(pkt.to_datum(datum_t::NOCOPY));

	// if the handshake_dvar failed to parse the bencoded data, return an error
	if( dvar.is_null() || dvar.type() != dvar_type_t::MAP )
		return parsing_error("unable to find utmsg_piecewish dvar_type_t::MAP");

	// if the dvar include the dowish_field, parse it
	if( dvar.map().contain("df", dvar_type_t::STRING) )
		return parse_dowish_field(dvar.map()["df"].str().get());
	
	// if the dvar include the dowish_index, parse it
	if( dvar.map().contain("di", dvar_type_t::ARRAY) )
		return parse_dowish_index(dvar.map()["di"].arr());
	
	// if the dvar include the nowish_index, parse it
	if( dvar.map().contain("ni", dvar_type_t::ARRAY) )
		return	parse_nowish_index(dvar.map()["ni"].arr());
	
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			command parsers
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Parse a dowish_field command
 */
bool bt_utmsg_piecewish_cnx_t::parse_dowish_field(const std::string &field_str)	throw()
{
	datum_t		field_datum	= datum_t(field_str);
	size_t		nb_piece	= utmsg_piecewish->nb_piece();
	bitfield_t	remote_pwish	= bitfield_t::from_datum(field_datum, nb_piece);
	bt_swarm_full_t	* swarm_full	= full_utmsg->get_swarm_full();
	// log to debug
	KLOG_DBG("enter");
	// if the parsing of the command failed, return an error
	if( remote_pwish.is_null() )	return parsing_error("unable to parse dowish_field");

	// notify bt_swarm_full_t of this command
	bool	tokeep	= swarm_full->notify_utmsg_dowish_field(remote_pwish);
	if( !tokeep )	return false;

	// return no error
	return true;
}

/** \brief Parse a dowish_index command
 */
bool bt_utmsg_piecewish_cnx_t::parse_dowish_index(const dvar_arr_t &dvar_arr)	throw()
{
	bt_swarm_full_t	* swarm_full	= full_utmsg->get_swarm_full();
	// log to debug
	KLOG_DBG("enter");
	// go thru the whole dvar_arr
	for(size_t i = 0; i < dvar_arr.size(); i++ ){
		// check this element type is dvar_type_t::INTEGER 
		if( dvar_arr[i].type() != dvar_type_t::INTEGER )
			return parsing_error("dowish_field is NOT an integer");
		// check that this integer is a uint32_t
		if( ! dvar_arr[i].integer().is_uint32_ok() )
			return parsing_error("dowish_field pieceidx is NOT a uint32_t");
		// get the pieceidx of this element
		size_t	pieceidx	= dvar_arr[i].integer().to_uint32();
		// notify bt_swarm_full_t of this command
		bool	tokeep	= swarm_full->notify_utmsg_dowish_index(pieceidx);
		if( !tokeep )	return false;
	}
	// return no error
	return true;
}

/** \brief Parse a nowish_index command
 */
bool	bt_utmsg_piecewish_cnx_t::parse_nowish_index(const dvar_arr_t &dvar_arr)	throw()
{
	bt_swarm_full_t	* swarm_full	= full_utmsg->get_swarm_full();
	// log to debug
	KLOG_DBG("enter");
	// go thru the whole dvar_arr
	for(size_t i = 0; i < dvar_arr.size(); i++ ){
		// check this element type is dvar_type_t::INTEGER 
		if( dvar_arr[i].type() != dvar_type_t::INTEGER )
			return parsing_error("dowish_field is NOT an integer");
		// check that this integer is a uint32_t
		if( ! dvar_arr[i].integer().is_uint32_ok() )
			return parsing_error("dowish_field pieceidx is NOT a uint32_t");
		// get the pieceidx of this element
		size_t	pieceidx	= dvar_arr[i].integer().to_uint32();
		// notify bt_swarm_full_t of this command
		bool	tokeep	= swarm_full->notify_utmsg_nowish_index(pieceidx);
		if( !tokeep )	return false;
	}
	// return no error
	return true;
}

NEOIP_NAMESPACE_END





