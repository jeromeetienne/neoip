/*! \file
    \brief Definition of the \ref rtmp2flv_apps_t class

\par Brief Description
TODO to comment

*/

/* system include */
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
/* local include */
#include "neoip_rtmp2flv_apps.hpp"
#include "neoip_clineopt_arr.hpp"
#include "neoip_flv.hpp"
#include "neoip_flv_err.hpp"
#include "neoip_fdwatch.hpp"
#include "neoip_dvar.hpp"
#include "neoip_dvar_helper.hpp"
#include "neoip_lib_session.hpp"
#include "neoip_lib_apps.hpp"
//#include "neoip_lib_apps_helper.hpp"

#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                                  CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief Default constructor
 */
rtmp2flv_apps_t::rtmp2flv_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// zero some fields
	rtmp_resp	= NULL;
}
	
/** \brief Destructor
 */
rtmp2flv_apps_t::~rtmp2flv_apps_t()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the rtmp_resp if needed
	nipmem_zdelete	rtmp_resp;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     setup function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief Start the operation
 */
flv_err_t	rtmp2flv_apps_t::start()	throw()
{
	flv_err_t	flv_err;
	// build the resp_arg
	socket_resp_arg_t	resp_arg;
	resp_arg	= socket_resp_arg_t().profile(socket_profile_t(socket_domain_t::TCP))
					.domain(socket_domain_t::TCP).type(socket_type_t::STREAM)
					.listen_addr("tcp://0.0.0.0:1935");
	// start the socket_resp_t
	rtmp_resp	= nipmem_new rtmp_resp_t();
	flv_err		= rtmp_resp->start(resp_arg);
	if( flv_err.failed() )	return flv_err;

	// return no error
	return flv_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			query function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the clineopt_arr_t for the cmdline option of the lib_session_t
 */
clineopt_arr_t	rtmp2flv_apps_t::clineopt_arr()	throw()
{
	clineopt_arr_t	clineopt_arr;
	clineopt_t	clineopt;
	// return the just built clineopt_arr_t
	return clineopt_arr;
}


NEOIP_NAMESPACE_END

