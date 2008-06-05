/*! \file
    \brief Header of the \ref nunit_event_t callback

*/


#ifndef __NEOIP_NUNIT_EVENT_CB_HPP__ 
#define __NEOIP_NUNIT_EVENT_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class nunit_event_t;
class nunit_path_t;

/** \brief the callback class for reporting nunit_event_t
 * 
 * - this is used to report the nunit_res_t inside the nunit layer
 */
class nunit_event_cb_t {
public:
	/** \brief called when a nunit_res_t has to be notified
	 * 
	 * @return a 'tokeep'
	 */
	virtual bool neoip_nunit_event_cb(void *userptr, const nunit_path_t &subtest_path
					, const nunit_event_t &nunit_event)	throw() = 0;
	virtual ~nunit_event_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NUNIT_EVENT_CB_HPP__  */



