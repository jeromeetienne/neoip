/*! \file
    \brief Class to handle the bt_utmsg_byteacct_cnx_t

\par Brief Description
\ref bt_peersc_byteacct_cnx_t is the contect for all bt_swarm_full_t which declares
supporting bt_utmsgtype_t::BYTEACCT.

\par About rate estimator
bt_peersc_byteacct_cnx_t contains a crude rate estimator based on the 'absolute'
number passed by the remote peer. The rates start to be estimated at the second
notification from the remote peer and maintains a delta of byte recv/xmit during
2 notifications. This delta divided by the time between the notification acts
as the estimated speed until the next notification.
- NOTE: the rate estimation are valid IFF rate_available() is true
- CON: it is always 'one estimation late'
- PRO: it relies only on absolute numbers which is good because it will likely be
  the only number available when a safer 'bank system' will be implemented.
  - safer as in 'not full trust'
- CON: the first estimation is available only at the second notification from 
  the remote peer. so it relies strongly on the timers setting.

*/

/* system include */
/* local include */
#include "neoip_bt_utmsg_byteacct_cnx.hpp"
#include "neoip_bt_utmsg_byteacct.hpp"
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
bt_utmsg_byteacct_cnx_t::bt_utmsg_byteacct_cnx_t()				throw()
{
	// zero some fields
	utmsg_byteacct	= NULL;
	full_utmsg	= NULL;
	m_recv_rate	= std::numeric_limits<double>::max();
	m_xmit_rate	= std::numeric_limits<double>::max();
}

/** \brief Destructor
 */
bt_utmsg_byteacct_cnx_t::~bt_utmsg_byteacct_cnx_t()				throw()
{
	// unlink this object from the bt_utmsg_byteacct_t and bt_swarm_full_utmsg_t
	if( utmsg_byteacct ){
		full_utmsg->cnx_vapi_unlink(this);
		utmsg_byteacct->cnx_unlink(this);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_utmsg_byteacct_cnx_t::start(bt_utmsg_byteacct_t *utmsg_byteacct
					, bt_swarm_full_utmsg_t *full_utmsg)	throw()
{
	// log to debug
	KLOG_WARN("enter");
	// copy the parameters
	this->utmsg_byteacct	= utmsg_byteacct;
	this->full_utmsg	= full_utmsg;
	
	// link this object to bt_utmsg_byteacct_t
	utmsg_byteacct->cnx_dolink(this);
	// link this object in the bt_swarm_full_utmsg_t
	full_utmsg->cnx_vapi_dolink(this);
	
	// init the xmit_delaygen and xmit_timeout
	xmit_delaygen	= delaygen_t(utmsg_byteacct->profile.xmit_delaygen());
	xmit_timeout.start(xmit_delaygen.current(), this, NULL);
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
bool bt_utmsg_byteacct_cnx_t::parse_pkt(const bt_utmsgtype_t &bt_utmsgtype
							, const pkt_t &pkt)	throw()
{
	// log to debug
	KLOG_DBG("enter bt_utmsgtype=" << bt_utmsgtype << " pkt=" << pkt);
	// sanity check - bt_utmsgtype MUST be the one of the bt_utmsg_vapi_t
	DBG_ASSERT( utmsg_byteacct->utmsgtype() == bt_utmsgtype );
	// convert the byteacct message body to a dvar_t
	dvar_t	byteacct_dvar	= bencode_t::to_dvar(pkt.to_datum(datum_t::NOCOPY));

	// if the handshake_dvar failed to parse the bencoded data, return an error
	if( byteacct_dvar.is_null() || byteacct_dvar.type() != dvar_type_t::MAP )
		return parsing_error("unable to find byteacct payload as dvar_type_t::MAP");

	// if the byteacct_dvar doesnt include the balance, return an error
	if( !byteacct_dvar.map().contain("d", dvar_type_t::INTEGER) )
		return parsing_error("unable to find 'd' byteacct payload");
	if( !byteacct_dvar.map().contain("u", dvar_type_t::INTEGER) )
		return parsing_error("unable to find 'u' byteacct payload");

	// get the data from the dvar_t
	file_size_t	new_dloaded_len = byteacct_dvar.map()["d"].integer().to_uint64();
	file_size_t	new_uloaded_len = byteacct_dvar.map()["u"].integer().to_uint64();
	
	// update the recv_rate/xmit_rate if possible
	if( !last_update.is_null() ){
		// sanity check - dloaded_len/uloaded_len MUST be already set
		DBG_ASSERT( !m_uloaded_len.is_null() );
		DBG_ASSERT( !m_dloaded_len.is_null() );
		// sanity check - new_dloaded/uloaded_len MUST be >= than m_dloaded/uloaded_len
		DBG_ASSERT( new_dloaded_len >= m_dloaded_len );
		DBG_ASSERT( new_uloaded_len >= m_uloaded_len );
		// compute the delta compare to the last
		file_size_t	delta_recv	= new_dloaded_len - m_dloaded_len;
		file_size_t	delta_xmit	= new_uloaded_len - m_uloaded_len;
		date_t		delta_date	= date_t::present() - last_update;
		// set the recv_rate/xmit_rate
		m_recv_rate	= delta_recv.to_double() / delta_date.to_sec_double();
		m_xmit_rate	= delta_xmit.to_double() / delta_date.to_sec_double();
	}
	
	
	// update the dloaded_len/uloaded_len with the new data
	m_dloaded_len	= new_dloaded_len;
	m_uloaded_len	= new_uloaded_len;
	// update the last_update
	last_update	= date_t::present();
	
	// TODO ok nice i got the uloaded/dloaded len of the remote peer
	// - now what do i do with it ?
	KLOG_DBG("uloaded_len="<< uloaded_len() << " dloaded_len="<< dloaded_len());

	// return noerror
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Generate the bt_cmd_t to xmit
 */
bt_cmd_t	bt_utmsg_byteacct_cnx_t::generate_xmit_cmd()	throw()
{
	bt_swarm_t *		bt_swarm	= utmsg_byteacct->get_swarm();
	const bt_swarm_stats_t&	swarm_stats	= bt_swarm->swarm_stats();
	
	// build the byteacct_dvar
	dvar_t	byteacct_dvar	= dvar_map_t();

	// insert the "b" field in the byteacct_dvar
	byteacct_dvar.map().insert("u", dvar_int_t(swarm_stats.uloaded_datalen().to_uint64()));
	byteacct_dvar.map().insert("d", dvar_int_t(swarm_stats.dloaded_datalen().to_uint64()));

	// generate the payload of the bt_cmd_t
	pkt_t	payload;
	payload << bt_utmsgtype_t(bt_utmsgtype_t::BYTEACCT);
	payload.append(datum_t(bencode_t::from_dvar(byteacct_dvar)));
	// return the bt_cmd_t to xmit
	return bt_cmd_t::build_utmsg_payl(payload.to_datum(datum_t::NOCOPY));
}

/** \brief function to call in case of error while parsing incoming payload
 */
bool	bt_utmsg_byteacct_cnx_t::parsing_error(const std::string &reason)	throw()
{
	// if there is a reason, log it
	if( !reason.empty() )	KLOG_ERR(reason);
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     timeout callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool bt_utmsg_byteacct_cnx_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// log to debug
	KLOG_WARN("enter");
	// notify the expiration of the timeout to the delaygen
	xmit_delaygen.notify_expiration();
	// sanity check - delaygen MUST NOT timeout
	DBG_ASSERT( !xmit_delaygen.is_timedout() );

	// generate the bt_cmd_t to xmit
	bt_cmd_t bt_cmd	= generate_xmit_cmd();
	// if there is a command to xmit, send it thru the bt_swarm_full_t
	if( !bt_cmd.is_null() )	full_utmsg->get_swarm_full()->send_cmd( bt_cmd );	
	
	// set the next timer
	xmit_timeout.change_period(xmit_delaygen.pre_inc());	
	// return tokeep
	return true;
}



NEOIP_NAMESPACE_END





