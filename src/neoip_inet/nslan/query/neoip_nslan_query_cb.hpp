/*! \file
    \brief Header of the nslan_query_cb_t
*/


#ifndef __NEOIP_NSLAN_QUERY_CB_HPP__ 
#define __NEOIP_NSLAN_QUERY_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class nslan_event_t;
class nslan_query_t;

/** \brief the callback class for \ref nslan_query_t
 */
class nslan_query_cb_t {
public:
	/** \brief callback notified when nslan_query_t has an event to report
	 * 
	 * @return a 'tokeep' aka false if the query has been deleted, true otherwise
	 */
	virtual bool neoip_nslan_query_cb(void *cb_userptr, nslan_query_t &cb_nslan_query
					, const nslan_event_t &nslan_event)	throw() = 0;
	virtual ~nslan_query_cb_t() {};
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NSLAN_QUERY_CB_HPP__  */



