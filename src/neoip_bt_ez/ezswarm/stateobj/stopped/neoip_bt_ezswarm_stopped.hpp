/*! \file
    \brief Header of the bt_ezswarm_stopped_t
    
*/


#ifndef __NEOIP_BT_EZSWARM_STOPPED_HPP__ 
#define __NEOIP_BT_EZSWARM_STOPPED_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_ezswarm_stopped_wikidbg.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declarations
class	bt_ezswarm_t;
class	bt_err_t;

/** \brief class definition for bt_ezswarm_stopped_t
 */
class bt_ezswarm_stopped_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<bt_ezswarm_stopped_t, bt_ezswarm_stopped_wikidbg_init> {
private:
	bt_ezswarm_t *	bt_ezswarm;	//!< backpointer to bt_ezswarm_t
public:
	/**************	ctor/dtor	***************************************/
	bt_ezswarm_stopped_t() 		throw();
	~bt_ezswarm_stopped_t()		throw();
	
	/************** setup function	***************************************/
	bt_err_t	start(bt_ezswarm_t *bt_ezswarm)	throw();

	/*************** list of friend class	*******************************/
	friend class	bt_ezswarm_stopped_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_EZSWARM_STOPPED_HPP__  */



