/*! \file
    \brief Definition of the \ref casti_swarm_udata_t
    
\par Brief Description
casti_swarm_udata_t handle all the bt_cast_udata_t stuff for the casti_swarm_udata_t

\par about bt_utmsg_bcast_t of the bt_cast_udata_t
- this is a 'full-trust' mechanism where any peer can trivially forge data
- only the neoip-casti can legitimatly originate bt_cast_udata_t
- there is an alternative using a trusted infrastructure
  - the infratructure will be a set of trusted web servers which would be 
    periodically queried by neoip-casto/neoip-btrelay

\par TODO
- TODO should the bt_cast_udata_t generation be in this modules ?
  - along with all the mdata stuff
  - if so rename this module casti_swarm_mdata_t
  - currently just port the xmit part of the bt_cast_udata_t in here
  
*/

/* system include */
/* local include */
#include "neoip_casti_swarm_udata.hpp"
#include "neoip_casti_swarm.hpp"

#include "neoip_bt_cast_pidx.hpp"
#include "neoip_bt_cast_udata.hpp"

#include "neoip_bt_ezswarm.hpp"
#include "neoip_bt_ezswarm_share.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_utmsg.hpp"

#include "neoip_bt_utmsg_bcast.hpp"

#include "neoip_pkt.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
casti_swarm_udata_t::casti_swarm_udata_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	casti_swarm	= NULL;
	
}

/** \brief Destructor
 */
casti_swarm_udata_t::~casti_swarm_udata_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sent the null bt_cast_udata_t - to act as a SIGTERM
	// - to warn all neoip-casto that it is the end of this casti_swarm_t
	xmit_via_utmsg_bcast( bt_cast_udata_t() );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Set the profile for this object
 */
casti_swarm_udata_t &	casti_swarm_udata_t::profile(const casti_swarm_udata_profile_t &m_profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( m_profile.check() == bt_err_t::OK );
	// copy the parameter
	this->m_profile	= m_profile;
	// return the object iself
	return *this;
}

/** \brief Start the operation
 */
bt_err_t casti_swarm_udata_t::start(casti_swarm_t *casti_swarm)		throw()
{
	// log to debug
	KLOG_WARN("enter");
	// copy the parameter
	this->casti_swarm	= casti_swarm;

	// start the xmit_udata_timeout
	xmit_timeout.start(profile().xmit_maxdelay(), this, NULL);
	// save the m_last pieceq_end when the bt_cast_udata_t HAS BEEN be xmited
	// - it is initialized at pieceq_end (which is 'likely' 0 before this 
	//   function is called when the bt_ezswarm_t enter in SHARE)
	m_last_pieceq_end	= casti_swarm->pieceq_end;

	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Internal function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return a pointer on the bt_ezswarm_t of casti_swarm_t
 * 
 * - just an alias to ease the readability
 */
bt_ezswarm_t *	casti_swarm_udata_t::bt_ezswarm()	const throw()
{
	return casti_swarm->bt_ezswarm();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			timeout_cb_t
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the timeout_t expire
 */
bool casti_swarm_udata_t::neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw()
{
	// sent the bt_cast_udata_t
	xmit_via_utmsg_bcast( casti_swarm->current_udata() );
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			notify_pieceq_change
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief xmit the bt_cast_udata_t depending on pieceq boundary
 */
void	casti_swarm_udata_t::notify_pieceq_changed()	throw()
{
	bt_swarm_t *		bt_swarm	= bt_ezswarm()->share()->bt_swarm();
	const bt_mfile_t &	bt_mfile	= bt_swarm->get_mfile();
	bt_cast_pidx_t		cast_pidx	= bt_cast_pidx_t().modulo(bt_mfile.nb_piece());
	// determine the nosend range 
	// - aka range of pieceidx during which udata must not be xmited due to pieceq advance
	// - TODO should be ported to bt_cast_pidx_t  
	size_t	nosend_beg	= last_pieceq_end();
	size_t	nosend_end	= (last_pieceq_end() + profile().xmit_maxpiece() - 1)
					% bt_mfile.nb_piece();
	// if current pieceq_end is in the nosend range, return now without xmit_udata
	if( cast_pidx.index(casti_swarm->pieceq_end).is_in(nosend_beg, nosend_end) )	return;
	// do xmit_via_utmsg_bcast of the current_udata
	xmit_via_utmsg_bcast( casti_swarm->current_udata() );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			xmit_via_utmsg_bcast
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Send the bt_cast_udata_t over the bt_utmsgtype_t::BCAST
 * 
 * - this can be triggered by the amount of data received since the last xmit
 *   - see xmit_udata_notify_pieceq_change() and xmit_udata_last_end
 * - this can be triggered by timeout_t in xmit_udata_timeout_cb
 */
void	casti_swarm_udata_t::xmit_via_utmsg_bcast(const bt_cast_udata_t &cast_udata)	throw()
{
	bt_swarm_t *		bt_swarm	= bt_ezswarm()->share()->bt_swarm();
	bt_swarm_utmsg_t *	swarm_utmsg	= bt_swarm->swarm_utmsg();
	bt_utmsg_vapi_t*	utmsg_vapi	= swarm_utmsg->utmsg_vapi(bt_utmsgtype_t::BCAST);
	bt_utmsg_bcast_t *	utmsg_bcast	= dynamic_cast<bt_utmsg_bcast_t *>(utmsg_vapi); 
	// sanity check - utmsg_bcast MUST NOT be null
	DBG_ASSERT( utmsg_bcast );
	// log to debug
	KLOG_ERR("enter cast_udata=" << cast_udata);
	
	// originate the just built packet
	utmsg_bcast->originate_pkt(casti_swarm_t::UTMSG_BCAST_KEY, pkt_t().serialize(cast_udata));
	
	// save the last pieceq_end when the bt_cast_udata_t HAS BEEN be xmited
	m_last_pieceq_end	= casti_swarm->pieceq_end;

	// restart the xmit_timeout
	xmit_timeout.start(profile().xmit_maxdelay(), this, NULL);}

NEOIP_NAMESPACE_END;




