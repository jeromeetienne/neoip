/*! \file
    \brief Header of the unit test of timeout_t
*/


#ifndef __NEOIP_EXPIRESET2_NUNIT_HPP__ 
#define __NEOIP_EXPIRESET2_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_expireset2.hpp"
#include "neoip_timeout.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the expireset2_t
 */
class expireset2_testclass_t : public nunit_testclass_api_t, private timeout_cb_t {
private:
	nunit_testclass_ftor_t		nunit_ftor; //!< the functor to notify nunit_res_t asynchronously 
	expireset2_t<std::string> *	expireset;

	timeout_t	timeout;	//!< the timeout to test the async notification of the nunit_res_t
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	/*************** ctor/dtor	***************************************/
	expireset2_testclass_t()		throw();
	~expireset2_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();	

	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)			throw();
	nunit_res_t	serial_consistency(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_EXPIRESET2_NUNIT_HPP__  */



