/*! \file
    \brief Header of the bt_ezswarm_alloc_t
    
*/


#ifndef __NEOIP_BT_EZSWARM_ALLOC_HPP__ 
#define __NEOIP_BT_EZSWARM_ALLOC_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_ezswarm_alloc_wikidbg.hpp"
#include "neoip_bt_alloc_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declarations
class	bt_ezswarm_t;
class	bt_err_t;

/** \brief class definition for bt_ezswarm_alloc_t
 */
class bt_ezswarm_alloc_t : NEOIP_COPY_CTOR_DENY, private bt_alloc_cb_t
			, private wikidbg_obj_t<bt_ezswarm_alloc_t, bt_ezswarm_alloc_wikidbg_init> {
private:
	bt_ezswarm_t *	bt_ezswarm;	//!< backpointer to bt_ezswarm_t
	/*************** bt_alloc_t	*******************************/
	bt_alloc_t *	bt_alloc;
	bool 		neoip_bt_alloc_cb(void *cb_userptr, bt_alloc_t &cb_bt_alloc
						, const bt_err_t &bt_err)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_ezswarm_alloc_t() 		throw();
	~bt_ezswarm_alloc_t()		throw();
	
	/*************** setup function	***************************************/
	bt_err_t	start(bt_ezswarm_t *bt_ezswarm)				throw();
	
	/*************** query function	***************************************/
	bt_alloc_t *	get_bt_alloc()	throw()	{ return bt_alloc;	}

	/*************** list of friend class	*******************************/
	friend class	bt_ezswarm_alloc_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_EZSWARM_ALLOC_HPP__  */



