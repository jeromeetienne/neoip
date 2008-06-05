/*! \file
    \brief Header of the test of file_utils_t
*/


#ifndef __NEOIP_FILE_UTILS_NUNIT_HPP__ 
#define __NEOIP_FILE_UTILS_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the file_utils_t
 */
class file_utils_testclass_t : public nunit_testclass_api_t {
private:
public:
	/*************** nunit test function	*******************************/
	nunit_res_t	get_current_dir(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	get_home_dir(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	create_remove_dir_no_recursion(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	create_remove_dir_do_recursion(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	umask(const nunit_testclass_ftor_t &testclass_ftor)		throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_UTILS_NUNIT_HPP__  */



