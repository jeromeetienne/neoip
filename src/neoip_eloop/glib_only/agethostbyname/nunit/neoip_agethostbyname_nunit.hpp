/*! \file
    \brief Header of the unit test of the agethostbyname_t layer
*/


#ifndef __NEOIP_AGETHOSTBYNAME_NUNIT_HPP__
#define __NEOIP_AGETHOSTBYNAME_NUNIT_HPP__
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_agethostbyname_cb.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	agethostbyname_t;

/** \brief Class which implement a nunit for the gen_id_t
 */
class agethostbyname_testclass_t : public nunit_testclass_api_t, private agethostbyname_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	
	/*************** agethostbyname	***************************************/
	agethostbyname_t *	agethostbyname;	//!< the agethostbyname_t which is tested	
	void neoip_agethostbyname_cb(void *cb_userptr, agethostbyname_t &cb_agethostbyname
						, const hostent_t &hostent)	throw();	
public:
	/*************** ctor/dtor	***************************************/
	agethostbyname_testclass_t()	throw();
	~agethostbyname_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	query_localhost(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_AGETHOSTBYNAME_NUNIT_HPP__  */



