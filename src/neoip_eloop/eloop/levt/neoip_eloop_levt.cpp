/*! \file
    \brief Implementation of the eloop_levt_t

*/

#ifdef USE_ELOOP_LEVT
/* system include */
#include <iostream>
#include <event.h>
/* local include */
#include "neoip_eloop_levt.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                           CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Default Constructor
 */
eloop_t::eloop_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// init the libevent 
	event_init();
}

/** \brief Default Constructor
 */
eloop_t::~eloop_t()	throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start to run on the eloop_t
 */
void	eloop_t::loop_run()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// run the loop
	// - NOTE: event_dispatch run until there are no more event or a event_loopexit is called
	event_dispatch();
}

/** \brief Stop to run on the eloop_t
 */
void	eloop_t::loop_stop()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// NOTE: this NULL is a struct timeval pointer meaning 0
	event_loopexit(NULL);
}

NEOIP_NAMESPACE_END
#endif 


