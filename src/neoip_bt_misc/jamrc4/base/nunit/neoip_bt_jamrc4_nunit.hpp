/*! \file
    \brief Header of the test of bt_jamrc4_t
*/


#ifndef __NEOIP_BT_JAMRC4_NUNIT_HPP__ 
#define __NEOIP_BT_JAMRC4_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	bt_jamrc4_profile_t;

/** \brief Class which implement a nunit for the bt_jamrc4_t
 */
class bt_jamrc4_testclass_t : public nunit_testclass_api_t {
private:
	nunit_res_t	do_cnx(const bt_jamrc4_profile_t &profile)	throw();
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	cnx_nojam2nojam(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	cnx_dojam2dojam(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	cnx_nojam2dojam(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	cnx_nojam2anyjam(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_JAMRC4_NUNIT_HPP__  */



