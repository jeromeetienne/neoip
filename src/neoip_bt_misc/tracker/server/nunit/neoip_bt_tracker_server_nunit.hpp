/*! \file
    \brief Header of the test of bt_tracker_server_t
*/


#ifndef __NEOIP_BT_TRACKER_SERVER_NUNIT_HPP__ 
#define __NEOIP_BT_TRACKER_SERVER_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_bt_tracker_server_cb.hpp"
#include "neoip_bt_tracker_client_cb.hpp"
#include "neoip_bt_tracker_request.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_slotpool_id.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	httpd_t;

/** \brief Class which implement a nunit for the bt_tracker_server_t
 */
class bt_tracker_server_testclass_t : public nunit_testclass_api_t, private bt_tracker_server_cb_t
					, private bt_tracker_client_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	httpd_t	*		httpd;		//!< the http server to receive the bt_tracker_request_t

	
	/*************** bt_tracker_server_t	*******************************/
	bt_tracker_server_t *	bt_tracker_server;
	bool			neoip_bt_tracker_server_cb(void *cb_userptr, bt_tracker_server_t &cb_bt_tracker_server
						, slot_id_t cnx_slotid, const bt_tracker_request_t &request
						, bt_tracker_reply_t &reply_out)	throw();

	/*************** bt_tracker_client_t	*******************************/
	bt_tracker_client_t *	bt_tracker_client;
	bool 			neoip_bt_tracker_client_cb(void *cb_userptr
						, bt_tracker_client_t &cb_bt_tracker_client
						, const bt_err_t &bt_err
						, const bt_tracker_reply_t &reply)	throw();	
public:
	/*************** ctor/dtor	***************************************/
	bt_tracker_server_testclass_t()	throw();
	~bt_tracker_server_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	general(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_TRACKER_SERVER_NUNIT_HPP__  */



