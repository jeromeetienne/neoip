/*! \file
    \brief Header of the unit test of the ndiag_aview_t layer
*/


#ifndef __NEOIP_NDIAG_AVIEW_NUNIT_HPP__ 
#define __NEOIP_NDIAG_AVIEW_NUNIT_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_ndiag_aview_cb.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the ndiag_aview_listener_t
 */
class ndiag_aview_testclass_t : public nunit_testclass_api_t, private ndiag_aview_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	
	/*************** ndiag_aview_t	***************************************/
	std::list<ndiag_aview_t *>	ndiag_aview_db;
	bool 		neoip_ndiag_aview_cb(void *cb_userptr, ndiag_aview_t &cb_ndiag_aview
					, const ipport_addr_t &new_ipport_pview)	throw();
public:
	/*************** ctor/dtor	***************************************/
	ndiag_aview_testclass_t()	throw();
	~ndiag_aview_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NDIAG_AVIEW_NUNIT_HPP__  */



