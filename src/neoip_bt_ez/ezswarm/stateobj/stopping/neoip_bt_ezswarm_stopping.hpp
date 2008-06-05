/*! \file
    \brief Header of the bt_ezswarm_stopping_t
    
*/


#ifndef __NEOIP_BT_EZSWARM_STOPPING_HPP__ 
#define __NEOIP_BT_EZSWARM_STOPPING_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_ezswarm_stopping_wikidbg.hpp"
#include "neoip_bt_tracker_client_cb.hpp"
#include "neoip_kad_delete_cb.hpp"
#include "neoip_bt_io_stopping_cb.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declarations
class	bt_ezswarm_t;
class	bt_err_t;
class	bt_tracker_request_t;

/** \brief class definition for bt_ezswarm_stopping_t
 */
class bt_ezswarm_stopping_t : NEOIP_COPY_CTOR_DENY, private zerotimer_cb_t
			, private bt_tracker_client_cb_t, private kad_delete_cb_t
			, private bt_io_stopping_cb_t
			, private wikidbg_obj_t<bt_ezswarm_stopping_t, bt_ezswarm_stopping_wikidbg_init> {
private:
	bt_ezswarm_t *		bt_ezswarm;	//!< backpointer to bt_ezswarm_t

	/*************** Internal function	*******************************/
	bt_tracker_request_t	build_tracker_request()	const throw();
	bool			is_completed()		const throw();

	/*************** zerotimer_t	***************************************/
	zerotimer_t		zerotimer;
	bool			neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)throw();

	/*************** bt_tracker_client_t	*******************************/
	bt_tracker_client_t *	bt_tracker_client;
	bool 			neoip_bt_tracker_client_cb(void *cb_userptr
						, bt_tracker_client_t &cb_bt_tracker_client
						, const bt_err_t &bt_err
						, const bt_tracker_reply_t &reply)	throw();
	/*************** kad_delete_t	***************************************/
	kad_delete_t *		kad_delete;
	bool 			neoip_kad_delete_cb(void *cb_userptr, kad_delete_t &cb_kad_delete
						, const kad_event_t &kad_event)		throw();

	/*************** bt_io_stopping_t	*******************************/
	bt_io_stopping_t *	io_stopping;
	bool 			neoip_bt_io_stopping_cb(void *cb_userptr, bt_io_stopping_t &cb_io_stopping
						, const bt_err_t &bt_err)		throw();
public:
	/**************	ctor/dtor	***************************************/
	bt_ezswarm_stopping_t() 		throw();
	~bt_ezswarm_stopping_t()		throw();
	
	/************** setup function	***************************************/
	bt_err_t	start(bt_ezswarm_t *bt_ezswarm)	throw();

	/*************** list of friend class	*******************************/
	friend class	bt_ezswarm_stopping_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_EZSWARM_STOPPING_HPP__  */



