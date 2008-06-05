/*! \file
    \brief Header of the bt_tracker_client_t
    
*/


#ifndef __NEOIP_BT_TRACKER_CLIENT_HPP__ 
#define __NEOIP_BT_TRACKER_CLIENT_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_tracker_client_profile.hpp"
#include "neoip_bt_tracker_client_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_http_sclient_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


// list of forward declaration
class	bt_tracker_request_t;
class	bt_tracker_reply_t;
class	socket_itor_arg_t;

/** \brief class definition for bt_tracker_client
 */
class bt_tracker_client_t : NEOIP_COPY_CTOR_DENY, private http_sclient_cb_t {
private:
	bt_tracker_client_profile_t	profile;
	socket_itor_arg_t *	itor_arg;	//!< store the caller socket_itor_arg_t during the setup

	/*************** http_client_t	*******************************/
	http_sclient_t *	http_sclient;
	bool 			neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_http_sclient
							, const http_sclient_res_t &sclient_res) throw();

	/*************** callback stuff	***************************************/
	bt_tracker_client_cb_t *callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const bt_err_t &bt_err
						, const bt_tracker_reply_t &reply)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback	
public:
	/*************** ctor/dtor	***************************************/
	bt_tracker_client_t() 		throw();
	~bt_tracker_client_t()		throw();

	/*************** Setup function	***************************************/
	bt_tracker_client_t &	set_profile(const bt_tracker_client_profile_t &profile)	throw();	
	bt_tracker_client_t &	set_itor_arg(const socket_itor_arg_t &itor_arg)			throw();
	bt_err_t		start(const bt_tracker_request_t &request,bt_tracker_client_cb_t *callback
							, void *userptr)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_TRACKER_CLIENT_HPP__  */



