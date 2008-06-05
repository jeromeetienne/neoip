/*! \file
    \brief Header of the \ref bt_httpo_listener_t
    
*/


#ifndef __NEOIP_BT_HTTPO_LISTENER_HPP__ 
#define __NEOIP_BT_HTTPO_LISTENER_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_httpo_listener_wikidbg.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	http_listener_t;
class	bt_httpo_resp_t;
class	bt_httpo_full_t;

/** \brief Accept the http connections and then spawn bt_httpo_full_t to handle them
 */
class bt_httpo_listener_t : NEOIP_COPY_CTOR_DENY
		, private wikidbg_obj_t<bt_httpo_listener_t, bt_httpo_listener_wikidbg_init> {
private:
	/*************** http_listener_t	*******************************/
	http_listener_t *	http_listener;

	/*************** bt_httpo_resp_t	***************************************/
	std::list<bt_httpo_resp_t *>	httpo_resp_db;
	void resp_dolink(bt_httpo_resp_t *httpo_resp) 	throw()	{ httpo_resp_db.push_back(httpo_resp);	}
	void resp_unlink(bt_httpo_resp_t *httpo_resp)	throw()	{ httpo_resp_db.remove(httpo_resp);	}	

	/*************** bt_httpo_full_t	***************************************/
	std::list<bt_httpo_full_t *>	httpo_full_db;
	void full_dolink(bt_httpo_full_t *httpo_full) 	throw()	{ httpo_full_db.push_back(httpo_full);	}
	void full_unlink(bt_httpo_full_t *httpo_full)	throw()	{ httpo_full_db.remove(httpo_full);	}
public:
	/*************** ctor/dtor	***************************************/
	bt_httpo_listener_t()		throw();
	~bt_httpo_listener_t()		throw();
	
	/*************** Setup function	***************************************/
	bt_err_t	start(http_listener_t *http_listener)			throw();
	
	/*************** List of friend class	*******************************/
	friend class	bt_httpo_resp_t;
	friend class	bt_httpo_full_t;
	friend class	bt_httpo_listener_wikidbg_t;		
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_HTTPO_LISTENER_HPP__  */










