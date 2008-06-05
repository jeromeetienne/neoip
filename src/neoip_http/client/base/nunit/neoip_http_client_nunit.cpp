/*! \file
    \brief Definition of the unit test for the \ref http_client_t

*/

/* system include */
/* local include */
#include "neoip_http_client_nunit.hpp"
#include "neoip_http_client.hpp"
#include "neoip_http_rephd.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_socket_itor_arg.hpp"
#include "neoip_socket_full.hpp"
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
http_client_testclass_t::http_client_testclass_t()	throw()
{
	// zero some field
	http_client	= NULL;
}

/** \brief Destructor
 */
http_client_testclass_t::~http_client_testclass_t()	throw()
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
nunit_err_t	http_client_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// sanity check
	DBG_ASSERT( !http_client );
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	http_client_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// destruct the http_client_t
	nipmem_zdelete http_client;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	http_client_testclass_t::one_cnx(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	http_err_t	http_err;
	// log to debug
	KLOG_DBG("enter");

	// init some variable
	http_reqhd_t	http_reqhd;
	http_reqhd.uri("http://127.0.0.1/~jerome/bla.txt");
//	http_reqhd.uri("http://google.com");

	// start the http_client_t
	http_client	= nipmem_new http_client_t();
	http_err	= http_client->start(http_reqhd, this, NULL);
	NUNIT_ASSERT( http_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     http_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref http_client_t to provide event
 */
bool	http_client_testclass_t::neoip_http_client_cb(void *cb_userptr, http_client_t &cb_http_client
			, const http_err_t &http_err, const http_rephd_t &http_rephd
			, socket_full_t *socket_full, const bytearray_t &recved_data)	throw() 
{
	// log to debug
	KLOG_DBG("enter http_err=" << http_err << " http_rephd=" << http_rephd 
					<< "recved_data=" << recved_data);

	// delete the http_client_t
	nipmem_zdelete	http_client;
	// delete the nofified socket_full if any
	nipmem_zdelete	socket_full;

	// report the result to nunit depending on the notified http_err_t
	if( http_err.succeed() )	nunit_ftor( NUNIT_RES_OK	);
	else				nunit_ftor( NUNIT_RES_ERROR	);

	// return dontkeep
	return false;
}



NEOIP_NAMESPACE_END

