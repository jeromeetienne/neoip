/*! \file
    \brief Definition of the unit test for the \ref bt_mfile_cpuhash_t

*/

/* system include */
/* local include */
#include "neoip_bt_mfile_cpuhash_nunit.hpp"
#include "neoip_bt_mfile_cpuhash.hpp"
#include "neoip_bt_io_sfile.hpp"
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
bt_mfile_cpuhash_testclass_t::bt_mfile_cpuhash_testclass_t()	throw()
{
	// zero some field
	bt_mfile_cpuhash	= NULL;
	bt_io_vapi		= NULL;
}

/** \brief Destructor
 */
bt_mfile_cpuhash_testclass_t::~bt_mfile_cpuhash_testclass_t()	throw()
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
nunit_err_t	bt_mfile_cpuhash_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// sanity check
	DBG_ASSERT( !bt_mfile_cpuhash );
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	bt_mfile_cpuhash_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// destruct the bt_mfile_cpuhash_t
	nipmem_zdelete	bt_mfile_cpuhash;
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
nunit_res_t	bt_mfile_cpuhash_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	bt_err_t	bt_err;
	bt_mfile_t	bt_mfile;
	// log to debug
	KLOG_DBG("enter");
	
	// build a sample bt_mfile_t
	bt_mfile.announce_uri	( "http://127.0.0.1:4000/announce" );
	bt_mfile.piecelen	( 1024 );
	
	bt_mfile.subfile_arr()	+= bt_mfile_subfile_t("file1", 19).local_path("/tmp/test_neoip_bt/file1");
	bt_mfile.subfile_arr()	+= bt_mfile_subfile_t("file2", 34).local_path("/tmp/test_neoip_bt/file2");
	bt_mfile.subfile_arr()	+= bt_mfile_subfile_t("file3",  0).local_path("/tmp/test_neoip_bt/file3");

	// init the bt_io_vapi_t
	bt_io_sfile_t *	bt_io_sfile;
	bt_io_sfile	= nipmem_new bt_io_sfile_t();
	bt_err		= bt_io_sfile->start(bt_mfile);
	NUNIT_ASSERT( bt_err.succeed() );	
	// assign the bt_io_vapi
	bt_io_vapi	= bt_io_sfile;
	
	
	// start the bt_mfile_cpuhash_t
	bt_mfile_cpuhash= nipmem_new bt_mfile_cpuhash_t();
	bt_err		= bt_mfile_cpuhash->start(bt_mfile, bt_io_vapi, this, NULL);
	NUNIT_ASSERT( bt_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     bt_mfile_cpuhash_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_mfile_cpuhash_t to provide event
 */
bool	bt_mfile_cpuhash_testclass_t::neoip_bt_mfile_cpuhash_cb(void *cb_userptr, bt_mfile_cpuhash_t &cb_bt_mfile_cpuhash
				, const bt_err_t &bt_err, const bt_id_arr_t &piecehash_arr)	throw() 
{
	// log to debug
	KLOG_WARN("enter bt_err=" << bt_err << " piecehash_arr=" << piecehash_arr);

	// delete the bt_mfile_cpuhash_t
	nipmem_zdelete bt_mfile_cpuhash;

	// report the result to nunit depending on the notified bt_err_t
	if( bt_err.succeed() )	nunit_ftor( NUNIT_RES_OK	);
	else			nunit_ftor( NUNIT_RES_ERROR	);

	// return dontkeep
	return false;
}



NEOIP_NAMESPACE_END

