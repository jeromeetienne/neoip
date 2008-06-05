/*! \file
    \brief Definition of the unit test for the \ref event_hook_t
*/

/* system include */
/* local include */
#include "neoip_event_hook_nunit.hpp"
#include "neoip_event_hook.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

static int nunit_event_hook_var;
/** \brief basic function able to receive event_hook_t for nunit
 */
class nunit_event_hook_t : public event_hook_cb_t {
	bool neoip_event_hook_notify_cb(void *userptr, const event_hook_t *cb_event_hook, int hook_level) throw(){
		KLOG_DBG("notified event " << hook_level);
		nunit_event_hook_var	= hook_level;
		return true;
	}
};

/** \brief function to test a event_hook_t
 */
nunit_res_t	event_hook_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	nunit_res_t		nunit_res;
	nunit_event_hook_t	nunit_event_hook;
	event_hook_t		event_hook = event_hook_t(2);

	// add 2 events on 2 class
	event_hook.append(0, &nunit_event_hook, NULL);
	event_hook.append(1, &nunit_event_hook, NULL);

	// notify the first event
	nunit_event_hook_var = -1;
	event_hook.notify(0);
	NUNIT_ASSERT( nunit_event_hook_var == 0 );

	// notify the second event
	nunit_event_hook_var = -1;
	event_hook.notify(1);
	NUNIT_ASSERT( nunit_event_hook_var == 1 );

	// remove 2 events on 2 class
	event_hook.remove(0, &nunit_event_hook, NULL);
	event_hook.remove(1, &nunit_event_hook, NULL);	

	// return no error
	return NUNIT_RES_OK;
}




NEOIP_NAMESPACE_END

