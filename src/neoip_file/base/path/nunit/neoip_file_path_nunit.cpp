/*! \file
    \brief Definition of the unit test for the \ref file_path_t

*/

/* system include */
/* local include */
#include "neoip_file_path_nunit.hpp"
#include "neoip_file_path.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	file_path_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// test the is_null()
	NUNIT_ASSERT(  file_path_t().is_null() );
	NUNIT_ASSERT( !file_path_t("/tmp/slota").is_null() );

	// report no error
	return NUNIT_RES_OK;
}
	
	
/** \brief Test function
 */
nunit_res_t	file_path_testclass_t::winpath(const nunit_testclass_ftor_t &testclass_ftor)	throw()
{
	NUNIT_ASSERT( !file_path_t("\\hello\\world").has_drvletter() );
	NUNIT_ASSERT(  file_path_t("\\hello\\world").is_absolute() );
	NUNIT_ASSERT( !file_path_t("\\hello\\world").is_relative() );
	NUNIT_ASSERT(  file_path_t("\\hello\\world").to_winpath_string() == "\\hello\\world" );
	
	NUNIT_ASSERT(  file_path_t("z:\\hello\\world").has_drvletter() );
	NUNIT_ASSERT(  file_path_t("z:hello\\world").drvletter() == "z" );
	NUNIT_ASSERT(  file_path_t("z:\\hello\\world").is_absolute() );
	NUNIT_ASSERT( !file_path_t("z:\\hello\\world").is_relative() );
	NUNIT_ASSERT(  file_path_t("z:\\hello\\world").to_winpath_string() == "z:\\hello\\world" );

	NUNIT_ASSERT(  file_path_t("z:hello\\world").has_drvletter() );
	NUNIT_ASSERT(  file_path_t("z:hello\\world").drvletter() == "z" );
	NUNIT_ASSERT( !file_path_t("z:hello\\world").is_absolute() );
	NUNIT_ASSERT(  file_path_t("z:hello\\world").is_relative() );
	NUNIT_ASSERT(  file_path_t("z:hello\\world").to_winpath_string() == "z:hello\\world" );

	// report no error
	return NUNIT_RES_OK;
}


/** \brief Test function
 */
nunit_res_t	file_path_testclass_t::slash_operator(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	file_path_t	file_path;

	// test the concatenation for relative path
	file_path	= file_path_t() / "tmp" / "slota";
	NUNIT_ASSERT( file_path == "./tmp/slota" );

	// test the concatenation for absolute path
	file_path	= file_path_t();
	file_path	/= "/";
	file_path	/= "tmp";
	file_path	/= "slota";
	NUNIT_ASSERT( file_path == "/tmp/slota" );
	
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
nunit_res_t	file_path_testclass_t::name_level(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	file_path_t	file_path;

	file_path	= "/";
	NUNIT_ASSERT( file_path.subpath(0) == "/" );
	NUNIT_ASSERT( file_path[0] == "/" );

	file_path	= ".";
	NUNIT_ASSERT( file_path.subpath(0) == "." );
	NUNIT_ASSERT( file_path[0] == "." );

	file_path	= "/tmp/bla/bli";
	NUNIT_ASSERT( file_path.subpath(0) == "/" );
	NUNIT_ASSERT( file_path.subpath(1) == "/tmp" );
	NUNIT_ASSERT( file_path.subpath(2) == "/tmp/bla" );
	NUNIT_ASSERT( file_path.subpath(3) == "/tmp/bla/bli" );
	NUNIT_ASSERT( file_path[0] == "/" );
	NUNIT_ASSERT( file_path[1] == "tmp" );
	NUNIT_ASSERT( file_path[2] == "bla" );
	NUNIT_ASSERT( file_path[3] == "bli" );

	file_path	= "tmp/bla/bli";
	NUNIT_ASSERT( file_path.subpath(0) == "." );
	NUNIT_ASSERT( file_path.subpath(1) == "tmp" );
	NUNIT_ASSERT( file_path.subpath(2) == "tmp/bla" );
	NUNIT_ASSERT( file_path.subpath(3) == "tmp/bla/bli" );
	NUNIT_ASSERT( file_path[0] == "." );
	NUNIT_ASSERT( file_path[1] == "tmp" );
	NUNIT_ASSERT( file_path[2] == "bla" );
	NUNIT_ASSERT( file_path[3] == "bli" );
	
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
nunit_res_t	file_path_testclass_t::full_path(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
#ifndef	_WIN32	// file_path_t::fullpath_t use the current dir.
		// which is different under native linux and wine
	std::string	expected_result	= "/home/jerome/workspace/yavipin/src/cfg_file";
#else
	std::string	expected_result	= "z:\\home\\jerome\\workspace\\yavipin\\src\\cfg_file";
#endif
	NUNIT_ASSERT( file_path_t("cfg_file").fullpath() == expected_result);
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
nunit_res_t	file_path_testclass_t::canonisation(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	NUNIT_ASSERT( file_path_t("/tmp/slot/bla/../bli")	== "/tmp/slot/bli");
	NUNIT_ASSERT( file_path_t("/tmp/////")			== "/tmp");
	NUNIT_ASSERT( file_path_t("//////")			== "/");
	NUNIT_ASSERT( file_path_t("")				== ".");
	NUNIT_ASSERT( file_path_t(".")				== ".");
	NUNIT_ASSERT( file_path_t("/tmp/./bla/././")		== "/tmp/bla");
	NUNIT_ASSERT( file_path_t("..")			== "..");
	NUNIT_ASSERT( file_path_t("/")				== "/");
	NUNIT_ASSERT( file_path_t("/..")			== "/");
	NUNIT_ASSERT( file_path_t("///.///..")			== "/");
	NUNIT_ASSERT( file_path_t("/////../.././/.././tmp")	== "/tmp");
	NUNIT_ASSERT( file_path_t("../../tmp")			== "../../tmp");
	
	// report no error
	return NUNIT_RES_OK;
}

/** \brief test the serial consistency
 */
nunit_res_t	file_path_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	file_path_t	file_path_toserial = "/tmp/bla/bli";
	file_path_t	file_path_unserial;
	serial_t	serial;
	// do the serial/unserial
	serial << file_path_toserial;
	serial >> file_path_unserial;
	// test the serialization consistency
	NUNIT_ASSERT( file_path_toserial == file_path_unserial );
	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END

