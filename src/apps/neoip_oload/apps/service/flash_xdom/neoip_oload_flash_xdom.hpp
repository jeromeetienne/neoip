/*! \file
    \brief Header of the \ref oload_flash_xdom_t
    
*/


#ifndef __NEOIP_OLOAD_FLASH_XDOM_HPP__ 
#define __NEOIP_OLOAD_FLASH_XDOM_HPP__ 
/* system include */
/* local include */
#include "neoip_oload_flash_xdom_wikidbg.hpp"
#include "neoip_http_sresp_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	http_listener_t;
class	http_uri_t;
class	bt_err_t;

/** \brief Handle the webdetect part for the bt_oload_stat_t
 */
class oload_flash_xdom_t : NEOIP_COPY_CTOR_DENY, private http_sresp_cb_t
		, private wikidbg_obj_t<oload_flash_xdom_t, oload_flash_xdom_wikidbg_init> {
private:
	/*************** http_sresp_t	***************************************/
	http_sresp_t *	m_http_sresp;
	bool		neoip_http_sresp_cb(void *cb_userptr, http_sresp_t &cb_http_sresp
						, http_sresp_ctx_t &sresp_ctx)	throw();
public:
	/*************** ctor/dtor	***************************************/
	oload_flash_xdom_t()		throw();
	~oload_flash_xdom_t()		throw();

	/*************** Setup function	***************************************/
	bt_err_t	start(http_listener_t *http_listener)		throw();
	
	/*************** List of friend class	*******************************/
	friend class	oload_flash_xdom_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_OLOAD_FLASH_XDOM_HPP__ */










