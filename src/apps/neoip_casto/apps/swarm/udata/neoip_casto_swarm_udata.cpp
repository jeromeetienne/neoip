/*! \file
    \brief Definition of the \ref casto_swarm_udata_t

\par Brief Description
casto_swarm_udata_t handle the reception of bt_cast_udata_t for casto_swarm_t    

*/

/* system include */
/* local include */
#include "neoip_casto_swarm_udata.hpp"
#include "neoip_casto_swarm_httpo.hpp"
#include "neoip_casto_swarm.hpp"

#include "neoip_bt_cast_helper.hpp"
#include "neoip_bt_cast_udata.hpp"
#include "neoip_bt_cast_pidx.hpp"

#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_ezswarm.hpp"
#include "neoip_bt_ezswarm_share.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_utmsg.hpp"

#include "neoip_bt_utmsg_bcast.hpp"
#include "neoip_bt_utmsg_bcast_handler.hpp"

#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
casto_swarm_udata_t::casto_swarm_udata_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	m_casto_swarm	= NULL;
	m_bcast_handler	= NULL;
}

/** \brief Destructor
 */
casto_swarm_udata_t::~casto_swarm_udata_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the bt_utmsg_bcase_handler_t if needed
	nipmem_zdelete	m_bcast_handler;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t casto_swarm_udata_t::start(casto_swarm_t *p_casto_swarm)	throw()
{
	// log to debug
	KLOG_WARN("enter");
	// copy the parameter
	this->m_casto_swarm	= p_casto_swarm;

	// sanity check - bt_ezswarm MUST be in share
	DBG_ASSERT( casto_swarm()->bt_ezswarm->in_share() ); 

	// get the pointer on bt_utmsg_bcast_t
	bt_ezswarm_t *		bt_ezswarm	= casto_swarm()->bt_ezswarm;
	bt_swarm_t *		bt_swarm	= bt_ezswarm->share()->bt_swarm();
	bt_swarm_utmsg_t *	swarm_utmsg	= bt_swarm->swarm_utmsg();
	bt_utmsg_vapi_t*	utmsg_vapi	= swarm_utmsg->utmsg_vapi(bt_utmsgtype_t::BCAST);
	bt_utmsg_bcast_t *	utmsg_bcast	= dynamic_cast<bt_utmsg_bcast_t *>(utmsg_vapi); 
	// sanity check - utmsg_bcast MUST NOT be null
	DBG_ASSERT( utmsg_bcast );	

	// start the bcast_handler
	bt_err_t	bt_err;
	m_bcast_handler	= nipmem_new bt_utmsg_bcast_handler_t();
	// TODO replace this hardcoded "casti" by a casti_swarm_t::UTMSG_BCAST_KEY
	bt_err		= m_bcast_handler->start(utmsg_bcast, "casti", this, NULL);
	if( bt_err.failed() )	return bt_err;

	// start the xmit_udata_timeout
	idle_timeout.start(casto_swarm()->cast_mdata().recv_udata_maxdelay(), this, NULL);
	
	// return no error
	return bt_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_utmsg_bcast_handler_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_utmsg_vapi_t when to notify a recved pkt_t
 */
bool	casto_swarm_udata_t::neoip_bt_utmsg_bcast_handler_cb(void *cb_userptr
						, bt_utmsg_bcast_handler_t &cb_bcast_handler
						, pkt_t &recved_pkt)	throw()
{
	bt_ezswarm_t *		bt_ezswarm	= casto_swarm()->bt_ezswarm;
	bt_swarm_t * 		bt_swarm	= bt_ezswarm->share()->bt_swarm();
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	bt_cast_mdata_t &	m_cast_mdata	= casto_swarm()->m_cast_mdata;
	casto_swarm_httpo_t *	swarm_httpo	= casto_swarm()->swarm_httpo();
	bt_cast_udata_t		cast_udata;
	// log to debug
	KLOG_DBG("enter recved_pkt=" << recved_pkt);
	// sanity check - bcast_handler MUST be set
	DBG_ASSERT( m_bcast_handler );
	
	// read a bt_cast_udata_t from the received packet
	try {
		recved_pkt >> cast_udata;
	}catch(serial_except_t &e){
		KLOG_ERR("failed to parse bt_cast_udata_t in " << recved_pkt);
		return true;
	}
	
	/* NOTE: broadcasting bt_cast_udata_t thru bt_utmsg_bcast_t is 'full-trust'
	 * - aka any peer can trivially forge fake bt_cast_udata_t
	 * - see neoip-casti neoip_casti_swarm.cpp comment for an alternative
	 *   using an infrastructure of trusted web servers
	 */

	// if bt_cast_udata_t is null, this is a signal from casti to notify gracefull shutdown
	if( cast_udata.is_null() ){
		std::string	reason	= "Received null cast_udata. aka casti gracefully shutted down";
		return casto_swarm()->autodelete(reason);
	}

	// check if the received bt_cast_udata_t is compatible with the current bt_cast_mdata_t
	// - it may not be compatible if the boot_nonce is different.
	// - NOTE: this is used to detect the reboot from neoip-casti
	if( !m_cast_mdata.compatible_with(cast_udata) ){
		std::string	reason = "Received imcompatible cast_udata. likely not the same boot_nonce";
		return casto_swarm()->autodelete(reason);
	}

	// log to debug
	KLOG_ERR("cast_mdata=" << m_cast_mdata);
	KLOG_ERR("cast_udata=" << cast_udata);


	// update the cast_mdata with the received bt_cast_udata_t
	m_cast_mdata.updated_with(cast_udata, bt_mfile);


	// syncronize all the unstarted bt_httpo_full_t with the new bt_cast_mdata_t
	swarm_httpo->httpo_full_resync_if_needed();

	// check all bt_htto_full_t and delete the ones in_overrun
	swarm_httpo->httpo_full_check_overrun();

	// if httpo_full_db is now empty, autodelete
	if( swarm_httpo->httpo_full_db().empty() )
		return casto_swarm()->autodelete("No more httpo_full_t");

	// remove all pieces which are nomore in the pieceq
	bt_cast_helper_t::remove_piece_outside_pieceq(bt_swarm, m_cast_mdata.pieceq_beg()
							, m_cast_mdata.pieceq_end() );

	// restart the idle_timeout
	idle_timeout.start(casto_swarm()->cast_mdata().recv_udata_maxdelay(), this, NULL);

	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			timeout_cb_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool casto_swarm_udata_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// delete the casto_swarm_t
	const delay_t &	recv_udata_maxdelay = casto_swarm()->cast_mdata().recv_udata_maxdelay();
	std::string	reason	= "udata idle_timeout expired after " + recv_udata_maxdelay.to_string();
	return casto_swarm()->autodelete(reason);
}

NEOIP_NAMESPACE_END;




