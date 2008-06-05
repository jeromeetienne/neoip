/*! \file
    \brief Definition of the unit test for the \ref datum_t

*/

/* system include */
/* local include */
#include "neoip_datum_nunit.hpp"
#include "neoip_datum.hpp"
#include "neoip_serial.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	datum_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// test the is_null() method
	NUNIT_ASSERT( datum_t().is_null() );
	NUNIT_ASSERT( !datum_t("hello", 5).is_null() );
	
	// test the .length() method
	NUNIT_ASSERT( datum_t("hello").length() == 5 );
	// test the + operator
	NUNIT_ASSERT( datum_t("hello") + datum_t(" world") == datum_t("hello world") );
	
	// test the nocopy flag
	const char * text	= "hello";
	NUNIT_ASSERT( datum_t(text, 5, datum_t::NOCOPY).char_ptr() == text	);
	NUNIT_ASSERT( datum_t(text, 5, datum_t::NOCOPY).is_nocopy()		);

	// test the inheritance of the datum_t::SECMEM
	NUNIT_ASSERT( (datum_t("hello", 5, datum_t::SECMEM) + datum_t(" world")).is_secmem() );

	// test the inheritance of the datum_t::NOCOPY
	NUNIT_ASSERT( !(datum_t(text, 5, datum_t::SECMEM) + datum_t(" world")).is_nocopy() );

	// test the range() method
	NUNIT_ASSERT( datum_t(text, 5, datum_t::NOCOPY).range(1, 1, datum_t::NOCOPY).char_ptr() == text+1 );

	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	datum_testclass_t::test_flag(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	datum_flag_t	datum_flag;
	// test the FLAG_DFL
	NUNIT_ASSERT( datum_flag == datum_flag_t::FLAG_DFL);

	// test the OR operation
	datum_flag	|= datum_flag_t::NOCOPY;
	NUNIT_ASSERT( datum_flag.is_nocopy() );
	
	// test the XOR operation
	datum_flag	^= datum_flag_t::NOCOPY;
	NUNIT_ASSERT(!datum_flag.is_nocopy() );
	
	// test the inheritance
	datum_flag	= datum_flag_t::NOCOPY | datum_flag_t::SECMEM;
	NUNIT_ASSERT(!datum_flag.inheritance().is_nocopy() );
	NUNIT_ASSERT( datum_flag.inheritance().is_secmem() );
	
	// return no error
	return NUNIT_RES_OK;
}

/** \brief test the datum_t serialization consistency
 */
nunit_res_t	datum_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	datum_t		obj_toserial;
	datum_t		obj_unserial;
	serial_t	serial;
	
	// test the serialization consistency for null datum
	NUNIT_ASSERT( obj_toserial.is_null() );
	serial << obj_toserial;
	serial >> obj_unserial;
	NUNIT_ASSERT( obj_toserial == obj_unserial );	

	// test the serialization consistency for non null datum
	obj_toserial	= datum_t("hello", 5);
	serial << obj_toserial;
	serial >> obj_unserial;
	NUNIT_ASSERT( obj_toserial == obj_unserial );	

	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

