/*! \file
    \brief Header of the \ref bt_oload0_herr_t
    
*/


#ifndef __NEOIP_BT_OLOAD0_HERR_HPP__ 
#define __NEOIP_BT_OLOAD0_HERR_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_oload0_herr_wikidbg.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_httpo_full_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_oload0_t;
class	http_rephd_t;

/** \brief handle the bt_httpo_full_t while returning an error
 * 
 * - this object is mainly due to an issue in the socket_full_t with tcp
 *   family which doesnt handle linger properly
 *   - FIXME to remove once it is possible to get a real linger
 */
class bt_oload0_herr_t : NEOIP_COPY_CTOR_DENY, private bt_httpo_full_cb_t
				, private wikidbg_obj_t<bt_oload0_herr_t, bt_oload0_herr_wikidbg_init>
				{
private:
	bt_oload0_t *	bt_oload0;	//!< backpointer to the attached bt_oload0_t
	
	/*************** bt_httpo_full_t	*******************************/
	std::list<bt_httpo_full_t *>	httpo_full_db;	
	bool		neoip_bt_httpo_full_cb(void *cb_userptr, bt_httpo_full_t &cb_bt_httpo_full
						, const bt_httpo_event_t &httpo_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_oload0_herr_t()		throw();
	~bt_oload0_herr_t()		throw();

	/*************** Setup function	***************************************/
	bt_err_t	start(bt_oload0_t *bt_oload0)	throw();

	/*************** Action function	*******************************/
	void		add_httpo_full(bt_httpo_full_t *httpo_full, const http_rephd_t &http_rephd) throw();

	/*************** List of friend class	*******************************/
	friend class	bt_oload0_herr_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_OLOAD0_HERR_HPP__ */










