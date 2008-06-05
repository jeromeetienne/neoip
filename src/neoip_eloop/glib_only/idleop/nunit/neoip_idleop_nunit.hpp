/*! \file
    \brief Header of the test of idleop_t
*/


#ifndef __NEOIP_IDLEOP_NUNIT_HPP__ 
#define __NEOIP_IDLEOP_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_idleop_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the idleop_t
 */
class idleop_testclass_t : public nunit_testclass_api_t, private idleop_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 

	/*************** idleop_t	***************************************/
	idleop_t *		idleop;
	bool 			neoip_idleop_cb(void *cb_userptr, idleop_t &cb_idleop)	throw();
public:
	/*************** ctor/dtor	***************************************/
	idleop_testclass_t()	throw();
	~idleop_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_IDLEOP_NUNIT_HPP__  */



