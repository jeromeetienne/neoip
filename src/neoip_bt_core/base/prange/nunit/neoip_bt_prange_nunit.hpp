/*! \file
    \brief Header of the test of bt_prange_t
*/


#ifndef __NEOIP_BT_OFFPIECE_NUNIT_HPP__ 
#define __NEOIP_BT_OFFPIECE_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the bt_prange_t
 */
class bt_prange_testclass_t : public nunit_testclass_api_t {
private:
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	comparison(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_OFFPIECE_NUNIT_HPP__  */



