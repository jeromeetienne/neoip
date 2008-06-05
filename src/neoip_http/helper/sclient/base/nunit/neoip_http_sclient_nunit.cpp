/*! \file
    \brief Definition of the unit test for the \ref http_sclient_t

*/

/* system include */
/* local include */
#include "neoip_http_sclient_nunit.hpp"
#include "neoip_http_sclient.hpp"
#include "neoip_http_sclient_res.hpp"
#include "neoip_http_client_pool.hpp"
#include "neoip_http_reqhd.hpp"
#include "neoip_file_range.hpp"
#include "neoip_http_listener.hpp"
#include "neoip_socket_resp_arg.hpp"
#include "neoip_http_sresp.hpp"
#include "neoip_http_sresp_ctx.hpp"
#include "neoip_http_resp_mode.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
http_sclient_testclass_t::http_sclient_testclass_t()	throw()
{
	// zero some field
	http_sclient	= NULL;
	http_sresp	= NULL;
	http_listener	= NULL;
	client_pool	= NULL;
}

/** \brief Destructor
 */
http_sclient_testclass_t::~http_sclient_testclass_t()	throw()
{
	// deinit the testclass just in case
	neoip_nunit_testclass_deinit();
}
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit init/deinit
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Init the testclass implementation
 */
nunit_err_t	http_sclient_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	http_sclient_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the http_client_pool_t if needed
	nipmem_zdelete	client_pool;
	// delete the http_sclient_t if needed
	nipmem_zdelete	http_sclient;
	// delete the http_sresp_t if needed
	nipmem_zdelete	http_sresp;
	// delete the http_listener_t if needed
	nipmem_zdelete	http_listener;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	http_sclient_testclass_t::one_cnx(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	http_err_t		http_err;
	http_sclient_profile_t	profile;
	// log to debug
	KLOG_ERR("enter");


	// start the http_listener_t
	socket_resp_arg_t	resp_arg;
	resp_arg	= socket_resp_arg_t().profile(socket_profile_t(socket_domain_t::TCP))
				.domain(socket_domain_t::TCP).type(socket_type_t::STREAM)
				.listen_addr("tcp://127.0.0.1:4000");
	http_listener	= nipmem_new http_listener_t();
	http_err	= http_listener->start(resp_arg);
	NUNIT_ASSERT( http_err.succeed() );
	
	// start the http_sresp_t 
	http_sresp	= nipmem_new http_sresp_t();
	http_err	= http_sresp->start(http_listener, "http://0.0.0.0/super_test"
					, http_method_t::GET, http_resp_mode_t::REJECT_SUBPATH
					, this, NULL);

	// start the http_client_pool_t
	client_pool	= nipmem_new http_client_pool_t();
	http_err	= client_pool->start();
	NUNIT_ASSERT( http_err.succeed() );
	// set the nb_request
	nb_request	= 1;

	// build the http_reqhd_t to use for the http_sclient_t
	http_reqhd_t	http_reqhd;
#if 0
	http_reqhd.uri("http://127.0.0.1:4000/super_test");
//	http_reqhd.uri("http://kde.fr");
//	http_reqhd.range(file_range_t(1,4));
#else
//	http_reqhd.uri("http://jmehost2/~jerome/flash/stream_php/stream.php?file=output.flv&pos=1927980");
//	http_reqhd.uri("http://jmehost2/~jerome/flash/stream_php/stream.php?file=output.flv&bla=slota&neoip_metavar_http_mod_type=flv&neoip_metavar_http_mod_flv_varname=pos");
//	http_reqhd.range(file_range_t(1, 16));
	http_reqhd.uri("https://jmehost2");
//	http_reqhd.range(file_range_t(0, file_size_t::MAX));
//	http_reqhd.range(file_range_t(0, 15));
//	http_reqhd.range(file_range_t(3, 200));
//	http_reqhd.range(file_range_t(3, file_size_t::MAX));
#endif
	// start the http_sclient_t
	http_sclient	= nipmem_new http_sclient_t();
	http_err	= http_sclient->set_client_pool(client_pool)
					.start(http_reqhd, this, NULL);
	NUNIT_ASSERT( http_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                         http_resp_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref http_resp_t when to notify an event
 * 
 * @return true if the http_resp_t is still valid after the callback
 */
bool	http_sclient_testclass_t::neoip_http_sresp_cb(void *cb_userptr
				, http_sresp_t &cb_http_sresp
				, http_sresp_ctx_t &sresp_ctx)		throw()
{
	// log to debug
	KLOG_ERR("enter sresp_ctx=" << sresp_ctx);
	
	// fill the response_body
	std::ostringstream &	oss	= sresp_ctx.response_body();
	oss << "01234";
	
	// add a funky mime-type 
	sresp_ctx.rephd().header_db().update("Content-Type", "application/x-javascript");
	
	// return tokeep
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     http_sclient_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref http_client_t to provide event
 */
bool	http_sclient_testclass_t::neoip_http_sclient_cb(void *cb_userptr, http_sclient_t &cb_sclient
				, const http_sclient_res_t &cb_sclient_res)	throw() 
{
	http_sclient_res_t	sclient_res	= cb_sclient_res;
	// log to debug
	KLOG_ERR("enter http_sclient_res=" << sclient_res);
	KLOG_ERR("reply_body=" << sclient_res.reply_body());

	// delete the http_sclient_res_t
	nipmem_zdelete	http_sclient;	

	// if client_pool is enable and if the reply is not bli.txt, relaunch it
	// - it helps testing the http_client_pool_t
	if( client_pool && nb_request > 1 ){	
		// update the nb_request
		nb_request--;
		// build the http_reqhd_t to use for the http_sclient_t
		http_reqhd_t	http_reqhd;
		http_reqhd.uri("http://127.0.0.1:4000/super_test");
		// start the http_sclient_t
		http_err_t	err;
		http_sclient	= nipmem_new http_sclient_t();
		err		= http_sclient->set_client_pool(client_pool).start(http_reqhd, this, NULL);
		DBG_ASSERT( err.succeed() );
		// return dontkeep
		return false;	
	}

	// report the result to nunit depending on the notified http_err_t
	if( sclient_res.full_get_ok() )	nunit_ftor( NUNIT_RES_OK	);
	else				nunit_ftor( NUNIT_RES_ERROR	);
	// return dontkeep
	return false;
}



NEOIP_NAMESPACE_END

