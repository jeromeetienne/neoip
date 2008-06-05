/*! \file
    \brief Header of the \ref rtmp2flv_apps_t class

*/


#ifndef __NEOIP_RTMP2FLV_APPS_HPP__ 
#define __NEOIP_RTMP2FLV_APPS_HPP__ 
/* system include */
/* local include */
#include "neoip_rtmp_resp.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	flv_err_t;
class	clineopt_arr_t;

/** \brief The object to run the neoip-rtmp2flv apps
 */
class rtmp2flv_apps_t : NEOIP_COPY_CTOR_DENY {
private:
	rtmp_resp_t *	rtmp_resp;
public:
	/*************** ctor/dtor	***************************************/
	rtmp2flv_apps_t()		throw();
	~rtmp2flv_apps_t()		throw();
	
	/*************** setup function	***************************************/
	flv_err_t	start()	throw();

	/*************** query function	***************************************/
	static clineopt_arr_t		clineopt_arr()		throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP2FLV_APPS_HPP__  */


 
