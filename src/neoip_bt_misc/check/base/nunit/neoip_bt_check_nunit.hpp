/*! \file
    \brief Header of the test of bt_check_t
*/


#ifndef __NEOIP_BT_CHECK_NUNIT_HPP__ 
#define __NEOIP_BT_CHECK_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_bt_check_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	bt_io_vapi_t;

/** \brief Class which implement a nunit for the bt_check_t
 */
class bt_check_testclass_t : public nunit_testclass_api_t, private bt_check_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	bt_io_vapi_t *		bt_io_vapi;	//!< the bt_io_vapi_t to use
	/*************** bt_check_t	*******************************/
	bt_check_t *		bt_check;
	bool 			neoip_bt_check_cb(void *cb_userptr, bt_check_t &cb_bt_check, const bt_err_t &bt_err
					, const bt_swarm_resumedata_t &swarm_resumedata)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_check_testclass_t()	throw();
	~bt_check_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_CHECK_NUNIT_HPP__  */



