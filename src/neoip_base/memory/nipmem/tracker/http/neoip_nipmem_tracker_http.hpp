/*! \file
    \brief Header of the \ref nipmem_tracker_http_t class
    
*/


#ifndef __NEOIP_NIPMEM_TRACKER_HTTP_HPP__ 
#define __NEOIP_NIPMEM_TRACKER_HTTP_HPP__ 
/* system include */
/* local include */
#include "neoip_nipmem_tracker_http_wikidbg.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	lib_httpd_t;

/** \brief display the content of nipmem_tracker_t via http
 */
class nipmem_tracker_http_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<nipmem_tracker_http_t, nipmem_tracker_http_wikidbg_init>
			{
private:
public:
	/*************** ctor/dtor	***************************************/
	nipmem_tracker_http_t()		throw();
	~nipmem_tracker_http_t()	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NIPMEM_TRACKER_HTTP_HPP__  */



