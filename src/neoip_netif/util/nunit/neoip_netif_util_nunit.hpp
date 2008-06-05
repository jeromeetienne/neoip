/*! \file
    \brief Header of the unit test of the netif_util_t layer
*/


#ifndef __NEOIP_NETIF_UTIL_NUNIT_HPP__ 
#define __NEOIP_NETIF_UTIL_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the netif_util_t
 */
class netif_util_testclass_t : public nunit_testclass_api_t {
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NETIF_UTIL_NUNIT_HPP__  */



