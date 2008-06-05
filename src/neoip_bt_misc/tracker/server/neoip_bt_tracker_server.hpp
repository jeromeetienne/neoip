/*! \file
    \brief Header of the bt_tracker_server_t
    
*/


#ifndef __NEOIP_BT_TRACKER_SERVER_HPP__ 
#define __NEOIP_BT_TRACKER_SERVER_HPP__ 
/* system include */
#include <map>
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_bt_tracker_server_cb.hpp"
#include "neoip_httpd_handler_cb.hpp"
#include "neoip_httpd_request.hpp"
#include "neoip_slotpool_id.hpp"
#include "neoip_file_path.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


// list of forward declaration
class	bt_tracker_reply_t;

/** \brief class definition for bt_tracker_server
 */
class bt_tracker_server_t : NEOIP_COPY_CTOR_DENY, private httpd_handler_cb_t {
private:
	file_path_t	handler_path;	//!< the path for the http handler

	std::map<slot_id_t, httpd_request_t>	httpd_request_db;	// all the pending httpd_request

	/*************** http handler	***************************************/
	httpd_t	*	httpd;	//!< pointer on the httpd_t on which the handler is registered
	httpd_err_t	neoip_httpd_handler_cb(void *cb_userptr, httpd_request_t &request)	throw();
	
	/*************** callback stuff	***************************************/
	bt_tracker_server_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(slot_id_t cnx_slotid, const bt_tracker_request_t &request
							, bt_tracker_reply_t &reply_out)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback	
public:
	/*************** ctor/dtor	***************************************/
	bt_tracker_server_t() 		throw();
	~bt_tracker_server_t()		throw();

	/*************** Setup function	***************************************/
	bt_err_t	start(httpd_t *httpd, const file_path_t &handler_path
					, bt_tracker_server_cb_t *callback, void *userptr)	throw();

	/*************** action function	*******************************/
	void		notify_reply(slot_id_t cnx_slotid, const bt_tracker_request_t &request
							, const bt_tracker_reply_t &reply) throw();	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_TRACKER_SERVER_HPP__  */



