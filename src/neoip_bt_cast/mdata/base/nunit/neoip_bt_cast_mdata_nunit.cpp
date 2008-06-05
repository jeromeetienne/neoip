/*! \file
    \brief Definition of the unit test for the \ref bt_cast_mdata_server_t

*/

/* system include */
/* local include */
#include "neoip_bt_cast_mdata_nunit.hpp"
#include "neoip_bt_cast_mdata_server.hpp"
#include "neoip_bt_cast_mdata_client.hpp"
#include "neoip_bt_cast_mdata.hpp"
#include "neoip_http_listener.hpp"
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
bt_cast_mdata_testclass_t::bt_cast_mdata_testclass_t()	throw()
{
	// zero some field
	m_http_listener	= NULL;
	mdata_server	= NULL;
	mdata_client	= NULL;
}

/** \brief Destructor
 */
bt_cast_mdata_testclass_t::~bt_cast_mdata_testclass_t()	throw()
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
nunit_err_t	bt_cast_mdata_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// sanity check
	DBG_ASSERT( !mdata_server );
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	bt_cast_mdata_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// destruct the bt_cast_mdata_client_t
	nipmem_zdelete	mdata_client;
	// destruct the bt_cast_mdata_server_t
	nipmem_zdelete	mdata_server;
	// delete http_listener_t if needed
	nipmem_zdelete	m_http_listener;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bt_cast_mdata_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	bt_err_t	bt_err;
	// log to debug
	KLOG_DBG("enter");

	// start the http_listener_t
	socket_resp_arg_t	resp_arg;
	resp_arg	= socket_resp_arg_t().profile(socket_profile_t(socket_domain_t::TCP))
					.domain(socket_domain_t::TCP).type(socket_type_t::STREAM)
					.listen_addr("tcp://0.0.0.0:4000");
	http_err_t	http_err;
	m_http_listener	= nipmem_new http_listener_t();
	http_err	= m_http_listener->start(resp_arg);
	NUNIT_ASSERT( http_err.succeed() );
	
	// start the bt_cast_mdata_server_t
	mdata_server	= nipmem_new bt_cast_mdata_server_t();
	bt_err		= mdata_server->start(m_http_listener, this, NULL);
	NUNIT_ASSERT( bt_err.succeed() );

	// start the bt_cast_mdata_client_t
	http_uri_t	server_uri	= "http://localhost:4000/neoip_bt_cast_mdata_server_xmlrpc.cgi";
	mdata_client	= nipmem_new bt_cast_mdata_client_t();
	bt_err		= mdata_client->start(server_uri, bt_cast_id_t("dummy cast id"), this, NULL);
	NUNIT_ASSERT( bt_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     bt_cast_mdata_server_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_cast_mdata_server_t to provide event
 */
bool	bt_cast_mdata_testclass_t::neoip_bt_cast_mdata_server_cb(void *cb_userptr
					, bt_cast_mdata_server_t &cb_mdata_server
					, const bt_cast_id_t &cast_id
					, bt_cast_mdata_t *cast_mdata_out)	throw()
{
	bt_cast_mdata_t	cast_mdata;
	// log to debug
	KLOG_ERR("enter cast_id=" << cast_id);

	// stub to put stuff in the bt_cast_mdata_t
	cast_mdata.cast_id			( cast_id 		);
	cast_mdata.cast_name			( "dummy cast_name"	);
	cast_mdata.boot_nonce			( 4242			);
	cast_mdata.slide_curs_nbpiece_hint	( 20 			);
	cast_mdata.pieceq_beg			( 0			);
	cast_mdata.pieceq_end			( 20			);
	cast_mdata.pieceq_maxlen		( 128			);
	cast_mdata.casti_date			( date_t::present()	);
	cast_mdata.recv_udata_maxdelay		( delay_t::from_sec(42)	);
	cast_mdata.cast_spos_arr().append(bt_cast_spos_t(4242, date_t::present()));

	// copy the just build cast_mdata
	*cast_mdata_out	= cast_mdata;
	
	// return tokeep
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     bt_cast_mdata_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_cast_mdata_client_t to provide event
 */
bool	bt_cast_mdata_testclass_t::neoip_bt_cast_mdata_client_cb(void *cb_userptr
					, bt_cast_mdata_client_t &cb_mdata_client
					, const bt_err_t &bt_err
					, const bt_cast_mdata_t &cast_mdata)	throw()
{
	// log to debug
	KLOG_ERR("enter bt_err=" << bt_err << " cast_mdata=" << cast_mdata);
	
	// test if the result if the supposed one
	if( bt_err.succeed() && cast_mdata.cast_id() == "dummy cast id"){
		neoip_nunit_testclass_deinit();
		nunit_ftor( NUNIT_RES_OK		);
		return false;
	}else{
		neoip_nunit_testclass_deinit();
		nunit_ftor( NUNIT_RES_ERROR	);
		return false;
	}
}



NEOIP_NAMESPACE_END

