/*! \file
    \brief Class to handle the bt_swarm_utmsg_t

\par Brief Description
\ref bt_swarm_utmsg_t handles the registration of the bt_utmsg_vapi_t plugin
to handle the various bt_utmsgtype_t.
- currently only one is implemented bt_peersrc_utpex_t. 

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_utmsg.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_utmsg_vapi.hpp"
#include "neoip_bt_utmsg_event.hpp"
#include "neoip_bt_utmsgtype.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

#include "neoip_bt_utmsg_piecewish.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_swarm_utmsg_t::bt_swarm_utmsg_t()		throw()
{
	// zero some fields
	m_bt_swarm	= NULL;
}

/** \brief Destructor
 */
bt_swarm_utmsg_t::~bt_swarm_utmsg_t()		throw()
{
	// sanity check - all the bt_utmsg_vapi_t MUST be unregistered before reahing this point
	DBG_ASSERT( m_utmsg_vapi_db.empty() );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                         setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_swarm_utmsg_t::start(bt_swarm_t *m_bt_swarm)	throw()
{
	// copy the parameter
	this->m_bt_swarm	= m_bt_swarm;
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the bt_utmsg_vapi_t for a bt_utmsgtype_t (or NULL if none match)
 */
bt_utmsg_vapi_t* bt_swarm_utmsg_t::utmsg_vapi(const bt_utmsgtype_t &bt_utmsgtype)const throw()
{
	utmsg_vapi_db_t::const_iterator	iter;
	// go thru the whole utmsg_vapi_db
	for(iter = m_utmsg_vapi_db.begin(); iter != m_utmsg_vapi_db.end(); iter++){
		bt_utmsg_vapi_t * utmsg_vapi	= *iter;
		// if this bt_utmsg_vapi_t matches, return its pointer
		if( bt_utmsgtype == utmsg_vapi->utmsgtype() )	return utmsg_vapi;
	}
	// if no bt_utmsg_vapi_t may handle this bt_utmsgtype_t, return NULL
	return NULL;	
}

/** \brief Return a pointer on bt_utmsg_piecewish_t (or null if not supported)
 */
bt_utmsg_piecewish_t *	bt_swarm_utmsg_t::utmsg_piecewish()	const throw()
{
	// get the bt_utmsg_vapi_t for the bt_utmsgtype_t::PIECEWISH
	bt_utmsg_vapi_t * vapi	= utmsg_vapi(bt_utmsgtype_t::PIECEWISH);
	// if not supported by the local peer, return NULL
	if( !vapi )	return NULL;
	// cast it to the proper type
	return dynamic_cast<bt_utmsg_piecewish_t*>(vapi);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//              bt_utmsg_vapi_t  callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Callback to receive bt_utmsg_event_t from the various bt_utmsg_vapi_t
 */
bool	bt_swarm_utmsg_t::neoip_bt_utmsg_cb(void *cb_userptr, bt_utmsg_vapi_t &cb_utmsg_vapi
				, const bt_utmsg_event_t &utmsg_event)	throw()
{
	bt_utmsg_vapi_t *	utmsg_vapi	= &cb_utmsg_vapi;
	// log to debug
	KLOG_WARN("enter utmsg_event=" << utmsg_event);
	
	// handle each possible events from its type
	switch( utmsg_event.get_value() ){
	case bt_utmsg_event_t::DOREGISTER:	m_utmsg_vapi_db.push_back(utmsg_vapi);	break;
	case bt_utmsg_event_t::UNREGISTER:	m_utmsg_vapi_db.remove(utmsg_vapi);	break;
	default:				DBG_ASSERT( 0 );
	}
	// return tokeep
	return true;
}


NEOIP_NAMESPACE_END





