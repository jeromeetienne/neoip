/*! \file
    \brief Header of the \ref scnx_itor_t 's callback

*/


#ifndef __NEOIP_SCNX_ITOR_CB_HPP__ 
#define __NEOIP_SCNX_ITOR_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class scnx_itor_t;
class scnx_event_t;

/** \brief The \ref scnx_itor_t callbacks
 */
class scnx_itor_cb_t {
public:
	/** \brief receive \ref scnx_event_t from \ref scnx_itor_t
	 */
	virtual bool neoip_scnx_itor_event_cb(void *cb_userptr, scnx_itor_t &cb_scnx_itor
						, const scnx_event_t &scnx_event) throw() = 0;
	//! virtual destructor
	virtual ~scnx_itor_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SCNX_ITOR_CB_HPP__  */



