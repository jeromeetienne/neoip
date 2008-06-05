/*! \file
    \brief Definition of the unit test for the \ref bt_swarm_t

*/

/* system include */
/* local include */
#include "neoip_bt_swarm_nunit.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_helper.hpp"
#include "neoip_bt_swarm_event.hpp"
#include "neoip_bt_swarm_peersrc.hpp"
#include "neoip_bt_swarm_utmsg.hpp"
#include "neoip_bt_session.hpp"
#include "neoip_bt_http_ecnx_pool.hpp"
#include "neoip_bt_io_sfile.hpp"
#include "neoip_bt_peersrc_http.hpp"
#include "neoip_bt_peersrc_kad.hpp"
#include "neoip_bt_peersrc_utpex.hpp"
#include "neoip_bt_mfile_helper.hpp"
#include "neoip_bt_swarm_resumedata_helper.hpp"
#include "neoip_file_sio.hpp"
#include "neoip_file_stat.hpp"
#include "neoip_file_err.hpp"
#include "neoip_kad.hpp"
#include "neoip_kad_publish.hpp"
#include "neoip_udp_vresp.hpp"
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
bt_swarm_testclass_t::bt_swarm_testclass_t()	throw()
{
	// zero some field
	bt_session	= NULL;
	bt_swarm	= NULL;
	bt_io_vapi	= NULL;
	http_ecnx_pool	= NULL;
	// zero stuff for peersrc_http
	peersrc_http	= NULL;	
	// zero stuff for peersrc_kad
	udp_vresp	= NULL;
	kad_listener	= NULL;
	kad_peer	= NULL;
	peersrc_kad	= NULL;
	// zero stuff for peersrc_utpex
	peersrc_utpex	= NULL;	
}

/** \brief Destructor
 */
bt_swarm_testclass_t::~bt_swarm_testclass_t()	throw()
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
nunit_err_t	bt_swarm_testclass_t::neoip_nunit_testclass_init()	throw()
{		
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	bt_swarm_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the bt_http_ecnx_pool_t if needed
	nipmem_zdelete	http_ecnx_pool;
	// delete the bt_peersrc_http_t if needed
	nipmem_zdelete	peersrc_http;
// deinit the peersrc_kad
	// delete the peersrc_kad if needed
	nipmem_zdelete	peersrc_kad;
	// delete the kad_peer_t if needed
	nipmem_zdelete	kad_peer;
	// delete the kad_listener_t if needed
	nipmem_zdelete	kad_listener;	
	// delete the udp_vresp_t if needed
	nipmem_zdelete	udp_vresp;
	// delete the bt_peersrc_utpex_t if needed
	nipmem_zdelete	peersrc_utpex;	
	// try to save the bt_swarm_resumedata_t
	if( 0 && bt_swarm ){
		file_path_t	file_path	= mfile_path.to_string() + ".neoip_bt.state";
		bt_err_t	bt_err;
		// try to write the bt_swarm_resumedata_t to a file
		bt_err	= bt_swarm_resumedata_helper_t::to_file(bt_swarm->get_current_resumedata(), file_path);
		// if there is an error, log the event
		KLOG_WARN("Cant write the bt_swarm_resumedata_t due to " << bt_err);
	}
	// delete the bt_swarm_t if needed
	nipmem_zdelete	bt_swarm;
	// delete the bt_io_vapi_t if needed
	nipmem_zdelete	bt_io_vapi;
	// delete the bt_session_t if needed
	nipmem_zdelete	bt_session;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bt_swarm_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	bt_err_t		bt_err;	
	bt_swarm_resumedata_t	swarm_resumedata;
	ipport_aview_t		listen_aview;
	// log to debug
	KLOG_DBG("enter");

#if 1
	listen_aview.lview	(":50001");
	listen_aview.pview	(":50001");
#else
	listen_aview.lview	("192.168.1.2:5001");
	listen_aview.pview	(":5001");
#endif

	// set the file_path_t to the .torrent file
//	mfile_path	= "/home/jerome/downloaded/ubuntu-6.06.1-desktop-i386.iso.torrent";
//	mfile_path	= "/home/jerome/downloaded/ubuntu-6.10-desktop-i386.iso.torrent";
//	mfile_path	= "/home/jerome/downloaded/IBM_Linux_Commercial.avi.torrent";
	mfile_path	= "/home/jerome/downloaded/local_ubuntu_experiment.torrent";
	

	// start the bt_session_t
	bt_session	= nipmem_new bt_session_t();
	bt_err		= bt_session->start(listen_aview);
	NUNIT_ASSERT( bt_err.succeed() );

// FIXME do a real nunit - currently only a testing function
	bt_mfile_t	bt_mfile;
	bt_mfile	= bt_mfile_helper_t::from_torrent_file(mfile_path);
	NUNIT_ASSERT( !bt_mfile.is_null() );
	// add the local_path in the bt_mfile.subfile_arr()
	bt_mfile.set_file_local_dir("/home/jerome/downloaded");
#if 0	// if the bt_mfile_t is on ubuntu-6.10, change the announce_uri to avoid any dns resolution
	if( bt_mfile.subfile_arr()[0].mfile_path() == "ubuntu-6.10-desktop-i386.iso" ){
		bt_mfile.announce_uri("http://82.211.81.143:6969/announce");
	}
#endif
#if 1	// if the bt_mfile_t is on ubuntu-6.06-1, add a http_uri_t toward jmehost2
	if( bt_mfile.subfile_arr()[0].mfile_path() == "ubuntu-6.06.1-desktop-i386.iso" ){
		bt_mfile.subfile_arr()[0].uri_arr()	+= "http://192.168.1.3/~jerome/ubuntu-6.06.1-desktop-i386.iso";
	}
#endif
#if 0	// if the bt_mfile_t is on IBM_Linux_Commercial, add a http_uri_t toward jmehost2
	if( bt_mfile.subfile_arr()[0].mfile_path() == "IBM_Linux_Commercial.avi" ){
		bt_mfile.subfile_arr()[0].uri_arr()	+= "http://192.168.1.3/~jerome/IBM_Linux_Commercial.avi";
	}
#endif
	// disable the piece hash checking
	bt_mfile.do_piecehash(false);

	// complete the bt_mfile_t initialization
	bt_mfile.complete_init();
	
	// init the bt_io_vapi_t
	bt_io_sfile_t *	bt_io_sfile;
	bt_io_sfile	= nipmem_new bt_io_sfile_t();
	bt_err		= bt_io_sfile->start(bt_mfile);
	NUNIT_ASSERT( bt_err.succeed() );	
	// assign the bt_io_vapi
	bt_io_vapi	= bt_io_sfile;


	// try to read the bt_swarm_resumedata_t
	swarm_resumedata = bt_swarm_resumedata_helper_t::from_file(mfile_path.to_string() + ".neoip_bt.state");

	// start a bt_swarm_t depending on the availability of a bt_swarm_resumedata_t
	bt_swarm	= nipmem_new bt_swarm_t();
	if( swarm_resumedata.is_null() ){
		bt_err		= bt_swarm->start(bt_session, bt_mfile, bt_io_vapi, this, NULL);
	}else{
		bt_err		= bt_swarm->start(bt_session, swarm_resumedata, bt_io_vapi, this, NULL);
	}
	NUNIT_ASSERT( bt_err.succeed() );

#if 0
	// set a bt_pieceprec_arr_t to download the whole torrent sequentially
	bt_pieceprec_arr_t	pieceprec_arr;
	pieceprec_arr.assign(bt_swarm->get_mfile().nb_piece(), bt_pieceprec_t::NOTNEEDED);
	pieceprec_arr.set_range_decrease(0, bt_swarm->get_mfile().nb_piece()-1, bt_pieceprec_t::HIGHEST);
	bt_swarm_helper_t::set_pselect_fixed_pieceprec_arr(bt_swarm, pieceprec_arr);
#endif
	// init the bt_http_ecnx_pool_t
	http_ecnx_pool	= nipmem_new bt_http_ecnx_pool_t();
	bt_err		= http_ecnx_pool->start(bt_swarm);
	if( bt_err.failed() )		return nunit_err_t(nunit_err_t::ERROR, bt_err.to_string());

#if 1
	// init the bt_peersrc_http_t
	peersrc_http	= nipmem_new bt_peersrc_http_t();
	bt_err		= peersrc_http->start(bt_swarm, bt_swarm->swarm_peersrc(), NULL);
	if( bt_err.failed() )		return nunit_err_t(nunit_err_t::ERROR, bt_err.to_string());

// init stuff for bt_peersrc_kad_t
	// create the udp_vresp
	inet_err_t	inet_err;
	udp_vresp	= nipmem_new udp_vresp_t();
	inet_err 	= udp_vresp->start(ipport_addr_t::ANY_IP4);
	// if the udp_vresp_t failed, report the error
	if( inet_err.failed() )		return nunit_err_t(nunit_err_t::ERROR, inet_err.to_string());
	// update the listen_aview.lview() with the local ipport_addr_t from udp_vresp_t
	listen_aview.update_once_bound	(udp_vresp->get_listen_addr());
	// init kad_listener
	kad_err_t	kad_err;
	kad_listener	= nipmem_new kad_listener_t();
	kad_err 	= kad_listener->start("255.255.255.255:7777", udp_vresp, listen_aview);
	if( kad_err.failed() )		return nunit_err_t(nunit_err_t::ERROR, kad_err.to_string());
	// set some variables for kad_peer_t
	kad_realmid_t	kad_realmid	= "nunit bt_swarm";
	kad_peerid_t	kad_peerid	= kad_peerid_t::build_random();
	kad_peer	= nipmem_new kad_peer_t();
	kad_err		= kad_peer->start(kad_listener, kad_realmid, kad_peerid);
	if( kad_err.failed() )		return nunit_err_t(nunit_err_t::ERROR, kad_err.to_string());
	// init the bt_peersrc_kad_t
	peersrc_kad	= nipmem_new bt_peersrc_kad_t();
	bt_err		= peersrc_kad->start(bt_swarm, kad_peer, bt_swarm->swarm_peersrc(), NULL);
	if( bt_err.failed() )		return nunit_err_t(nunit_err_t::ERROR, bt_err.to_string());

	// init the bt_peersrc_utpex_t
	peersrc_utpex	= nipmem_new bt_peersrc_utpex_t();
	bt_err		= peersrc_utpex->start(bt_swarm, bt_swarm->swarm_peersrc()
							, bt_swarm->swarm_utmsg(), NULL);
	if( bt_err.failed() )		return nunit_err_t(nunit_err_t::ERROR, bt_err.to_string());
#endif
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   bt_swarm_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_swarm_t when to notify an event
 */
bool 	bt_swarm_testclass_t::neoip_bt_swarm_cb(void *cb_userptr, bt_swarm_t &cb_bt_swarm
					, const bt_swarm_event_t &swarm_event)	throw()
{
	// log to debug
	KLOG_ERR("enter event=" << swarm_event);
	
	// handle the fatal bt_swarm_event_t
	if( swarm_event.is_fatal() ){
		neoip_nunit_testclass_deinit();
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}
	
	// handle the bt_swarm_event_t according to its value
	switch(swarm_event.get_value()){
	case bt_swarm_event_t::PIECE_NEWLY_AVAIL:	
#if 1	// TODO to reenable - just to create a never ending nunit
			// if the bt_swarm_t is now a seed, declare the nunit as succeed
			if( bt_swarm->is_seed() ){
				neoip_nunit_testclass_deinit();
				nunit_ftor(NUNIT_RES_OK);
				return false;
			}
#endif
			break;
	default:	DBG_ASSERT( 0 );
	}
	
	// return tokeep
	return true;
}


NEOIP_NAMESPACE_END

