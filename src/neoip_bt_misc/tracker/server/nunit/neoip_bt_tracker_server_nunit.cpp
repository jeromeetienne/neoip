/*! \file
    \brief Definition of the unit test for the \ref bt_tracker_server_t

*/

/* system include */
/* local include */
#include "neoip_bt_tracker_server_nunit.hpp"
#include "neoip_bt.hpp"
#include "neoip_httpd.hpp"
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
bt_tracker_server_testclass_t::bt_tracker_server_testclass_t()	throw()
{
	// zero some field
	httpd		= NULL;
	bt_tracker_server	= NULL;
	bt_tracker_client	= NULL;
}

/** \brief Destructor
 */
bt_tracker_server_testclass_t::~bt_tracker_server_testclass_t()	throw()
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
nunit_err_t	bt_tracker_server_testclass_t::neoip_nunit_testclass_init()	throw()
{
	bool	failed;
	// create the httpd_t
	httpd	= nipmem_new httpd_t();
	failed	= httpd->start("127.0.0.1:4000");
	if( failed )	return nunit_err_t::ERROR;
	
	// create and start the bt_tracker_server_t
	bt_err_t	bt_err;
	bt_tracker_server	= nipmem_new bt_tracker_server_t();
	bt_err	= bt_tracker_server->start(httpd, "/announce", this, NULL);
	if( bt_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, bt_err.to_string());

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	bt_tracker_server_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the bt_tracker_client_t if needed
	nipmem_zdelete	bt_tracker_client;
	// delete the bt_tracker_server_t if needed
	nipmem_zdelete	bt_tracker_server;
	// delete the httpd_t if needed
	nipmem_zdelete	httpd;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bt_tracker_server_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	bt_err_t	bt_err;

	// build the bt_tracker_request_t for the bt_tracker_client_t
	bt_tracker_request_t	bt_tracker_request;
	bt_tracker_request.announce_uri	( "http://localhost:4000/announce" );
	bt_tracker_request.infohash	( "0xe296b5687f63150fcadb8f20f5925b94ba1becbf" );
	bt_tracker_request.peerid	( bt_id_t::build_random() );
	bt_tracker_request.port		( 50003 );
	bt_tracker_request.compact	( true );
	bt_tracker_request.nopeerid	( false );

	// start the bt_tracker_client_t
	bt_tracker_client	= nipmem_new bt_tracker_client_t();
	bt_err	= bt_tracker_client->start(bt_tracker_request, this, NULL);
	NUNIT_ASSERT( bt_err.succeed() );	

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    bt_tracker_server_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_tracker_server_t when to notify an request
 */
bool	bt_tracker_server_testclass_t::neoip_bt_tracker_server_cb(void *cb_userptr
				, bt_tracker_server_t &cb_bt_tracker_server, slot_id_t cnx_slotid
				, const bt_tracker_request_t &request, bt_tracker_reply_t &reply_out)	throw()
{
	// log to debug
	KLOG_DBG("enter cnx_slotid=" << cnx_slotid << " request=" << request);	
	
	// handle the reply in sync
#if 1
	// build a successfull reply
	reply_out.request_period(delay_t::from_sec(42));
	reply_out.nb_seeder	(42);
	reply_out.nb_leecher	(43);
	reply_out.peer_arr()	+= bt_tracker_peer_t("42.43.44.45:1234", "fake dummy peerid" );
#else
	// build a failed reply
	reply_out.failure_reason	("dummy faillure reason");
#endif

	// return tokeep
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     bt_tracker_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_tracker_client_t to provide event
 */
bool	bt_tracker_server_testclass_t::neoip_bt_tracker_client_cb(void *cb_userptr, bt_tracker_client_t &cb_bt_tracker_client
						, const bt_err_t &bt_err
						, const bt_tracker_reply_t &reply)	throw() 
{
	// log to debug
	KLOG_DBG("enter bt_err=" << bt_err << " reply=" << reply);

	// delete the bt_tracker_client_t
	nipmem_zdelete bt_tracker_client;

	// test if the nunit succeed
	if( bt_err.succeed()
			&& reply.request_period() == delay_t::from_sec(42) 
			&& reply.nb_seeder() == 42 && reply.nb_leecher() == 43
			&& reply.peer_arr().size() == 1
			&& reply.peer_arr()[0].get_ipport() == "42.43.44.45:1234" ){
		nunit_ftor( NUNIT_RES_OK );
		return false;
	}

	// report a faillure
	nunit_ftor( NUNIT_RES_ERROR );
	return false;
}

NEOIP_NAMESPACE_END

