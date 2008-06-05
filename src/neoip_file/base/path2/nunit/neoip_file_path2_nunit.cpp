/*! \file
    \brief Definition of the unit test for the \ref file_path2_t

*/

/* system include */
/* local include */
#include "neoip_file_path2_nunit.hpp"
#include "neoip_file_path2.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	file_path2_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// test the is_null()
	NUNIT_ASSERT(  file_path2_t().is_null() );
	NUNIT_ASSERT( !file_path2_t("/tmp/slota").is_null() );

	// report no error
	return NUNIT_RES_OK;
}
	
	
/** \brief Test function
 */
nunit_res_t	file_path2_testclass_t::winpath(const nunit_testclass_ftor_t &testclass_ftor)	throw()
{
	NUNIT_ASSERT( !file_path2_t("\\hello\\world").has_drvletter() );
	NUNIT_ASSERT(  file_path2_t("\\hello\\world").is_absolute() );
	NUNIT_ASSERT( !file_path2_t("\\hello\\world").is_relative() );
	NUNIT_ASSERT(  file_path2_t("\\hello\\world").to_winpath_string() == "\\hello\\world" );
	
	NUNIT_ASSERT(  file_path2_t("z:\\hello\\world").has_drvletter() );
	NUNIT_ASSERT(  file_path2_t("z:hello\\world").drvletter() == "z" );
	NUNIT_ASSERT(  file_path2_t("z:\\hello\\world").is_absolute() );
	NUNIT_ASSERT( !file_path2_t("z:\\hello\\world").is_relative() );
	NUNIT_ASSERT(  file_path2_t("z:\\hello\\world").to_winpath_string() == "z:\\hello\\world" );

	NUNIT_ASSERT(  file_path2_t("z:hello\\world").has_drvletter() );
	NUNIT_ASSERT(  file_path2_t("z:hello\\world").drvletter() == "z" );
	NUNIT_ASSERT( !file_path2_t("z:hello\\world").is_absolute() );
	NUNIT_ASSERT(  file_path2_t("z:hello\\world").is_relative() );
	NUNIT_ASSERT(  file_path2_t("z:hello\\world").to_winpath_string() == "z:hello\\world" );

	// report no error
	return NUNIT_RES_OK;
}


/** \brief Test function
 */
nunit_res_t	file_path2_testclass_t::slash_operator(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	file_path2_t	file_path2;

	// test the concatenation for relative path2
	file_path2	= file_path2_t() / "tmp" / "slota";
	NUNIT_ASSERT( file_path2 == "./tmp/slota" );

	// test the concatenation for absolute path2
	file_path2	= file_path2_t();
	file_path2	/= "/";
	file_path2	/= "tmp";
	file_path2	/= "slota";
	NUNIT_ASSERT( file_path2 == "/tmp/slota" );
	
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
nunit_res_t	file_path2_testclass_t::name_level(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	file_path2_t	file_path2;

	file_path2	= "/";
	NUNIT_ASSERT( file_path2.subpath(0) == "/" );
	NUNIT_ASSERT( file_path2[0] == "/" );

	file_path2	= ".";
	NUNIT_ASSERT( file_path2.subpath(0) == "." );
	NUNIT_ASSERT( file_path2[0] == "." );

	file_path2	= "/tmp/bla/bli";
	NUNIT_ASSERT( file_path2.subpath(0) == "/" );
	NUNIT_ASSERT( file_path2.subpath(1) == "/tmp" );
	NUNIT_ASSERT( file_path2.subpath(2) == "/tmp/bla" );
	NUNIT_ASSERT( file_path2.subpath(3) == "/tmp/bla/bli" );
	NUNIT_ASSERT( file_path2[0] == "/" );
	NUNIT_ASSERT( file_path2[1] == "tmp" );
	NUNIT_ASSERT( file_path2[2] == "bla" );
	NUNIT_ASSERT( file_path2[3] == "bli" );

	file_path2	= "tmp/bla/bli";
	NUNIT_ASSERT( file_path2.subpath(0) == "." );
	NUNIT_ASSERT( file_path2.subpath(1) == "tmp" );
	NUNIT_ASSERT( file_path2.subpath(2) == "tmp/bla" );
	NUNIT_ASSERT( file_path2.subpath(3) == "tmp/bla/bli" );
	NUNIT_ASSERT( file_path2[0] == "." );
	NUNIT_ASSERT( file_path2[1] == "tmp" );
	NUNIT_ASSERT( file_path2[2] == "bla" );
	NUNIT_ASSERT( file_path2[3] == "bli" );
	
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
nunit_res_t	file_path2_testclass_t::full_path2(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	NUNIT_ASSERT( file_path2_t("cfg_file").fullpath2() == "/home/jerome/workspace/yavipin/src/cfg_file");
	
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
nunit_res_t	file_path2_testclass_t::canonisation(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	NUNIT_ASSERT( file_path2_t("/tmp/slot/bla/../bli")	== "/tmp/slot/bli");
	NUNIT_ASSERT( file_path2_t("/tmp/////")			== "/tmp");
	NUNIT_ASSERT( file_path2_t("//////")			== "/");
	NUNIT_ASSERT( file_path2_t("")				== ".");
	NUNIT_ASSERT( file_path2_t(".")				== ".");
	NUNIT_ASSERT( file_path2_t("/tmp/./bla/././")		== "/tmp/bla");
	NUNIT_ASSERT( file_path2_t("..")			== "..");
	NUNIT_ASSERT( file_path2_t("/")				== "/");
	NUNIT_ASSERT( file_path2_t("/..")			== "/");
	NUNIT_ASSERT( file_path2_t("///.///..")			== "/");
	NUNIT_ASSERT( file_path2_t("/////../.././/.././tmp")	== "/tmp");
	NUNIT_ASSERT( file_path2_t("../../tmp")			== "../../tmp");
	
	// report no error
	return NUNIT_RES_OK;
}

/** \brief test the serial consistency
 */
nunit_res_t	file_path2_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	file_path2_t	file_path2_toserial = "/tmp/bla/bli";
	file_path2_t	file_path2_unserial;
	serial_t	serial;
	// do the serial/unserial
	serial << file_path2_toserial;
	serial >> file_path2_unserial;
	// test the serialization consistency
	NUNIT_ASSERT( file_path2_toserial == file_path2_unserial );
	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END

