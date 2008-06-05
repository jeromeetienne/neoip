/*! \file
    \brief Definition of the unit test for the \ref dvar_t
*/

/* system include */
/* local include */
#include "neoip_dvar_nunit.hpp"
#include "neoip_dvar.hpp"
#include "neoip_dvar_helper.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief function to test a dvar_t
 */
nunit_res_t	dvar_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
// test default dvar_t
	dvar_t	dvar;
	// test some query function on the default constructor
	NUNIT_ASSERT( dvar.is_null() );
	NUNIT_ASSERT( dvar.type() == dvar_type_t::NONE );

	// test comparison operator inter type
	dvar_t	dvar1	= dvar_int_t(42);
	dvar_t	dvar2	= dvar_str_t("hello world.");
	NUNIT_ASSERT( dvar1 == dvar1 );
	NUNIT_ASSERT( dvar1 != dvar2 );
	NUNIT_ASSERT( dvar1 <  dvar2 );
	NUNIT_ASSERT( dvar2 >  dvar1 );
	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a dvar_t
 */
nunit_res_t	dvar_testclass_t::integer(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");


	dvar_t	dvar( dvar_int_t(42) );
	// test some query function on the dvar_int_t
	NUNIT_ASSERT( !dvar.is_null() );
	NUNIT_ASSERT( dvar.type() == dvar_type_t::INTEGER );
	NUNIT_ASSERT( dvar.integer().get() == 42 );
	// set another value 
	dvar.integer().set(43);
	NUNIT_ASSERT( dvar.integer().get() == 43 );
	// set another value but in 64bit
	dvar.integer().set(0x7000000000000000LL);
	NUNIT_ASSERT( dvar.integer().to_int64() == 0x7000000000000000LL );
	
	// test comparison operator intra type
	dvar_t	dvar1	= dvar_int_t(42);
	dvar_t	dvar2	= dvar_int_t(99);
	NUNIT_ASSERT( dvar1 == dvar1 );
	NUNIT_ASSERT( dvar1 != dvar2 );
	NUNIT_ASSERT( dvar1 <  dvar2 );
	NUNIT_ASSERT( dvar2 >  dvar1 );
	
	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a dvar_t
 */
nunit_res_t	dvar_testclass_t::dbl(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");


	dvar_t	dvar( dvar_dbl_t(42.6) );
	// test some query function on the dvar_dbl_t
	NUNIT_ASSERT( !dvar.is_null() );
	NUNIT_ASSERT( dvar.type() == dvar_type_t::DOUBLE );
	NUNIT_ASSERT( dvar.dbl().get() == 42.6 );
	// set another value 
	dvar.dbl().set(43.9);
	NUNIT_ASSERT( dvar.dbl().get() == 43.9 );
	
	// test comparison operator intra type
	dvar_t	dvar1	= dvar_dbl_t(42.5);
	dvar_t	dvar2	= dvar_dbl_t(99.9);
	NUNIT_ASSERT( dvar1 == dvar1 );
	NUNIT_ASSERT( dvar1 != dvar2 );
	NUNIT_ASSERT( dvar1 <  dvar2 );
	NUNIT_ASSERT( dvar2 >  dvar1 );
	
	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a dvar_t
 */
nunit_res_t	dvar_testclass_t::string(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	

// test dvar_str_t
	dvar_t	dvar	= dvar_str_t("Hello world.");
	// test some query function on the dvar_str_t
	NUNIT_ASSERT( !dvar.is_null() );
	NUNIT_ASSERT( dvar.type() == dvar_type_t::STRING );
	NUNIT_ASSERT( dvar.str().get() == "Hello world." );
	// set another value 
	dvar.str().set("gratefull salutation");
	NUNIT_ASSERT( dvar.str().get() == "gratefull salutation" );	

	// test comparison operator intra type
	dvar_t	dvar1	= dvar_str_t("gratefull salutation");
	dvar_t	dvar2	= dvar_str_t("hello world.");
	NUNIT_ASSERT( dvar1 == dvar1 );
	NUNIT_ASSERT( dvar1 != dvar2 );
	NUNIT_ASSERT( dvar1 <  dvar2 );
	NUNIT_ASSERT( dvar2 >  dvar1 );

	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a dvar_t
 */
nunit_res_t	dvar_testclass_t::array(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

// test dvar_arr_t
	dvar_t	dvar	= dvar_arr_t();
	// test some query function on the dvar_arr_t
	NUNIT_ASSERT( !dvar.is_null() );
	NUNIT_ASSERT( dvar.arr().size() == 0 );
	NUNIT_ASSERT( dvar.arr().empty() == true );
	NUNIT_ASSERT( dvar.type() == dvar_type_t::ARRAY );
	// add a value to it
	dvar.arr() += dvar_int_t(42);
	NUNIT_ASSERT( dvar.arr().size() == 1 );
	NUNIT_ASSERT( dvar.arr().empty() == false );
	NUNIT_ASSERT( dvar.arr()[0] == dvar_int_t(42) );

	// test comparison operator intra type
	dvar_t	dvar1	= dvar_arr_t().append(dvar_int_t(42)).append(dvar_int_t(98));
	dvar_t	dvar2	= dvar_arr_t().append(dvar_int_t(42)).append(dvar_int_t(99));
	NUNIT_ASSERT( dvar1 == dvar1 );
	NUNIT_ASSERT( dvar1 != dvar2 );
	NUNIT_ASSERT( dvar1 <  dvar2 );
	NUNIT_ASSERT( dvar2 >  dvar1 );
	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a dvar_t
 */
nunit_res_t	dvar_testclass_t::map(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

// test dvar_map_t
	dvar_t	dvar	= dvar_map_t();
	// test some query function on the dvar_map_t
	NUNIT_ASSERT( !dvar.is_null() );
	NUNIT_ASSERT( dvar.map().size() == 0 );
	NUNIT_ASSERT( dvar.map().empty() == true );
	NUNIT_ASSERT( dvar.type() == dvar_type_t::MAP );
	// add a value to it
	dvar.map().insert("magic_number", dvar_int_t(42));
	NUNIT_ASSERT( dvar.map().size() == 1 );
	NUNIT_ASSERT( dvar.map().empty() == false );
	NUNIT_ASSERT( dvar.map().contain("magic_number") );
	NUNIT_ASSERT( dvar.map()["magic_number"] == dvar_int_t(42) );

	// test comparison operator intra type
	dvar_t	dvar1	= dvar_map_t().insert("aaaa", dvar_int_t(42)).insert("bbbb", dvar_int_t(42));
	dvar_t	dvar2	= dvar_map_t().insert("aaaa", dvar_int_t(42)).insert("bbbb", dvar_int_t(43));
	NUNIT_ASSERT( dvar1 == dvar1 );
	NUNIT_ASSERT( dvar1 != dvar2 );
	NUNIT_ASSERT( dvar1 <  dvar2 );
	NUNIT_ASSERT( dvar2 >  dvar1 );

	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a dvar_t
 */
nunit_res_t	dvar_testclass_t::to_xml(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	dvar_t	dvar_main	= dvar_map_t();
	dvar_t	dvar_arr1	= dvar_arr_t().append(dvar_int_t(42)).append(dvar_int_t(98));
	dvar_t	dvar_arr2	= dvar_arr_t().append(dvar_int_t(42)).append(dvar_int_t(99));
	dvar_t	dvar_map1	= dvar_map_t().insert("aaaa", dvar_int_t(42)).insert("bbbb", dvar_int_t(42));
	dvar_main.map().insert("super_array1", dvar_arr1);
	dvar_main.map().insert("super_array2", dvar_arr2);
	dvar_main.map().insert("singleint", dvar_int_t(42));
	dvar_main.map().insert("singlestttrring", dvar_str_t("slota"));
	dvar_main.map().insert("megamap", dvar_map1);
	
	// NOTE: this is not a real nunit, aka it will succeed everytime
	// - this is more a testbed
	KLOG_DBG("dvar=" << dvar_main);
	KLOG_DBG("dvar.to_xml=" << dvar_helper_t::to_xml(dvar_main));

	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END

