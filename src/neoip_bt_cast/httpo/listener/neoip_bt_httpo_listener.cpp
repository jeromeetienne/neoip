/*! \file
    \brief Definition of the \ref bt_httpo_listener_t

\par Brief description
- bt_httpo_listener_t provides the infrastructure to export the contents of a bt_swarm_t
  via http connection (instead of the typical write the content on disk)
- those http connections, bt_httpo_full_t, may be seekable or not.
  - aka there is a support for http range-request 

*/

/* system include */
/* local include */
#include "neoip_bt_httpo_listener.hpp"
#include "neoip_bt_httpo_resp.hpp"
#include "neoip_bt_httpo_full.hpp"
#include "neoip_http_listener.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Constructor
 */
bt_httpo_listener_t::bt_httpo_listener_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some field
	http_listener	= NULL;	
}

/** \brief Destructor
 */
bt_httpo_listener_t::~bt_httpo_listener_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete all the bt_httpo_full_t if needed
	while( !httpo_full_db.empty() )	nipmem_delete httpo_full_db.front();		
	// sanity check - all bt_httpo_resp_t MUST have been deleted before
	DBG_ASSERT( httpo_resp_db.empty() );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Start the operation
 */
bt_err_t	bt_httpo_listener_t::start(http_listener_t *http_listener)	throw()
{
	// copy the parameter
	this->http_listener	= http_listener;
	// return no error
	return bt_err_t::OK;
}



NEOIP_NAMESPACE_END;




