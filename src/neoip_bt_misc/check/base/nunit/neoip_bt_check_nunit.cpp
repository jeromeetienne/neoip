/*! \file
    \brief Definition of the unit test for the \ref bt_check_t

*/

/* system include */
/* local include */
#include "neoip_bt_check_nunit.hpp"
#include "neoip_bt_check.hpp"
#include "neoip_bt_io_sfile.hpp"
#include "neoip_bt_mfile_helper.hpp"
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
bt_check_testclass_t::bt_check_testclass_t()	throw()
{
	// zero some field
	bt_check	= NULL;
	bt_io_vapi	= NULL;
}

/** \brief Destructor
 */
bt_check_testclass_t::~bt_check_testclass_t()	throw()
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
nunit_err_t	bt_check_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// sanity check
	DBG_ASSERT( !bt_check );
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	bt_check_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// destruct the bt_check_t
	nipmem_zdelete bt_check;
	// delete the bt_io_vapi_t if needed
	nipmem_zdelete	bt_io_vapi;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bt_check_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	bt_err_t	bt_err;
	// log to debug
	KLOG_DBG("enter");

	// load the bencoded mfile
	file_path_t	file_path	= "/home/jerome/downloaded/ubuntu-6.06.1-desktop-i386.iso.torrent";
	bt_mfile_t	bt_mfile	= bt_mfile_helper_t::from_torrent_file(file_path);
	NUNIT_ASSERT( !bt_mfile.is_null() );
	// add the local_path in the bt_mfile.subfile_arr()
	bt_mfile.set_file_local_dir("/home/jerome/downloaded");

	// init the bt_io_vapi_t
	bt_io_sfile_t *	bt_io_sfile;
	bt_io_sfile	= nipmem_new bt_io_sfile_t();
	bt_err		= bt_io_sfile->start(bt_mfile);
	NUNIT_ASSERT( bt_err.succeed() );
	
	// assigne the bt_io_vapi
	bt_io_vapi	= bt_io_sfile;

	// start the bt_check_t
	bt_check	= nipmem_new bt_check_t();
	bt_err		= bt_check->start(bt_check_policy_t::SUBFILE_EXIST, bt_mfile, bt_io_vapi, this, NULL);
	NUNIT_ASSERT( bt_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     bt_check_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_check_t to provide event
 */
bool	bt_check_testclass_t::neoip_bt_check_cb(void *cb_userptr, bt_check_t &cb_bt_check
				, const bt_err_t &bt_err
				, const bt_swarm_resumedata_t &swarm_resumedata)	throw() 
{
	// log to debug
	KLOG_DBG("enter bt_err=" << bt_err << " swarm_resumedata=" << swarm_resumedata);

	// delete the bt_check_t
	nipmem_zdelete bt_check;

	// report the result to nunit depending on the notified bt_err_t
	if( bt_err.succeed() )	nunit_ftor( NUNIT_RES_OK	);
	else			nunit_ftor( NUNIT_RES_ERROR	);

	// return dontkeep
	return false;
}



NEOIP_NAMESPACE_END

