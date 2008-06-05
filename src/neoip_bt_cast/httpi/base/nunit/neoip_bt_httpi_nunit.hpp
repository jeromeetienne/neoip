/*! \file
    \brief Header of the test of bt_httpi_t
*/


#ifndef __NEOIP_BT_HTTPI_NUNIT_HPP__ 
#define __NEOIP_BT_HTTPI_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_bt_httpi_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// forward declaration
class	bt_io_vapi_t;

/** \brief Class which implement a nunit for the bt_httpi_t
 */
class bt_httpi_testclass_t : public nunit_testclass_api_t, private bt_httpi_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	bt_io_vapi_t *		bt_io_vapi;	//!< the bt_io_vapi_t to write the imported data
	 
	/*************** bt_httpi_t	*******************************/
	bt_httpi_t *		bt_httpi;
	bool 			neoip_bt_httpi_cb(void *cb_userptr, bt_httpi_t &cb_bt_httpi
					, const bt_httpi_event_t &httpi_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_httpi_testclass_t()		throw();
	~bt_httpi_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HTTPI_NUNIT_HPP__  */



