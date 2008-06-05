/*! \file
    \brief Declaration of the flv_parse_t
    
*/


#ifndef __NEOIP_FLV_PARSE_CB_HPP__ 
#define __NEOIP_FLV_PARSE_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	flv_parse_t;
class	flv_parse_event_t;

/** \brief the callback class for flv_parse_t
 */
class flv_parse_cb_t {
public:
	/** \brief callback notified by \ref flv_parse_t when to notify an event
	 * 
	 * @return true if the flv_parse_t is still valid after the callback
	 */
	virtual bool neoip_flv_parse_cb(void *cb_userptr, flv_parse_t &cb_flv_parse
					, const flv_parse_event_t &parse_event)	throw() = 0;
	// virtual destructor
	virtual ~flv_parse_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FLV_PARSE_CB_HPP__  */



