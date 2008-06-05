/*! \file
    \brief Definition of the unit test for the \ref socket_itor_t, \ref socket_resp_t and co

\par Brief description
This module performs an unit test for the socket classes

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_udp_vresp_utest.hpp"
#include "neoip_udp_vresp.hpp"
#include "neoip_udp.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

using namespace neoip;


NEOIP_NAMESPACE_BEGIN;

class udp_vresp_utest_t : private udp_vresp_cb_t, private udp_full_cb_t {
private:
	udp_vresp_t *	udp_vresp;
	
	/** \brief callback notified by udp_vresp_t when the result is known
	 */
	bool neoip_inet_udp_vresp_event_cb(void *cb_userptr, udp_vresp_t &cb_udp_vresp
							, const udp_event_t &udp_event)	throw() 
	{
		// sanity check - the event MUST be resp_ok
		DBG_ASSERT( udp_event.is_resp_ok() );
		// display the result
		KLOG_ERR("udp_vresp notified udp_event=" << udp_event);
		// return 'tokeep'	
		return true;
	}
	
	udp_full_t *	udp_full;
	bool		neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
						, const udp_event_t &udp_event)		throw()
	{
		// display the result
		KLOG_ERR("udp_full notified udp_event=" << udp_event);
		KLOG_ERR("udp_full slot_id=" << cb_udp_full.get_object_slotid() );
		return true;		
	}
	
public:
	/** \brief Constructor
	 */
	udp_vresp_utest_t()		throw()
	{
		inet_err_t	inet_err;
		// log to debug
		KLOG_ERR("enter");
		// create the udp_vresp
		udp_vresp	= nipmem_new udp_vresp_t();
		// setup the listen_addr and start the udp_vresp_t
		inet_err = udp_vresp->start("127.0.0.1:4000");
		DBG_ASSERT( inet_err.succeed() );
		
		// register some callback
		udp_vresp->register_callback('a', this, NULL);
		udp_vresp->register_callback('c', this, NULL);
	}

	/** \brief Destructor
	 */
	~udp_vresp_utest_t()		throw()
	{
		// unregister some callback
		udp_vresp->unregister_callback('a');
		udp_vresp->unregister_callback('c');
		// delete the udp_vresp_t
		nipmem_delete udp_vresp;
	}
};

NEOIP_NAMESPACE_END


/** \brief unit test for the udp_vresp class
 */
int neoip_udp_vresp_utest()
{
	int		n_error		= 0;

	if( n_error )	goto error;
	KLOG_ERR("udp_vresp utest PASSED");
	return 0;
error:;
	KLOG_ERR("udp_vresp utest FAILED!!!!");
	return -1;
}

static udp_vresp_utest_t *	udp_vresp_utest;

/** \brief unit test for the reludp class
 */
int neoip_udp_vresp_utest_start()
{
	// create the udp_vresp_utest
	udp_vresp_utest	= nipmem_new udp_vresp_utest_t();
	// return no error;
	return 0;
}

int neoip_udp_vresp_utest_end()
{
	// log to debug
	KLOG_ERR("enter");
	// delete the udp_vresp_utest
	nipmem_delete udp_vresp_utest;
	// return no error;
	return 0;
}
