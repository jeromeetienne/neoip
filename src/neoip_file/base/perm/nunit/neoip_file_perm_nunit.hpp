/*! \file
    \brief Header of the test of file_perm_t
*/


#ifndef __NEOIP_FILE_PERM_NUNIT_HPP__ 
#define __NEOIP_FILE_PERM_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the file_perm_t
 */
class file_perm_testclass_t : public nunit_testclass_api_t {
private:
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	shortcut(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	to_octal(const nunit_testclass_ftor_t &testclass_ftor)		throw();
	nunit_res_t	from_octal(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_PERM_NUNIT_HPP__  */



