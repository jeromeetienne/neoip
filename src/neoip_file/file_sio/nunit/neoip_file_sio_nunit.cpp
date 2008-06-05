/*! \file
    \brief Definition of the unit test for the \ref file_sio_t

*/

/* system include */
/* local include */
#include "neoip_file_sio_nunit.hpp"
#include "neoip_file_sio.hpp"
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
file_sio_testclass_t::file_sio_testclass_t()	throw()
{
	// zero some field
	file_sio	= NULL;
}

/** \brief Destructor
 */
file_sio_testclass_t::~file_sio_testclass_t()	throw()
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
nunit_err_t	file_sio_testclass_t::neoip_nunit_testclass_init()	throw()
{
	file_path_t	file_path	= file_utils_t::get_temp_path("/tmp/neoip_file_sio_nunit");
	file_err_t	file_err;
	file_mode_t	file_mode;
	
	// start a file_sio_t
	file_mode	= file_mode_t::RW | file_mode_t::CREATE | file_mode_t::TRUNCATE;
	file_sio	= nipmem_new file_sio_t();
	file_err	= file_sio->start(file_path, file_mode);
	if( file_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, file_err.to_string() );

	// build the data2write
	data2write	= datum_t(10000);
	for(size_t i = 0; i < data2write.size(); i++)	*((char *)data2write.get_data()+i)	= i;
	
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	file_sio_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// if a file has been created, remove it now
	if( file_sio )	file_utils_t::remove_file(file_sio->get_path());
	// delete the file_sio if needed
	nipmem_zdelete	file_sio;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	file_sio_testclass_t::write(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	file_err_t	file_err;
	// seek to the position 0
	file_err	= file_sio->seek(0);
	NUNIT_ASSERT( file_err.succeed() );
	
	// write the data to the file
	file_err	= file_sio->write(data2write);
	NUNIT_ASSERT( file_err.succeed() );

	// report no error
	return NUNIT_RES_OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	file_sio_testclass_t::read(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	file_err_t	file_err;
	// seek to the position 0
	file_err	= file_sio->seek(0);
	NUNIT_ASSERT( file_err.succeed() );
	
	// read the data from the file
	datum_t		data_out;
	file_err	= file_sio->read(data2write.size(), data_out);
	NUNIT_ASSERT( file_err.succeed() );
	NUNIT_ASSERT( data2write == data_out );

	// report no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

