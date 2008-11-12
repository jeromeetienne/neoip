/*! \file
    \brief Definition of the unit test for the \ref bt_httpi_t

*/

/* system include */
/* local include */
#include "neoip_bt_httpi_nunit.hpp"
#include "neoip_bt_httpi.hpp"
#include "neoip_bt_scasti_event.hpp"
#include "neoip_bt_cast_mdata.hpp"
#include "neoip_bt_cast_helper.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_bt_io_pfile.hpp"
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
bt_httpi_testclass_t::bt_httpi_testclass_t()	throw()
{
	// zero some field
	bt_httpi	= NULL;
}

/** \brief Destructor
 */
bt_httpi_testclass_t::~bt_httpi_testclass_t()	throw()
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
nunit_err_t	bt_httpi_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// sanity check
	DBG_ASSERT( !bt_httpi );
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	bt_httpi_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// destruct the bt_httpi_t if needed
	nipmem_zdelete	bt_httpi;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bt_httpi_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	bt_err_t	bt_err;
	// log to debug
	KLOG_DBG("enter");

	bt_cast_mdata_t	cast_mdata;
	cast_mdata.cast_name	("slota");
	
	// create the bt_mfile_t
	bt_mfile_t	bt_mfile	= bt_cast_helper_t::build_mfile(cast_mdata);
	
	// create the bt_io_pfile_profile_t
	bt_io_pfile_profile_t	profile;
	profile.dest_dirpath	("/tmp/blabla");
	
	// create the bt_io_vapi_t
	bt_io_pfile_t *	bt_io_pfile;
	bt_io_pfile	= nipmem_new bt_io_pfile_t();
	bt_err		= bt_io_pfile->profile(profile).start(bt_mfile);
	NUNIT_ASSERT( bt_err.succeed() );

	// start the bt_httpi_t
	bt_httpi	= nipmem_new bt_httpi_t();
	bt_err		= bt_httpi->start("http://jmehost2:8000/example1.ogg", bt_io_pfile
					, bt_scasti_mod_type_t::RAW, this, NULL);
	NUNIT_ASSERT( bt_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     bt_httpi_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_httpi_t to provide event
 */
bool	bt_httpi_testclass_t::neoip_bt_httpi_cb(void *cb_userptr, bt_httpi_t &cb_bt_httpi
				, const bt_scasti_event_t &scasti_event)	throw() 
{
	// log to debug
	KLOG_ERR("enter bt_scasti_event_t=" << scasti_event);

	// TODO to code something here :)
	// - currently 
	
	if( scasti_event.is_fatal() ){
		neoip_nunit_testclass_deinit();
		nunit_ftor( NUNIT_RES_ERROR	);
	}

#if 1
	return true;
#else
	// delete the bt_httpi_t
	nipmem_zdelete bt_httpi;

	// return dontkeep
	return false;
#endif
}



NEOIP_NAMESPACE_END

