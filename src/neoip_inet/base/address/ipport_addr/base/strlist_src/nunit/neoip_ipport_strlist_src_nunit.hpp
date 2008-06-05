/*! \file
    \brief Header of the unit test of the ipport_strlist_src_t layer
*/


#ifndef __NEOIP_IPPORT_STRLIST_SRC_NUNIT_HPP__ 
#define __NEOIP_IPPORT_STRLIST_SRC_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_ipport_strlist_src_cb.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ipport_strlist_src_t;

/** \brief Class which implement a nunit for the gen_id_t
 */
class ipport_strlist_src_testclass_t : public nunit_testclass_api_t, private ipport_strlist_src_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	
	ipport_strlist_src_t *	strlist_src;	//!< the ipport_strlist_src_t which is tested
	bool 	neoip_ipport_strlist_src_cb(void *cb_userptr, ipport_strlist_src_t &cb_ipport_strlist_src
					, const ipport_addr_t &ipport_addr)	throw();
public:
	/*************** ctor/dtor	***************************************/
	ipport_strlist_src_testclass_t()	throw();
	~ipport_strlist_src_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)			throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_IPPORT_STRLIST_SRC_NUNIT_HPP__  */



