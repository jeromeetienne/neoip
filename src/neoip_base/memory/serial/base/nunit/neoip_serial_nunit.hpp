/*! \file
    \brief Header of the test of serial_t
*/


#ifndef __NEOIP_SERIAL_NUNIT_HPP__ 
#define __NEOIP_SERIAL_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the serial_t
 */
class serial_testclass_t : public nunit_testclass_api_t {
private:
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	basic_serial(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	failed_serial(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	headtail_freelen(const nunit_testclass_ftor_t &testclass_ftor)		throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SERIAL_NUNIT_HPP__  */



