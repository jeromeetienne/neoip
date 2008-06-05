/*! \file
    \brief Declaration of the upnp_getportendian_test_t callback
    
*/

#ifndef __NEOIP_UPNP_GETPORTENDIAN_TEST_CB_HPP__ 
#define __NEOIP_UPNP_GETPORTENDIAN_TEST_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	upnp_getportendian_test_t;
class	upnp_err_t;

/** \brief the callback class for upnp_getportendian_test_t
 */
class upnp_getportendian_test_cb_t {
public:
	/** \brief callback notified by \ref upnp_getportendian_test_t on completion
	 */
	virtual bool neoip_upnp_getportendian_test_cb(void *cb_userptr, upnp_getportendian_test_t &cb_getportendian_test
					, const upnp_err_t &upnp_err)	throw() = 0;
	virtual ~upnp_getportendian_test_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_UPNP_GETPORTENDIAN_TEST_CB_HPP__  */



