/*! \file
    \brief Header of the unit test of the asyncexe_t layer
*/


#ifndef __NEOIP_ASYNCEXE_NUNIT_HPP__ 
#define __NEOIP_ASYNCEXE_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_asyncexe_cb.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the asyncexe_listener_t
 */
class asyncexe_testclass_t : public nunit_testclass_api_t, private asyncexe_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	
	/*************** asyncexe	***************************************/
	asyncexe_t *	asyncexe;
	bool		neoip_asyncexe_cb(void *cb_userptr, asyncexe_t &cb_asyncexe
					, const libsess_err_t &libsess_err, const bytearray_t &stdout_barray
					, const int &exit_status)	throw();
public:
	/*************** ctor/dtor	***************************************/
	asyncexe_testclass_t()	throw();
	~asyncexe_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ASYNCEXE_NUNIT_HPP__  */



