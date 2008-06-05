/*! \file
    \brief Header of the test of bt_oload0_t
*/


#ifndef __NEOIP_BT_OLOAD0_NUNIT_HPP__ 
#define __NEOIP_BT_OLOAD0_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	bt_oload0_t;

/** \brief Class which implement a nunit for the bt_oload0_t
 */
class bt_oload0_testclass_t : public nunit_testclass_api_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	bt_oload0_t *		bt_oload0;
public:
	/*************** ctor/dtor	***************************************/
	bt_oload0_testclass_t()	throw();
	~bt_oload0_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_OLOAD0_NUNIT_HPP__  */



