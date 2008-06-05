/*! \file
    \brief Class to handle the bt_utmsg_fstart_cnx_t

\par Brief Description
\ref bt_peersc_fstart_cnx_t is the context for all bt_swarm_full_t which declares
supporting bt_utmsgtype_t::FSTART.

*/

/* system include */
/* local include */
#include "neoip_bt_utmsg_fstart_cnx.hpp"
#include "neoip_bt_utmsg_fstart.hpp"
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
bt_utmsg_fstart_cnx_t::bt_utmsg_fstart_cnx_t()				throw()
{
	// zero some fields
	utmsg_fstart	= NULL;
	full_utmsg	= NULL;
	m_remote_fstart	= false;
}

/** \brief Destructor
 */
bt_utmsg_fstart_cnx_t::~bt_utmsg_fstart_cnx_t()				throw()
{
	// unlink this object from the bt_utmsg_fstart_t and bt_swarm_full_utmsg_t
	if( utmsg_fstart ){
		full_utmsg->cnx_vapi_unlink(this);
		utmsg_fstart->cnx_unlink(this);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_utmsg_fstart_cnx_t::start(bt_utmsg_fstart_t *utmsg_fstart
					, bt_swarm_full_utmsg_t *full_utmsg)	throw()
{
	// log to debug
	KLOG_WARN("enter");
	// copy the parameters
	this->utmsg_fstart	= utmsg_fstart;
	this->full_utmsg	= full_utmsg;
	
	// link this object to bt_utmsg_fstart_t
	utmsg_fstart->cnx_dolink(this);
	// link this object in the bt_swarm_full_utmsg_t
	full_utmsg->cnx_vapi_dolink(this);
	
	// if utmsg_fstart->local_fstart(), send a command to warn the remote peer
	// - as it default to 'not fstart'
	if( utmsg_fstart->local_fstart() )	send_cmd( utmsg_fstart->generate_xmit_cmd() );
	
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Send a bt_cmd_t to thru this connection
 */
void	bt_utmsg_fstart_cnx_t::send_cmd(const bt_cmd_t &bt_cmd)	const throw()
{
	// just forward the bt_cmd_t to the remote peer
	full_utmsg->get_swarm_full()->send_cmd( bt_cmd );
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
bool bt_utmsg_fstart_cnx_t::parse_pkt(const bt_utmsgtype_t &bt_utmsgtype
							, const pkt_t &pkt)	throw()
{
	// log to debug
	KLOG_DBG("enter bt_utmsgtype=" << bt_utmsgtype << " pkt=" << pkt);
	// sanity check - bt_utmsgtype MUST be the one of the bt_utmsg_vapi_t
	DBG_ASSERT( utmsg_fstart->utmsgtype() == bt_utmsgtype );
	// convert the fstart message body to a dvar_t
	dvar_t	dvar	= bencode_t::to_dvar(pkt.to_datum(datum_t::NOCOPY));

	// if the handshake_dvar failed to parse the bencoded data, return an error
	if( dvar.is_null() || dvar.type() != dvar_type_t::MAP )
		return parsing_error("unable to find fstart payload as dvar_type_t::MAP");

	// if the dvar doesnt include the balance, return an error
	if( !dvar.map().contain("v", dvar_type_t::INTEGER) )
		return parsing_error("unable to find 'v' fstart payload");

	// get the data from the dvar_t
	m_remote_fstart	= dvar.map()["v"].integer().to_size_t();

	// log to debug
	KLOG_ERR("remote_fstart=" << remote_fstart() );

	// return noerror
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to call in case of error while parsing incoming payload
 */
bool	bt_utmsg_fstart_cnx_t::parsing_error(const std::string &reason)	throw()
{
	// if there is a reason, log it
	if( !reason.empty() )	KLOG_ERR(reason);
	// return tokeep
	return true;
}

NEOIP_NAMESPACE_END





