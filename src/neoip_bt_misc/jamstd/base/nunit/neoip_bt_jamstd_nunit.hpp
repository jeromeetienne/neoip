/*! \file
    \brief Header of the test of bt_jamstd_t
*/


#ifndef __NEOIP_BT_JAMSTD_NUNIT_HPP__ 
#define __NEOIP_BT_JAMSTD_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the bt_jamstd_t
 */
class bt_jamstd_testclass_t : public nunit_testclass_api_t {
private:
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	connection_esta(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	resp_detect_plainbt(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_JAMSTD_NUNIT_HPP__  */



