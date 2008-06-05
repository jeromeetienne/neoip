/*! \file
    \brief Class to handle the bt_utmsg_bcast_handler_t

\par Brief Description
\rev bt_utmsg_bcast_handler_t is used to register an handler for a given opstr.
Thus when a message with this opstr is received the bt_utmsg_bcast_handler_cb_t
will be called.

- LIMITATION: only one callback may be set for a given opstr

*/

/* system include */
/* local include */
#include "neoip_bt_utmsg_bcast_handler.hpp"
#include "neoip_bt_utmsg_bcast.hpp"
#include "neoip_pkt.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_utmsg_bcast_handler_t::bt_utmsg_bcast_handler_t()		throw()
{
	// zero some fields
	m_utmsg_bcast	= NULL;
}

/** \brief Destructor
 */
bt_utmsg_bcast_handler_t::~bt_utmsg_bcast_handler_t()		throw()
{
	// unlink this object from m_utmsg_bcast if needed
	if( m_utmsg_bcast )	m_utmsg_bcast->handler_unlink(this);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_utmsg_bcast_handler_t::start(bt_utmsg_bcast_t *m_utmsg_bcast
					, const std::string &m_opstr
					, bt_utmsg_bcast_handler_cb_t *callback
					, void *userptr)			throw()
{
	// copy the parameter
	this->m_utmsg_bcast	= m_utmsg_bcast;
	this->m_opstr		= m_opstr;
	this->callback		= callback;
	this->userptr		= userptr;
	// sanity check - m_opstr MUST NOT be already linked in m_utmsg_bcast
	DBG_ASSERT( m_utmsg_bcast->handler_by_opstr(m_opstr) == NULL );
	// link this object to m_utmsg_bcast
	m_utmsg_bcast->handler_dolink(this);
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Function called by the caller to notify a recved_pkt to this handler
 * 
 * - NOTE: this is delivered with a zerotimer_t to avoid complex nested notifications
 */
void	bt_utmsg_bcast_handler_t::notify_recved_pkt(pkt_t &recved_pkt)		throw()
{
	// put the recved_pkt in the notified_pkt_db
	notified_pkt_db.push_back(recved_pkt);
	// if the zerotimer_t is not running, launch it
	if( notify_zerotimer.empty() )	notify_zerotimer.append(this, NULL);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the nohash_zerotimer expire
 * 
 * - only used to avoid any nested notification issue
 */
bool	bt_utmsg_bcast_handler_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// sanity check - the notified_pkt_db MUST NOT be empty
	DBG_ASSERT( !notified_pkt_db.empty() );
	// notify all the pkt_t present in notified_pkt_db 
	while( !notified_pkt_db.empty() ){
		// get the first pkt_t to notify
		pkt_t	pkt	= notified_pkt_db.front();
		// remove it from the database
		notified_pkt_db.pop_front();
		// just notify the callback with the recved_pkt
		bool tokeep	= notify_callback(pkt);
		if( !tokeep )	return false;
	}
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback with the recved_pkt
 */
bool bt_utmsg_bcast_handler_t::notify_callback(pkt_t &recved_pkt)		throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_bt_utmsg_bcast_handler_cb(userptr, *this, recved_pkt);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END





