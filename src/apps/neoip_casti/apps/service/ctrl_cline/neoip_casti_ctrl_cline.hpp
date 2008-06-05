/*! \file
    \brief Header of the \ref casti_ctrl_cline_t
    
*/


#ifndef __NEOIP_CASTI_CTRL_CLINE_HPP__ 
#define __NEOIP_CASTI_CTRL_CLINE_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_casti_ctrl_cline_wikidbg.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	casti_apps_t;
class	casti_swarm_arg_t;
class	clineopt_arr_t;

/** \brief Handle the control via webpage
 */
class casti_ctrl_cline_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t
		, private wikidbg_obj_t<casti_ctrl_cline_t, casti_ctrl_cline_wikidbg_init> {
private:
	casti_apps_t *		m_casti_apps;
	
	/*************** Internal function	*******************************/
	casti_swarm_arg_t	build_swarm_arg()	throw();
	
	/*************** timeout_t	***************************************/
	timeout_t	refresh_timeout;
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	/*************** ctor/dtor	***************************************/
	casti_ctrl_cline_t()		throw();
	~casti_ctrl_cline_t()		throw();
	
	/*************** Setup function	***************************************/
	bt_err_t	start(casti_apps_t *m_casti_apps)		throw();

	/*************** Query function	***************************************/
	static clineopt_arr_t	clineopt_arr()		throw();

	/*************** List of friend class	*******************************/
	friend class	casti_ctrl_cline_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CASTI_CTRL_CLINE_HPP__  */










