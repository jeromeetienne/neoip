/*! \file
    \brief Header of the bt_ezswarm_check_t
    
*/


#ifndef __NEOIP_BT_EZSWARM_CHECK_HPP__ 
#define __NEOIP_BT_EZSWARM_CHECK_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_ezswarm_check_wikidbg.hpp"
#include "neoip_bt_check_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declarations
class	bt_ezswarm_t;
class	bt_err_t;
class	bt_io_vapi_t;

/** \brief class definition for bt_ezswarm_check_t
 */
class bt_ezswarm_check_t : NEOIP_COPY_CTOR_DENY, private bt_check_cb_t
			, private wikidbg_obj_t<bt_ezswarm_check_t, bt_ezswarm_check_wikidbg_init> {
private:
	bt_ezswarm_t *	bt_ezswarm;	//!< backpointer to bt_ezswarm_t
	
	/*************** bt_check_t	*******************************/
	bt_check_t *	bt_check;
	bool 		neoip_bt_check_cb(void *cb_userptr, bt_check_t &cb_bt_check, const bt_err_t &bt_err
					, const bt_swarm_resumedata_t &swarm_resumedata)	throw();
public:
	/**************	ctor/dtor	***************************************/
	bt_ezswarm_check_t() 		throw();
	~bt_ezswarm_check_t()		throw();
	
	/************** setup function	***************************************/
	bt_err_t	start(bt_ezswarm_t *bt_ezswarm)		throw();

	/*************** query function	***************************************/
	bt_check_t *	get_bt_check()	throw()	{ return bt_check;	}

	/*************** list of friend class	*******************************/
	friend class	bt_ezswarm_check_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_EZSWARM_CHECK_HPP__  */



