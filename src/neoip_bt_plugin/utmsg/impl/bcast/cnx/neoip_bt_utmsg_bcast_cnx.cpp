/*! \file
    \brief Class to handle the bt_utmsg_bcast_cnx_t

\par Brief Description
\ref bt_peersc_bcast_cnx_t is the context for all bt_swarm_full_t which declares
supporting bt_utmsgtype_t::BCAST.

*/

/* system include */
/* local include */
#include "neoip_bt_utmsg_bcast_cnx.hpp"
#include "neoip_bt_utmsg_bcast.hpp"
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
bt_utmsg_bcast_cnx_t::bt_utmsg_bcast_cnx_t()				throw()
{
	// zero some fields
	utmsg_bcast	= NULL;
	m_full_utmsg	= NULL;
}

/** \brief Destructor
 */
bt_utmsg_bcast_cnx_t::~bt_utmsg_bcast_cnx_t()				throw()
{
	// unlink this object from the bt_utmsg_bcast_t and bt_swarm_full_utmsg_t
	if( utmsg_bcast ){
		m_full_utmsg->cnx_vapi_unlink(this);
		utmsg_bcast->cnx_unlink(this);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_utmsg_bcast_cnx_t::start(bt_utmsg_bcast_t *utmsg_bcast
					, bt_swarm_full_utmsg_t *m_full_utmsg)	throw()
{
	// log to debug
	KLOG_WARN("enter");
	// copy the parameters
	this->utmsg_bcast	= utmsg_bcast;
	this->m_full_utmsg	= m_full_utmsg;
	
	// link this object to bt_utmsg_bcast_t
	utmsg_bcast->cnx_dolink(this);
	// link this object in the bt_swarm_full_utmsg_t
	m_full_utmsg->cnx_vapi_dolink(this);

	// return no error
	return bt_err_t::OK;
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
bool bt_utmsg_bcast_cnx_t::parse_pkt(const bt_utmsgtype_t &bt_utmsgtype
							, const pkt_t &pkt)	throw()
{
	// log to debug
	KLOG_DBG("enter bt_utmsgtype=" << bt_utmsgtype << " pkt=" << pkt);
	// sanity check - bt_utmsgtype MUST be the one of the bt_utmsg_vapi_t
	DBG_ASSERT( utmsg_bcast->utmsgtype() == bt_utmsgtype );

	// forward to the bt_utmsg_bcast_t 
	bt_err_t	bt_err = utmsg_bcast->parse_pkt(pkt, this);
	if( bt_err.failed() )	KLOG_ERR(bt_err.to_string() << " in pkt " << pkt);

	// return no error
	return true;
}

NEOIP_NAMESPACE_END





