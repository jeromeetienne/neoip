/*! \file
    \brief Definition of the unit test for the \ref bt_alloc_t

- FIXME currently this nunit is only using the bt_alloc_t aka a testbed
  - it is possible to write a selfconsistency test with the bt_check_t 
  - aka bt_alloc_t create stuff which are checked ok by bt_check_t

*/

/* system include */
/* local include */
#include "neoip_bt_alloc_nunit.hpp"
#include "neoip_bt_alloc.hpp"
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
bt_alloc_testclass_t::bt_alloc_testclass_t()	throw()
{
	// zero some field
	bt_alloc	= NULL;
}

/** \brief Destructor
 */
bt_alloc_testclass_t::~bt_alloc_testclass_t()	throw()
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
nunit_err_t	bt_alloc_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// sanity check
	DBG_ASSERT( !bt_alloc );
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	bt_alloc_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// destruct the bt_alloc_t
	nipmem_zdelete bt_alloc;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bt_alloc_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	bt_err_t	bt_err;
	// log to debug
	KLOG_DBG("enter");
	
// FIXME do a real nunit - currently only a testing function

	// load the bencoded mfile
	file_path_t	file_path	= "/home/jerome/downloaded/ubuntu-6.10-desktop-i386.iso.torrent";
	bt_mfile_t	bt_mfile	= bt_mfile_helper_t::from_torrent_file(file_path);
	NUNIT_ASSERT( !bt_mfile.is_null() );
	// add the local_path in the bt_mfile.subfile_arr()
	bt_mfile.set_file_local_dir("/tmp");
	bt_mfile.complete_init();
	
	
	KLOG_ERR("file_path=" << file_path);
	for(size_t i = 0; i < file_path.size(); i++){
		file_path_t subpath	= file_path.subpath(i);
		KLOG_ERR("subpath("<< i <<")=" << subpath);
	}

	// start the bt_alloc_t
	bt_alloc	= nipmem_new bt_alloc_t();
	bt_err		= bt_alloc->start(bt_mfile, "HOLE", this, NULL);
	NUNIT_ASSERT( bt_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     bt_alloc_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_alloc_t to provide event
 */
bool	bt_alloc_testclass_t::neoip_bt_alloc_cb(void *cb_userptr, bt_alloc_t &cb_bt_alloc
				, const bt_err_t &bt_err)	throw() 
{
	// log to debug
	KLOG_DBG("enter bt_err=" << bt_err);
	
	// log the error if needed
	if( bt_err.failed() )	KLOG_ERR("bt_alloc_t failed due to " << bt_err);

	// delete the bt_alloc_t
	nipmem_zdelete bt_alloc;

	// report the result to nunit depending on the notified bt_err_t
	if( bt_err.succeed() )	nunit_ftor( NUNIT_RES_OK	);
	else			nunit_ftor( NUNIT_RES_ERROR	);

	// return dontkeep
	return false;
}



NEOIP_NAMESPACE_END

