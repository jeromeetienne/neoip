/*! \file
    \brief unit test for the upnp_watch_t unit testt

*/

/* system include */
/* local include */
#include "neoip_upnp_watch_nunit.hpp"
#include "neoip_upnp_watch.hpp"
#include "neoip_upnp_watch_event.hpp"
#include "neoip_upnp_bindport.hpp"
#include "neoip_upnp_err.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
upnp_watch_testclass_t::upnp_watch_testclass_t()	throw()
{
	// zero some field
	upnp_watch	= NULL;
	upnp_bindport	= NULL;
}

/** \brief Destructor
 */
upnp_watch_testclass_t::~upnp_watch_testclass_t()	throw()
{
	// deinit the testclass just in case
	neoip_nunit_testclass_deinit();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit init/deinit
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Init the testclass implementation
 */
nunit_err_t	upnp_watch_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	upnp_watch_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the upnp_bindport_t
	nipmem_zdelete	upnp_bindport;
	// delete the upnp_watch
	nipmem_zdelete	upnp_watch;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief general test 
 */
nunit_res_t	upnp_watch_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	upnp_err_t upnp_err;
	// log to debug
	KLOG_DBG("enter");
	// sanity check - upnp_watch MUST NOT be already init
	DBG_ASSERT( !upnp_watch );
	
	// create a upnp_watch_listener_t
	upnp_watch	= nipmem_new upnp_watch_t();
	upnp_err	= upnp_watch->start(this, NULL);
	if( upnp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, upnp_err.to_string());
	
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			upnp_watch_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref upnp_watch_t to notify an event
 */
bool 	upnp_watch_testclass_t::neoip_upnp_watch_cb(void *cb_userptr, upnp_watch_t &cb_upnp_watch
						, const upnp_watch_event_t &watch_event)	throw()
{
	// log to debug
	KLOG_ERR("enter upnp_watch_event=" << watch_event);

	// handle each possible events from its type
	switch( watch_event.get_value() ){
	case upnp_watch_event_t::UPNPDISC_CHANGED:
		KLOG_ERR("now upnp_isavail()=" << upnp_watch->upnp_isavail());
		KLOG_ERR("now current_disc_res=" << upnp_watch->current_disc_res());
		break;
	case upnp_watch_event_t::EXTIPADDR_CHANGED:
		KLOG_ERR("now extipaddr()=" << upnp_watch->extipaddr());
		break;
	default:	DBG_ASSERT( 0 );
	}


	// start the upnp_bindport_t
	if( !upnp_bindport ){
		upnp_err_t	upnp_err;
		upnp_bindport	= nipmem_new upnp_bindport_t();
		upnp_err	= upnp_bindport->start(upnp_watch, ":1234", upnp_sockfam_t::TCP, ":27360"
							, "neoip upnp_bindport_t nunit", this, NULL); 
		DBG_ASSERT( upnp_err.succeed() );
	}
	// return tokeep
	return true;
}

/** \brief callback notified by \ref upnp_bindport_t on completion
 */
bool	upnp_watch_testclass_t::neoip_upnp_bindport_cb(void *cb_userptr, upnp_bindport_t &cb_upnp_bindport
			, const upnp_err_t &upnp_err, const ipport_addr_t &ipport_pview)	throw()
{
	KLOG_ERR("enter upnp_err=" << upnp_err << " ipport_pview=" << ipport_pview);
#if 1
	return true;
#else
	// notify the result to nunit
	if( upnp_err.failed() )	return nunit_ftor(NUNIT_RES_ERROR);
	else			return nunit_ftor(NUNIT_RES_OK);
#endif
}

NEOIP_NAMESPACE_END
