/*! \file
    \brief Class to handle the bt_peersrc_utpex_cnx_t

\par Brief Description
\ref bt_peersc_utpex_cnx_t is the contect for all bt_swarm_full_t which declares
supporting bt_utmsgtype_t::UTPEX.

*/

/* system include */
/* local include */
#include "neoip_bt_peersrc_utpex_cnx.hpp"
#include "neoip_bt_peersrc_utpex.hpp"
#include "neoip_bt_peersrc_helper.hpp"
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
bt_peersrc_utpex_cnx_t::bt_peersrc_utpex_cnx_t()				throw()
{
	// zero some fields
	peersrc_utpex	= NULL;
	full_utmsg	= NULL;
	next_seqnb	= 0;
}

/** \brief Destructor
 */
bt_peersrc_utpex_cnx_t::~bt_peersrc_utpex_cnx_t()				throw()
{
	// unlink this object from the bt_peersrc_utpex_t and bt_swarm_full_utmsg_t
	if( peersrc_utpex ){
		full_utmsg->cnx_vapi_unlink(this);
		peersrc_utpex->cnx_unlink(this);
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_peersrc_utpex_cnx_t::start(bt_peersrc_utpex_t *peersrc_utpex
					, bt_swarm_full_utmsg_t *full_utmsg)	throw()
{
	// log to debug
	KLOG_WARN("enter");
	// copy the parameters
	this->peersrc_utpex	= peersrc_utpex;
	this->full_utmsg	= full_utmsg;
	
	// link this object to bt_peersrc_utpex_t
	peersrc_utpex->cnx_dolink(this);
	// link this object in the bt_swarm_full_utmsg_t
	full_utmsg->cnx_vapi_dolink(this);
	
	// init the xmit_delaygen and xmit_timeout
	xmit_delaygen	= delaygen_t(peersrc_utpex->profile.xmit_delaygen());
	xmit_timeout.start(xmit_delaygen.current(), this, NULL);	
	// return no error
	return bt_err_t::OK;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_utmsg_cnx_vapi_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief parse the message received by this connection
 */
bool	bt_peersrc_utpex_cnx_t::parse_pkt(const bt_utmsgtype_t &bt_utmsgtype
							, const pkt_t &pkt)	throw()
{
	// just foward it to the peersrc_utpex
	return peersrc_utpex->parse_pkt(this, bt_utmsgtype, pkt );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Generate the pkt_t to xmit
 */
bt_cmd_t	bt_peersrc_utpex_cnx_t::generate_xmit_cmd()	throw()
{
	bt_peersrc_peer_arr_t	opened_arr;	
	bt_peersrc_peer_arr_t	closed_arr;
	// get the ipport_addr_arr_t's from the next_seqnb and update the next_seqnb
	next_seqnb	= peersrc_utpex->get_log_main(next_seqnb, opened_arr, closed_arr);
	// purge the peersrc_utpex log if needed
	peersrc_utpex->purge_log_if_needed();

	// if both ipport_addr_arr_t are empty, return a null bt_cmd_t and thus dont send any packet
	if( opened_arr.empty() && closed_arr.empty() )	return bt_cmd_t();
	
	// log to debug
	KLOG_DBG("opened_arr=" << opened_arr << " closed_arr=" << closed_arr);
	
	// build the utpex_dvar
	dvar_t	utpex_dvar	= dvar_map_t();

	// insert the "added" field in the utpex_dvar
	std::string	added_str	= bt_peersrc_helper_t::peer_arr_to_compactfmt(opened_arr);
	utpex_dvar.map().insert("added", dvar_str_t(added_str));

	// insert the "added.f" field in the utpex_dvar
	std::string	addedf_str	= bt_peersrc_helper_t::peer_arr_to_peerflag(opened_arr);
	utpex_dvar.map().insert("added.f", dvar_str_t(addedf_str));

	// insert the "dropped" field in the utpex_dvar
	std::string	dropped_str	= bt_peersrc_helper_t::peer_arr_to_compactfmt(closed_arr);
	utpex_dvar.map().insert("dropped", dvar_str_t(dropped_str));

	// generate the payload of the bt_cmd_t
	pkt_t	payload;
	payload << bt_utmsgtype_t(bt_utmsgtype_t::UTPEX);
	payload.append(datum_t(bencode_t::from_dvar(utpex_dvar)));
	// return the bt_cmd_t to xmit
	return bt_cmd_t::build_utmsg_payl(payload.to_datum(datum_t::NOCOPY));
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     timeout callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool bt_peersrc_utpex_cnx_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
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





