/*! \file
    \brief Definition of the unit test for the \ref bt_mfile_t

- FIXME this is more a testbed that a real nunit, code a real nunit

*/

/* system include */
#include <string>
/* local include */
#include "neoip_bt_mfile_nunit.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_bt_mfile_helper.hpp"
#include "neoip_file_sio.hpp"
#include "neoip_file_stat.hpp"
#include "neoip_file_err.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bt_mfile_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	datum_t		bencoded_mfile;	
	// log to debug
	KLOG_DBG("enter");

	// create the bt_mfile_t from the bencode_mfile
	file_path_t	file_path	= "/home/jerome/downloaded/ubuntu-6.06.1-desktop-i386.iso.torrent";
	bt_mfile_t	bt_mfile	= bt_mfile_helper_t::from_torrent_file(file_path);
	NUNIT_ASSERT( !bt_mfile.is_null() );
	// add the local_path in the bt_mfile.subfile_arr()
	bt_mfile.set_file_local_dir("/home/jerome/downloaded");
	// complete the bt_mfile_t initialization
	bt_mfile.complete_init();
	// log to debug
	KLOG_DBG("filename=" << file_path << "bt_mfile=" << bt_mfile);
	
	datum_t	slota_mfile	= bt_mfile.to_bencode();
	KLOG_DBG("back into the bencoded format=" << slota_mfile);


	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

