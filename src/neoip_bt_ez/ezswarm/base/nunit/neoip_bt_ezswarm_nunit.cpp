/*! \file
    \brief Definition of the unit test for the \ref bt_ezswarm_t

*/

/* system include */
/* local include */
#include "neoip_bt_ezswarm_nunit.hpp"
#include "neoip_bt_ezswarm.hpp"
#include "neoip_bt_ezswarm_event.hpp"
#include "neoip_bt_ezswarm_share.hpp"
#include "neoip_bt_ezsession.hpp"
#include "neoip_bt_mfile_helper.hpp"
#include "neoip_bt_swarm.hpp"
#include "neoip_bt_swarm_event.hpp"
#include "neoip_bt_swarm_resumedata_helper.hpp"
#include "neoip_ipport_aview.hpp"
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
bt_ezswarm_testclass_t::bt_ezswarm_testclass_t()	throw()
{
	// zero some field
	bt_ezsession	= NULL;
	bt_ezswarm	= NULL;
}

/** \brief Destructor
 */
bt_ezswarm_testclass_t::~bt_ezswarm_testclass_t()	throw()
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
nunit_err_t	bt_ezswarm_testclass_t::neoip_nunit_testclass_init()	throw()
{		
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	bt_ezswarm_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the bt_ezswarm_t if needed
	nipmem_zdelete	bt_ezswarm;
	// delete the bt_ezsession_t if needed
	nipmem_zdelete	bt_ezsession;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bt_ezswarm_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	file_path_t		mfile_path;
	bt_swarm_resumedata_t	swarm_resumedata;
	bt_err_t		bt_err;
	bt_ezswarm_profile_t	ezswarm_profile;


	// set the file_path_t to the .torrent file
//	mfile_path	= "/home/jerome/downloaded/ubuntu-6.06.1-desktop-i386.iso.torrent";
	mfile_path	= "/home/jerome/downloaded/ubuntu-6.10-desktop-i386.iso.torrent";
//	mfile_path	= "/home/jerome/downloaded/IBM_Linux_Commercial.avi.torrent";
//	mfile_path	= "/home/jerome/downloaded/local_ubuntu_experiment.torrent";

	// launch the bt_ezsession_t
	bt_err		= launch_ezsession();
	NUNIT_ASSERT( bt_err.succeed() );	

	// try to read the bt_swarm_resumedata_t
	swarm_resumedata = bt_swarm_resumedata_helper_t::from_file(mfile_path.to_string() + ".neoip_bt.state");

	// if there is no resume data, try to read the .torrent directly	
	if( swarm_resumedata.is_null() ){
		bt_mfile_t	bt_mfile;
		// load the .torrent
		bt_mfile	= bt_mfile_helper_t::from_torrent_file(mfile_path);
		NUNIT_ASSERT( !bt_mfile.is_null() );
		// add the local_path in the bt_mfile.subfile_arr()
		bt_mfile.set_file_local_dir("/tmp/downloaded");
		// complete the bt_mfile_t initialization
		bt_mfile.complete_init();
		// build the bt_swarm_resumedata_t from the read bt_mfile_t
		swarm_resumedata	= bt_swarm_resumedata_t::from_mfile(bt_mfile);
	}
	
	// set the bt_ezswarm_opt_t
	bt_ezswarm_opt_t	ezswarm_opt;
	ezswarm_opt	|= bt_ezswarm_opt_t::IO_SFILE;
	ezswarm_opt	|= bt_ezswarm_opt_t::ECNX_HTTP;
	ezswarm_opt	|= bt_ezswarm_opt_t::PEERSRC_HTTP;
	ezswarm_opt	|= bt_ezswarm_opt_t::PEERSRC_KAD;
	ezswarm_opt	|= bt_ezswarm_opt_t::PEERSRC_UTPEX;

	// start a bt_swarm_t depending on the availability of a bt_swarm_resumedata_t
	bt_ezswarm	= nipmem_new bt_ezswarm_t();
	bt_ezswarm->set_profile(ezswarm_profile);
	bt_err		= bt_ezswarm->set_profile(ezswarm_profile)
				.start(swarm_resumedata, ezswarm_opt, bt_ezsession, this, NULL);
	NUNIT_ASSERT( bt_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			bt_ezsession_t handling
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch the bt_ezsession_t
 */
bt_err_t	bt_ezswarm_testclass_t::launch_ezsession()	throw()
{
	bt_err_t		bt_err;
	ipport_aview_t		tcp_listen_aview;
	ipport_aview_t		udp_listen_aview;
	ipport_addr_t		nslan_addr	= "255.255.255.255:7777";
#if 1
	tcp_listen_aview.lview	(":50001");
	tcp_listen_aview.pview	(":50001");
#else
	tcp_listen_aview.lview	("192.168.1.2:5001");
	tcp_listen_aview.pview	(":5001");
#endif
	udp_listen_aview.lview	(ipport_addr_t::ANY_IP4);
	udp_listen_aview.pview	(ipport_addr_t());

	// set the bt_ezswarm_opt_t
	bt_ezsession_opt_t	ezsession_opt;
	ezsession_opt	|= bt_ezsession_opt_t::KAD_PEER;

	// build the bt_ezsession_profile_t
	bt_ezsession_profile_t	ezsession_profile;
	ezsession_profile.nslan_addr(nslan_addr);
	ezsession_profile.kad_realmid("nunit bt_swarm");

	// start the bt_ezsession_t;
	bt_ezsession	= nipmem_new bt_ezsession_t();
	bt_err		= bt_ezsession->profile(ezsession_profile).start(ezsession_opt, tcp_listen_aview
							, udp_listen_aview, kad_peerid_t::build_random()
							, bt_id_t::build_random());
	if( bt_err.failed() )	return bt_err;


	// return no error
	return bt_err_t::OK;	
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   bt_ezswarm_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_ezswarm_t when to notify an event
 */
bool 	bt_ezswarm_testclass_t::neoip_bt_ezswarm_cb(void *cb_userptr, bt_ezswarm_t &cb_bt_ezswarm
					, const bt_ezswarm_event_t &ezswarm_event)	throw()
{
	// log to debug
	KLOG_WARN("enter ezevent=" << ezswarm_event);
	
	// if it is bt_swarm_event_t::PIECE_NEWLY_AVAIL and that bt_swarm_t is now seed, notify succeess
	if( ezswarm_event.is_swarm_event() && ezswarm_event.get_swarm_event()->is_piece_newly_avail() ){
		bt_swarm_t *	bt_swarm	= bt_ezswarm->share()->bt_swarm();
		if( bt_swarm->is_seed() ){
			neoip_nunit_testclass_deinit();
			nunit_ftor(NUNIT_RES_OK);
			return false;
		}
	}
	
	// return tokeep
	return true;
}


NEOIP_NAMESPACE_END

