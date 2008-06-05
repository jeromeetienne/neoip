/*! \file
    \brief Definition of the unit test for the \ref event_hook_t and co

*/

/* system include */
#include <map>
/* local include */
#include "neoip_event_hook_utest.hpp"
#include "neoip_event_hook.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

using namespace neoip;


static int utest_event_hook_var;
/** \brief basic function able to receive event_hook_t for utest
 */
class utest_event_hook_t : public event_hook_cb_t {
	bool neoip_event_hook_notify_cb(void *userptr, const event_hook_t *cb_event_hook, int hook_level) throw(){
		KLOG_DBG("notified event " << hook_level);
		utest_event_hook_var	= hook_level;
		return true;
	}
};

/** \brief unit test for the event_hook_t class
 */
int neoip_event_hook_utest()
{
	int			n_error = 0;
	utest_event_hook_t	utest_event_hook;
	event_hook_t		event_hook = event_hook_t(2);

	// add 2 events on 2 class
	event_hook.append(0, &utest_event_hook, NULL);
	event_hook.append(1, &utest_event_hook, NULL);

	// notify the first event
	utest_event_hook_var = -1;
	event_hook.notify(0);
	if( utest_event_hook_var != 0 ){
		KLOG_ERR("event_hook.notify() is BUGGED!");
		n_error++;
		goto error;	
	}

	// notify the second event
	utest_event_hook_var = -1;
	event_hook.notify(1);
	if( utest_event_hook_var != 1 ){
		KLOG_ERR("event_hook.notify() is BUGGED!");
		n_error++;
		goto error;	
	}

	if( n_error )	goto error;
	KLOG_ERR("event_hook_t unit test PASSED!!!");

	return n_error;
error:;	KLOG_ERR("event_hook_t unit test FAILED!!!");
	return 1;
}

