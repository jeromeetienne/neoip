/*! \file
    \brief Header of the neoip_dnsgrab class
    
*/


#ifndef __NEOIP_DNSGRAB_CB_HPP__ 
#define __NEOIP_DNSGRAB_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class dnsgrab_request_t;
class dnsgrab_t;

/** \brief the callback class for dnsgrab_request_t
 */
class dnsgrab_cb_t {
public:
	/** \brief callback notified when a dnsgrab request is received by this \ref dnsgrab_cb_t
	 * 
	 * @return a 'requestOK' aka if true, the just received request will 
	 *         be notified layer, if false, the request is to be discarded
	 *         immediatly
	 */
	virtual bool neoip_dnsgrab_cb(void *cb_userptr, dnsgrab_t &cb_dnsgrab, dnsgrab_request_t &request)	throw() = 0;
	virtual ~dnsgrab_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DNSGRAB_CB_HPP__  */



