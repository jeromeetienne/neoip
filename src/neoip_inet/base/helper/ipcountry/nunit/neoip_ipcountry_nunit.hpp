/*! \file
    \brief Header of the test of ipcountry_t
*/


#ifndef __NEOIP_IPCOUNTRY_NUNIT_HPP__ 
#define __NEOIP_IPCOUNTRY_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_ipcountry_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the ipcountry_t
 * 
 * - currently it only test if the connection works
 */
class ipcountry_testclass_t : public nunit_testclass_api_t, private ipcountry_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 

	/*************** ipcountry_t	***************************************/
	ipcountry_t *	ipcountry;
	bool		neoip_ipcountry_cb(void *cb_userptr, ipcountry_t &cb_ipcountry
				, const inet_err_t &inet_err, const std::string &country_code)	throw();
public:
	/*************** ctor/dtor	***************************************/
	ipcountry_testclass_t()	throw();
	~ipcountry_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_IPCOUNTRY_NUNIT_HPP__  */



