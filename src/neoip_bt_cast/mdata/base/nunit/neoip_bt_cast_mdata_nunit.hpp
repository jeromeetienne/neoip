/*! \file
    \brief Header of the test of bt_cast_mdata_server_t
*/


#ifndef __NEOIP_BT_CAST_MDATA_NUNIT_HPP__ 
#define __NEOIP_BT_CAST_MDATA_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_bt_cast_mdata_server_cb.hpp"
#include "neoip_bt_cast_mdata_client_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	http_listener_t;

/** \brief Class which implement a nunit for the bt_cast_mdata_server_t
 */
class bt_cast_mdata_testclass_t : public nunit_testclass_api_t
					, private bt_cast_mdata_server_cb_t 
					, private bt_cast_mdata_client_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	http_listener_t *	m_http_listener;//!< the http_listener_t

	/*************** bt_cast_mdata_server_t	*******************************/
	bt_cast_mdata_server_t *mdata_server;
	bool			neoip_bt_cast_mdata_server_cb(void *cb_userptr
					, bt_cast_mdata_server_t &cb_mdata_server
					, const bt_cast_id_t &cast_id
					, bt_cast_mdata_t *cast_mdata_out)	throw();

	/*************** bt_cast_mdata_client_t	*******************************/
	bt_cast_mdata_client_t *mdata_client;
	bool			neoip_bt_cast_mdata_client_cb(void *cb_userptr, bt_cast_mdata_client_t &cb_mdata_client
					, const bt_err_t &bt_err, const bt_cast_mdata_t &cast_mdata)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_cast_mdata_testclass_t()	throw();
	~bt_cast_mdata_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_CAST_MDATA_NUNIT_HPP__  */



