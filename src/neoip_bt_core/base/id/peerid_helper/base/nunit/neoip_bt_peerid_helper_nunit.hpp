/*! \file
    \brief Header of the test of bt_peerid_helper_t
*/


#ifndef __NEOIP_BT_PEERID_HELPER_NUNIT_HPP__ 
#define __NEOIP_BT_PEERID_HELPER_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the bt_peerid_helper_t
 */
class bt_peerid_helper_testclass_t : public nunit_testclass_api_t {
private:
	bool		is_buildparse_consistent(const std::string &progname
						, const std::string &progvers)	throw();
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PEERID_HELPER_NUNIT_HPP__  */



