/*! \file
    \brief Definition of the \ref lib_session_exit_t

\par Brief Description
lib_session_exit_t is a mechanism for code to be called when the lib_session_t
is about to exit. e.g. to unregister a resource or delete file.

\par About the local states
- the lib_session_exit_t is in_exiting() as soon as begin_exiting() is called
  and the caller will be notified by a zerotimer_t launched in the begin_exiting()
- the lib_session_exit_t is is_notified() as soon as the caller has been notified

\par about the exit_order
- there is a 'exit_order' which allow to determine the order at which the 
  various lib_session_exit_t will be called.
- the lib_session_exit_t are notified in increasing exit_order
- multiple lib_session_exit_t may have the same exit_order
- example:
  - layer A and B have an exit_order of 4
  - layer C have an exit_order of 2
  - layer C will be notified first, and ONLY when layer C has deleted its 
    lib_session_exit_t, layer A and B will be notified.
    - layer A and B will be notified in the same event loop because they have
      the same exit_order 

\par About expiration of a specific lib_session_exit_t
- the lib_session_t exiting has already a global timeout.
- sometime it is insteresting to timeout a given lib_session_exit_t to let 
  time to the others lib_session_exit_t with a greater exit_order.
- if it is wished it must be coded inside the lib_session_exit_t caller 
  - this is not coded here on purpose because it is IMPORTANT that the caller
    is fully aware of the end of the lib_session_exit_t 
  - it may be mandatory to delete some rescources at one exit_order BEFORE
    going to the next. so the caller is the one deleting its lib_session_exit
    to explicitly show that it is ok to continue the lib_session_t exiting process.

*/

/* system include */
/* local include */
#include "neoip_lib_session_exit.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

// definition of \ref lib_session_exit_t constant
const size_t	lib_session_exit_t::EXIT_ORDER_BT_SWARM		= 10;
const size_t	lib_session_exit_t::EXIT_ORDER_APPS		= 50;
const size_t	lib_session_exit_t::EXIT_ORDER_UPNP_WATCH	= 99;
// end of constants definition
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
lib_session_exit_t::lib_session_exit_t()	throw()
{
	// zero some field
	lib_session	= NULL;
	m_in_exiting	= false;
	m_is_notified	= false;
}

/** \brief Destructor
 */
lib_session_exit_t::~lib_session_exit_t()	throw()
{
	// unlink this object from the lib_session_t
	if( lib_session )	lib_session->exit_unlink(this);	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
void	lib_session_exit_t::start(lib_session_t *lib_session, size_t _exit_order
			, lib_session_exit_cb_t *callback, void *userptr)	throw()
{
	// copy the parameter
	this->lib_session	= lib_session;
	this->m_exit_order	= _exit_order;
	this->callback		= callback;
	this->userptr		= userptr;
	// link this object to the lib_session_t
	lib_session->exit_dolink(this);
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief called by lib_session_t when this lib_session_exit_t should begin its exiting
 * 
 * - WARNING: this function may delete the object
 */
void	lib_session_exit_t::begin_exiting()		throw()
{
	// log to debug
	KLOG_DBG("enter exit_order=" << exit_order());
	// sanity check - lib_session_exit MUST NOT be in_exiting() already
	DBG_ASSERT( in_exiting() == false );
	// set the m_in_exiting
	m_in_exiting	= true;
	// launch the zerotimer_t - just to avoid the insync notification
	DBG_ASSERT( zerotimer.empty() );
	zerotimer.append(this, NULL);	
}

/** \brief callback called when the \ref zerotimer_t expire
 */
bool	lib_session_exit_t::neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw()
{
	// log to debug
	KLOG_DBG("enter exit_order=" << exit_order());
	// set the m_is_notified
	m_is_notified	= true;
	// notify the object owner
	return notify_callback();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     main notification function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the callback for the caller
 */
bool lib_session_exit_t::notify_callback()	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_lib_session_exit_cb(userptr, *this);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END;




