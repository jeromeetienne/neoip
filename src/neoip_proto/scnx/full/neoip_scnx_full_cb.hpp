/*! \file
    \brief Header of the \ref scnx_full_t 's callback
*/


#ifndef __NEOIP_SCNX_FULL_CB_HPP__ 
#define __NEOIP_SCNX_FULL_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	scnx_full_t;
class	scnx_event_t;

/** \brief the callback class for \ref scnx_full_t
 */
class scnx_full_cb_t {
public:
	/** \brief receive \ref scnx_event_t from \ref scnx_full_t
	 */
	virtual bool neoip_scnx_full_event_cb(void *cb_userptr, scnx_full_t &cb_scnx_full
							, const scnx_event_t &scnx_event ) throw() = 0;
	virtual ~scnx_full_cb_t() {};
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SCNX_FULL_CB_HPP__  */



