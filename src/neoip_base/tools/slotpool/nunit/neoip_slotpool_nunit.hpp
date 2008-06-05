/*! \file
    \brief Header of the unit test of slotpool_t
*/


#ifndef __NEOIP_SLOTPOOL_NUNIT_HPP__ 
#define __NEOIP_SLOTPOOL_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the slotpool_t
 */
class slotpool_testclass_t : public nunit_testclass_api_t {
private:
public:
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SLOTPOOL_NUNIT_HPP__  */



