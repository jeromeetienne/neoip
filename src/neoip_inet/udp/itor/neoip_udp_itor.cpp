/*! \file
    \brief Definition of the udp_itor_t

\par Implementation Notes
- the udp_full_t MUST be created during the start()
  - it is required in order to bind the local ipport.
  - the ntudp_pview_pool_t requires it to avoid some race conditions in the binding
    of the nat traversal udp hole punching.
    
*/

/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_udp_itor.hpp"
#include "neoip_udp_full.hpp"
#include "neoip_udp_layer.hpp"
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
udp_itor_t::udp_itor_t()				throw()
{
	// zero some fields
	callback	= NULL;
	// create the neoip_inet_udp_full_t
	udp_full	= nipmem_new udp_full_t();
	// link this object to the udp_layer_t
	udp_layer_get()->udp_itor_link(this);		
}

/** \brief destructor
 */
udp_itor_t::~udp_itor_t()				throw()
{
	// delete the udp_full if needed
	if( udp_full )	nipmem_delete udp_full;
	// unlink this object to the udp_layer_t
	udp_layer_get()->udp_itor_unlink(this);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                    null function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief return true is the object is null, false otherwise
 */
bool	udp_itor_t::is_null()	const throw()
{
	if( callback == NULL )	return true;
	return false;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       setup function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief set the callback
 */
inet_err_t	udp_itor_t::set_callback(udp_itor_cb_t *callback, void *userptr)throw()
{
	this->callback	= callback;
	this->userptr	= userptr;
	return inet_err_t::OK;
}

/** \brief set the local address
 */
inet_err_t	udp_itor_t::set_local_addr(const ipport_addr_t &local_addr)	throw()
{
	// set the local address of the udp_full
	return	udp_full->set_local_addr( local_addr );
}


/** \brief set the remote address
 */
inet_err_t	udp_itor_t::set_remote_addr(const ipport_addr_t &remote_addr)	throw()
{
	// set the local address of the udp_full
	return	udp_full->set_remote_addr( remote_addr );
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       start
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief start the action
 */
inet_err_t	udp_itor_t::start()						throw()
{
	// check the parameter
	if( callback == NULL )
		return inet_err_t(inet_err_t::BAD_PARAMETER, "a callback MUST be provided.");

	// set the zerotimer to create the udp_full_t
	zerotimer.append(this, NULL);
	// return no error
	return inet_err_t::OK;
}


/** \brief Set the mandatory parameters and start the actions
 * 
 * - This is only a helper function on top of the actual functions
 */
inet_err_t udp_itor_t::start(const ipport_addr_t &remote_addr, udp_itor_cb_t *callback, void *userptr)
										throw()
{
	inet_err_t	inet_err;
	// set the remote address
	inet_err = set_remote_addr(remote_addr);
	if( inet_err.failed() )	return inet_err;
	// set the callback
	inet_err = set_callback(callback, userptr);
	if( inet_err.failed() )	return inet_err;
	// start the action
	return start();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      query function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief return the local_addr
 */
const ipport_addr_t &	udp_itor_t::get_local_addr()				const throw()
{
	return udp_full->get_local_addr();
}

/** \brief return the remote_addr
 */
const ipport_addr_t &	udp_itor_t::get_remote_addr()				const throw()
{
	return udp_full->get_remote_addr();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                      to_string function
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief convert the object into a string
 */
std::string	udp_itor_t::to_string()	const throw()
{
	if( is_null() )	return "null";
	return get_local_addr().to_string() + " to " + get_remote_addr().to_string();
}


///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       zerotimer callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	udp_itor_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// build the event to notify
	udp_event_t	udp_event	= udp_event_t::build_cnx_established(udp_full);
	// mark the udp_full as unused
	udp_full	= NULL;
	// notify the caller
	bool		tokeep		= notify_callback(udp_event);
	if( !tokeep )	return false;
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   central function to notify the caller
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback
 */
bool	udp_itor_t::notify_callback(const udp_event_t &udp_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_inet_udp_itor_event_cb( userptr, *this, udp_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;	
}

NEOIP_NAMESPACE_END



