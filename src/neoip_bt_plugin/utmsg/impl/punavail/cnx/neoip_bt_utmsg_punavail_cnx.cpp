/*! \file
    \brief Class to handle the bt_utmsg_punavail_cnx_t

\par Brief Description
\ref bt_peersc_punavail_cnx_t is the contect for all bt_swarm_full_t which declares
supporting bt_utmsgtype_t::PUNAVAIL.

*/

/* system include */
/* local include */
#include "neoip_bt_utmsg_punavail_cnx.hpp"
#include "neoip_bt_utmsg_punavail.hpp"
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
bt_utmsg_punavail_cnx_t::bt_utmsg_punavail_cnx_t()				throw()
{
	// zero some fields
	utmsg_punavail	= NULL;
	full_utmsg	= NULL;
}

/** \brief Destructor
 */
bt_utmsg_punavail_cnx_t::~bt_utmsg_punavail_cnx_t()				throw()
{
	// unlink this object from the bt_utmsg_punavail_t and bt_swarm_full_utmsg_t
	if( utmsg_punavail ){
		full_utmsg->cnx_vapi_unlink(this);
		utmsg_punavail->cnx_unlink(this);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_utmsg_punavail_cnx_t::start(bt_utmsg_punavail_t *utmsg_punavail
					, bt_swarm_full_utmsg_t *full_utmsg)	throw()
{
	// log to debug
	KLOG_WARN("enter");
	// copy the parameters
	this->utmsg_punavail	= utmsg_punavail;
	this->full_utmsg	= full_utmsg;
	
	// link this object to bt_utmsg_punavail_t
	utmsg_punavail->cnx_dolink(this);
	// link this object in the bt_swarm_full_utmsg_t
	full_utmsg->cnx_vapi_dolink(this);
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			packet reception
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify a utmsg packet
 * 
 * @return a tokeep from the bt_swarm_full_t notification (if no notification, return true)
 */
bool bt_utmsg_punavail_cnx_t::parse_pkt(const bt_utmsgtype_t &bt_utmsgtype, const pkt_t &pkt)	throw()
{
	// log to debug
	KLOG_DBG("enter bt_utmsgtype=" << bt_utmsgtype << " pkt=" << pkt);
	// sanity check - bt_utmsgtype MUST be the one of the bt_utmsg_vapi_t
	DBG_ASSERT( utmsg_punavail->utmsgtype() == bt_utmsgtype );
	// convert the punavail message body to a dvar_t
	dvar_t	punavail_dvar	= bencode_t::to_dvar(pkt.to_datum(datum_t::NOCOPY));

	// if the handshake_dvar failed to parse the bencoded data, return an error
	if( punavail_dvar.is_null() )
		return parsing_error("piece_unavail has no data");
	// check this element type is dvar_type_t::INTEGER 
	if( punavail_dvar.type() != dvar_type_t::INTEGER )
		return parsing_error("piece_unavail is NOT an integer");
	// check that this integer is a uint32_t
	if( ! punavail_dvar.integer().is_uint32_ok() )
		return parsing_error("piece_unavail is NOT a uint32_t");


	// get the pieceidx of the piece unavail
	size_t	pieceidx	= punavail_dvar.integer().to_uint32();
	// notify the bt_swarm_full_t of it
	bt_swarm_full_t *	swarm_full	= full_utmsg->get_swarm_full();
	bool	tokeep	= swarm_full->notify_utmsg_punavail(pieceidx);
	if( !tokeep )	return false;

	// return noerror
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Generate the bt_cmd_t to xmit
 */
bt_cmd_t	bt_utmsg_punavail_cnx_t::generate_xmit_cmd(size_t pieceidx)	throw()
{
	// build the punavail_dvar
	dvar_t	punavail_dvar	= dvar_int_t(pieceidx);
	// generate the payload of the bt_cmd_t
	pkt_t	payload;
	payload << bt_utmsgtype_t(bt_utmsgtype_t::PUNAVAIL);
	payload.append(datum_t(bencode_t::from_dvar(punavail_dvar)));
	// return the bt_cmd_t to xmit
	return bt_cmd_t::build_utmsg_payl(payload.to_datum(datum_t::NOCOPY));
}

/** \brief function to call in case of error while parsing incoming payload
 */
bool	bt_utmsg_punavail_cnx_t::parsing_error(const std::string &reason)	throw()
{
	// if there is a reason, log it
	if( !reason.empty() )	KLOG_ERR(reason);
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			packet transmition
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Send a PieceNoHave extension
 */
void	bt_utmsg_punavail_cnx_t::xmit_punavail(size_t pieceidx)	throw()
{
	// generate the bt_cmd_t to xmit
	bt_cmd_t bt_cmd	= generate_xmit_cmd(pieceidx);
	// send it thru the bt_swarm_full_t
	full_utmsg->get_swarm_full()->send_cmd( bt_cmd );
}

NEOIP_NAMESPACE_END





