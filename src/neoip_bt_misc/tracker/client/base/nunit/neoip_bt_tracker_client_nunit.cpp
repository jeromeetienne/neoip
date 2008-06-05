/*! \file
    \brief Definition of the unit test for the \ref bt_tracker_client_t

*/

/* system include */
/* local include */
#include "neoip_bt_tracker_client_nunit.hpp"
#include "neoip_bt_tracker_client.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

#include "neoip_bencode.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_bt_tracker_request.hpp"
#include "neoip_bt_tracker_reply.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_file_sio.hpp"
#include "neoip_file_stat.hpp"
#include "neoip_file_err.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
bt_tracker_client_testclass_t::bt_tracker_client_testclass_t()	throw()
{
	// zero some field
	bt_tracker_client	= NULL;
}

/** \brief Destructor
 */
bt_tracker_client_testclass_t::~bt_tracker_client_testclass_t()	throw()
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
nunit_err_t	bt_tracker_client_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// sanity check
	DBG_ASSERT( !bt_tracker_client );
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	bt_tracker_client_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// destruct the bt_tracker_client_t
	nipmem_zdelete bt_tracker_client;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bt_tracker_client_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	bt_err_t	bt_err;
	// log to debug
	KLOG_DBG("enter");

	// load the bencoded mfile
	file_sio_t	file_sio;
	file_err_t	file_err;	
	datum_t		bencoded_mfile;
	file_path_t	file_path	= "/home/jerome/downloaded/ubuntu-6.06.1-desktop-i386.iso.torrent";
	file_err	= file_sio.start(file_path, file_mode_t::READ);
	NUNIT_ASSERT( file_err.succeed() );
	file_err	= file_sio.read(file_stat_t(file_path).get_size(), bencoded_mfile);
	NUNIT_ASSERT( file_err.succeed() );
	
	// build the bt_mfile_t from the bencoded_mfile
	bt_mfile_t	bt_mfile	= bt_mfile_t::from_bencode( bencoded_mfile );
	NUNIT_ASSERT( !bt_mfile.is_null() );
	// add the local_path in the bt_mfile.subfile_arr()
	bt_mfile.set_file_local_dir("/home/jerome/downloaded");
	// complete the bt_mfile_t initialization
	bt_mfile.complete_init();	

	// build the tracker_request
	bt_tracker_request_t	tracker_request;
	tracker_request.announce_uri	( bt_mfile.announce_uri() );
	tracker_request.infohash	( bt_mfile.infohash() );
	tracker_request.peerid		( bt_id_t::build_random() );
	tracker_request.port		( 5001 );

	// start the bt_tracker_client_t
	bt_tracker_client	= nipmem_new bt_tracker_client_t();
	bt_err			= bt_tracker_client->start(tracker_request, this, NULL);
	NUNIT_ASSERT( bt_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     bt_tracker_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_tracker_client_t to provide event
 */
bool	bt_tracker_client_testclass_t::neoip_bt_tracker_client_cb(void *cb_userptr, bt_tracker_client_t &cb_bt_tracker_client
				, const bt_err_t &bt_err, const bt_tracker_reply_t &reply)	throw() 
{
	// log to debug
	KLOG_WARN("enter bt_err=" << bt_err << " reply=" << reply);

	// delete the bt_tracker_client_t
	nipmem_zdelete bt_tracker_client;

	// report the result to nunit depending on the notified bt_err_t
	if( bt_err.succeed() )	nunit_ftor( NUNIT_RES_OK	);
	else			nunit_ftor( NUNIT_RES_ERROR	);

	// return dontkeep
	return false;
}



NEOIP_NAMESPACE_END

