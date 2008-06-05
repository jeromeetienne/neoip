/*! \file
    \brief Header of the test of bt_alloc_t
*/


#ifndef __NEOIP_BT_ALLOC_NUNIT_HPP__ 
#define __NEOIP_BT_ALLOC_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_bt_alloc_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the bt_alloc_t
 */
class bt_alloc_testclass_t : public nunit_testclass_api_t, private bt_alloc_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 

	/*************** bt_alloc_t	*******************************/
	bt_alloc_t *		bt_alloc;
	bool 			neoip_bt_alloc_cb(void *cb_userptr, bt_alloc_t &cb_bt_alloc
					, const bt_err_t &bt_err) throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_alloc_testclass_t()	throw();
	~bt_alloc_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_ALLOC_NUNIT_HPP__  */



