/*! \file
    \brief Header of the bt_io_cache_stopping_t
    
*/


#ifndef __NEOIP_BT_IO_CACHE_STOPPING_HPP__ 
#define __NEOIP_BT_IO_CACHE_STOPPING_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_io_cache_stopping_wikidbg.hpp"
#include "neoip_bt_io_stopping_cb.hpp"
#include "neoip_bt_io_stopping.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_io_cache_t;

/** \brief class definition for bt_io_cache_stopping
 */
class bt_io_cache_stopping_t : NEOIP_COPY_CTOR_DENY, public bt_io_stopping_t
			, private wikidbg_obj_t<bt_io_cache_stopping_t, bt_io_cache_stopping_wikidbg_init> {
private:
	bt_io_cache_t *	m_io_cache;	//!< backpointer on the attached bt_io_cache_t
			
	/*************** callback stuff	***************************************/
	bt_io_stopping_cb_t*callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const bt_err_t &bt_err)			throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_io_cache_stopping_t() 		throw();
	~bt_io_cache_stopping_t()		throw();

	/*************** Setup function	***************************************/
	bt_err_t	start(bt_io_cache_t *io_cache, bt_io_stopping_cb_t *callback
							, void *userptr) 	throw();

	/*************** query function	***************************************/
	bt_io_cache_t *	io_cache()	const throw()	{ return m_io_cache;	}

	/*************** notify function	*******************************/
	void		notify_blkwr_completion()	throw();

	/*************** List of friend class	*******************************/
	friend class	bt_io_cache_stopping_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_IO_CACHE_STOPPING_HPP__  */



