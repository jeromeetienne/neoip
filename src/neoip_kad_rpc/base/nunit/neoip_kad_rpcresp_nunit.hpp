/*! \file
    \brief Header of the test of kad_rpcresp_t
*/


#ifndef __NEOIP_KAD_RPCRESP_NUNIT_HPP__ 
#define __NEOIP_KAD_RPCRESP_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	kad_rpcresp_t;

/** \brief Class which implement a nunit for the kad_rpcresp_t
 */
class kad_rpcresp_testclass_t : public nunit_testclass_api_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	kad_rpcresp_t *		kad_rpcresp;
public:
	/*************** ctor/dtor	***************************************/
	kad_rpcresp_testclass_t()	throw();
	~kad_rpcresp_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_RPCRESP_NUNIT_HPP__  */



