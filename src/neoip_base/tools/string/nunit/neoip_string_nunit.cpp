/*! \file
    \brief Definition of the unit test for the \ref string_t
*/

/* system include */
/* local include */
#include "neoip_string_nunit.hpp"
#include "neoip_string.hpp"
#include "neoip_delay.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a string_t
 */
nunit_res_t	string_testclass_t::test_split(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	std::vector<std::string>	part = string_t::split("hello.world", ".");
	NUNIT_ASSERT( part.size() == 2 );
	NUNIT_ASSERT( part[0] == "hello" );
	NUNIT_ASSERT( part[1] == "world" );

	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a string_t
 */
nunit_res_t	string_testclass_t::test_replace(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	NUNIT_ASSERT(string_t::replace("super string", "'", "&apos;") == "super string");
	NUNIT_ASSERT(string_t::replace("super'string", "'", "&apos;") == "super&apos;string");
	NUNIT_ASSERT(string_t::replace("super'string", "super", "hyper") == "hyper'string");
	NUNIT_ASSERT(string_t::replace("abababb", "b", "B") == "aBaBaBB");
	NUNIT_ASSERT(string_t::replace("babababb", "b", "B") == "BaBaBaBB");
	NUNIT_ASSERT(string_t::replace("aababababbaa", "b", "B") == "aaBaBaBaBBaa");

	// return no error
	return NUNIT_RES_OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a string_t
 */
nunit_res_t	string_testclass_t::test_casecmp(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	NUNIT_ASSERT( string_t::casecmp("hello, world", "HeLLo, WoRld") == 0 );

	// return no error
	return NUNIT_RES_OK;
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a string_t
 */
nunit_res_t	string_testclass_t::test_to_upperlower(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	NUNIT_ASSERT( string_t::to_upper("HeLLo, WoRld") == "HELLO, WORLD" );
	NUNIT_ASSERT( string_t::to_lower("HeLLo, WoRld") == "hello, world" );

	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a string_t
 */
nunit_res_t	string_testclass_t::test_escape(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	std::string	original_str	= "Hel%lo, world./have peace all together";
	std::string	escaped_str	= string_t::escape_in(original_str, " %/");
	std::string	unescaped_str	= string_t::unescape(escaped_str);
	NUNIT_ASSERT( unescaped_str == original_str );

	// return no error
	return NUNIT_RES_OK;
}



////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a string_t
 */
nunit_res_t	string_testclass_t::test_strip(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	// test lstrip
	NUNIT_ASSERT( string_t::lstrip(" p pppbonjour", " p") == "bonjour" );
	NUNIT_ASSERT( string_t::lstrip("", " ") == "" );
	// test rstrip
	NUNIT_ASSERT( string_t::rstrip("bonjour p ppp", " p") == "bonjour" );
	NUNIT_ASSERT( string_t::rstrip("", " ") == "" );
	// test strip
	NUNIT_ASSERT( string_t::strip(" p pppbonjour", " p") == "bonjour" );
	NUNIT_ASSERT( string_t::strip("") == "" );

	// return no error
	return NUNIT_RES_OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a string_t
 */
nunit_res_t	string_testclass_t::test_nice_string(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// test the string_t::size_sepa()
	NUNIT_ASSERT( string_t::size_sepa(0)			== "0"		);
	NUNIT_ASSERT( string_t::size_sepa(1)			== "1"		);
	NUNIT_ASSERT( string_t::size_sepa(12)			== "12"		);
	NUNIT_ASSERT( string_t::size_sepa(123)			== "123"	);
	NUNIT_ASSERT( string_t::size_sepa(1234)			== "1,234"	);
	NUNIT_ASSERT( string_t::size_sepa(12345)		== "12,345"	);
	NUNIT_ASSERT( string_t::size_sepa(123456)		== "123,456"	);
	NUNIT_ASSERT( string_t::size_sepa(1234567)		== "1,234,567"	);
	NUNIT_ASSERT( string_t::size_sepa(12345678)		== "12,345,678"	);

	// test the string_t::size_string()
	NUNIT_ASSERT( string_t::size_string(0)			== "    0bytes" );
	NUNIT_ASSERT( string_t::size_string(1020)		== " 1020bytes" );
	NUNIT_ASSERT( string_t::size_string(1024)		== "1.000Kbyte" );
	NUNIT_ASSERT( string_t::size_string(1024+100)		== "1.098Kbyte" );
	NUNIT_ASSERT( string_t::size_string((1024+100)*1023)	== "1.097Mbyte" );
	
	// test the string_t::delay_string()
	NUNIT_ASSERT( string_t::delay_string(delay_t::from_sec(0))		== "    0s" );
	NUNIT_ASSERT( string_t::delay_string(delay_t::from_sec(1))		== "    1s" );
	NUNIT_ASSERT( string_t::delay_string(delay_t::from_sec(60))		== " 1m00s" );
	NUNIT_ASSERT( string_t::delay_string(delay_t::from_sec(20*60+30))	== "20m30s" );
	NUNIT_ASSERT( string_t::delay_string(delay_t::from_sec(25*60*60+30))	== " 1d01h" );
	
	// test the string_t::percent_string()
	NUNIT_ASSERT( string_t::percent_string(0.0)	== "0.00%" );
	NUNIT_ASSERT( string_t::percent_string(0.01)	== "1.00%" );
	NUNIT_ASSERT( string_t::percent_string(0.0531)	== "5.31%" );
	NUNIT_ASSERT( string_t::percent_string(0.1)	== "10.0%" );
	NUNIT_ASSERT( string_t::percent_string(0.532)	== "53.2%" );
	NUNIT_ASSERT( string_t::percent_string(0.99999)	== "99.9%" );
	NUNIT_ASSERT( string_t::percent_string(1.0)	== " 100%" );

//	KLOG_ERR("progressbar=" << string_t::progress_bar_str(0.5, 0.3, 30) );
//	KLOG_ERR("progressbar=" << string_t::progress_bar_str(0.3, 0.3, 30) );
//	KLOG_ERR("progressbar=" << string_t::progress_bar_str(1.0, 0.8, 30) );

	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a string_t
 */
nunit_res_t	string_testclass_t::test_to_uint(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	uint64_t	mult	= 1024;
	// log to debug
	KLOG_DBG("enter");
	
	NUNIT_ASSERT( string_t::to_uint64("3200" ) == uint64_t(3200)			);
	NUNIT_ASSERT( string_t::to_uint64("3200b") == uint64_t(3200)			);
	NUNIT_ASSERT( string_t::to_uint64("3200B") == uint64_t(3200)			);
	NUNIT_ASSERT( string_t::to_uint64("3200k") == uint64_t(3200)*mult		);
	NUNIT_ASSERT( string_t::to_uint64("3200K") == uint64_t(3200)*mult		);
	NUNIT_ASSERT( string_t::to_uint64("3200m") == uint64_t(3200)*mult*mult		);
	NUNIT_ASSERT( string_t::to_uint64("3200M") == uint64_t(3200)*mult*mult		);
	NUNIT_ASSERT( string_t::to_uint64("3200g") == uint64_t(3200)*mult*mult*mult	);
	NUNIT_ASSERT( string_t::to_uint64("3200G") == uint64_t(3200)*mult*mult*mult	);
	
	// test for double before the unit	
	NUNIT_ASSERT( string_t::to_uint64("1.5k")  == uint64_t(1.5*mult)	);

	// return no error
	return NUNIT_RES_OK;
}



NEOIP_NAMESPACE_END

