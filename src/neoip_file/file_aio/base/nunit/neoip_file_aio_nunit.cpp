/*! \file
    \brief Definition of the unit test for the \ref file_aio_t

*/

/* system include */
/* local include */
#include "neoip_file_aio_nunit.hpp"
#include "neoip_file_aio.hpp"
#include "neoip_file_aread.hpp"
#include "neoip_file_awrite.hpp"
#include "neoip_file_utils.hpp"
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
file_aio_testclass_t::file_aio_testclass_t()	throw()
{
	// zero some field
	file_aio	= NULL;
	file_aread	= NULL;
	file_awrite	= NULL;
}

/** \brief Destructor
 */
file_aio_testclass_t::~file_aio_testclass_t()	throw()
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
nunit_err_t	file_aio_testclass_t::neoip_nunit_testclass_init()	throw()
{
//	file_path_t	file_path	= file_utils_t::get_temp_path("/tmp/neoip_file_aio_nunit");
	file_path_t	file_path	= "/tmp/neoip_file_aio_nunit";
	file_err_t	file_err;
	file_mode_t	file_mode;

	// build the file_aio_profile_t - with short iterlen to stress the mechanism
	file_aio_profile_t	profile;
	profile.aread_iterlen(999).awrite_iterlen(999);
	
	// start a file_aio_t
	file_mode	= file_mode_t::RW | file_mode_t::CREATE | file_mode_t::TRUNCATE;
	file_aio	= nipmem_new file_aio_t();
	file_err	= file_aio->set_profile(profile).start(file_path, file_mode);
	if( file_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, file_err.to_string() );
	
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	file_aio_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
#if 0
	// if a file has been created, remove it now
	if( file_aio )		file_utils_t::remove_file(file_aio->get_path());
#endif
	nipmem_zdelete	file_aio;
	nipmem_zdelete	file_aread;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	file_aio_testclass_t::awrite(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	file_err_t	file_err;
	// build the data2write
	data2write	= datum_t(10000);
	for(size_t i = 0; i < data2write.size(); i++)	*((char *)data2write.get_data()+i)	= i;
	// start a file_awrite_t
	file_awrite	= nipmem_new file_awrite_t(file_aio);
	file_err	= file_awrite->start(0, data2write, this, NULL);
	NUNIT_ASSERT( file_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        file_awrite_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref file_awrite_t when to notify the result of the operation
 */
bool	file_aio_testclass_t::neoip_file_awrite_cb(void *cb_userptr, file_awrite_t &cb_file_awrite
					, const file_err_t &file_err)	throw()
{
	// log to debug
	KLOG_DBG("enter file_err=" << file_err);
	// delete the file_awrite_t
	nipmem_zdelete	file_awrite;

	// notify nunit depending on the result
	if( file_err.succeed() )	nunit_ftor(NUNIT_RES_OK);
	else				nunit_ftor(NUNIT_RES_ERROR);		
	
	// return dontkeep
	return false;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	file_aio_testclass_t::aread(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	file_err_t	file_err;
	// start a file_aread_t
	file_aread	= nipmem_new file_aread_t(file_aio);
	file_err	= file_aread->start(0, data2write.size(), this, NULL);
	NUNIT_ASSERT( file_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        file_aread_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref file_aread_t when to notify the result of the operation
 */
bool	file_aio_testclass_t::neoip_file_aread_cb(void *cb_userptr, file_aread_t &cb_file_aread
					, const file_err_t &file_err, const datum_t &readdata)	throw()
{
	// log to debug
	KLOG_DBG("enter file_err=" << file_err);

	// notify nunit depending on the result
	if( readdata == data2write && file_err.succeed() )	nunit_ftor(NUNIT_RES_OK);
	else							nunit_ftor(NUNIT_RES_ERROR);		
	
	// delete the file_aread_t
	nipmem_zdelete	file_aread;

	// return dontkeep
	return false;
}

NEOIP_NAMESPACE_END

