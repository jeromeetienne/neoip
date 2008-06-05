/*! \file
    \brief Definition of the unit test for the \ref bytearray2_t

*/

/* system include */
/* local include */
#include "neoip_bytearray2_nunit.hpp"
#include "neoip_bytearray2.hpp"
#include "neoip_cpu_timer.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bytearray2_testclass_t::basic_serial(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
		
	bytearray2_t	bytearray;
	// serialize 100 uint32_t
	for(size_t i = 0; i < 100; i++)	bytearray << (uint32_t)i;
	// serialize 100 uint16_t
	for(size_t i = 0; i < 100; i++)	bytearray << (uint16_t)i;
	// serialize 100 uint8_t
	for(size_t i = 0; i < 100; i++)	bytearray << (uint8_t)i;

	// unserialize 100 uint32_t
	for(size_t i = 0; i < 100; i++){
		uint32_t	tmp;
		bytearray >> tmp;
 		NUNIT_ASSERT( tmp == (uint32_t)i );
 	}
	// unserialize 100 uint16_t
 	for(size_t i = 0; i < 100; i++){
		uint16_t	tmp;
		bytearray >> tmp;
 		NUNIT_ASSERT( tmp == (uint16_t)i );
 	}
	// unserialize 100 uint8_t
	for(size_t i = 0; i < 100; i++){
		uint8_t	tmp;
		bytearray >> tmp;
 		NUNIT_ASSERT( tmp == (uint8_t)i );
 	}

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
nunit_res_t	bytearray2_testclass_t::failed_serial(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	// serialize a uint8_t
	bytearray2_t	bytearray;
	uint8_t		tmp	= 0x42;
	bytearray << tmp;

	// unserialized a uint64_t
	uint64_t	val;
	bool		recv_except	= false;
	try {
		bytearray >> val;
	}catch(serial2_except_t &e){
		recv_except	= true;
	}
	// check that a serial2_except_t has been thrown
	NUNIT_ASSERT( recv_except );

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
nunit_res_t	bytearray2_testclass_t::headtail_freelen(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
		
	bytearray2_t	bytearray(100, 10, 1024);

	// test the head_freelen()/tail_freelen() method
	NUNIT_ASSERT( bytearray.head_freelen()	== 10 );
	NUNIT_ASSERT( bytearray.length() 	==  0 );
	NUNIT_ASSERT( bytearray.tail_freelen()	== 90 );
	bytearray << (uint64_t)42;
	NUNIT_ASSERT( bytearray.head_freelen()	== 10 );
	NUNIT_ASSERT( bytearray.length()	== sizeof(uint64_t) );
	NUNIT_ASSERT( bytearray.tail_freelen()	== 90 - sizeof(uint64_t) );

	uint64_t val;
	bytearray >> val;
	NUNIT_ASSERT( val == 42 );
	NUNIT_ASSERT( bytearray.head_freelen()	== 10 + sizeof(uint64_t) );
	NUNIT_ASSERT( bytearray.length()	==  0 );
	NUNIT_ASSERT( bytearray.tail_freelen()	== 90 - sizeof(uint64_t) );

	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END

