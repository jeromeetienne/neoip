/*! \file
    \brief Definition of the unit test for the \ref serial2_t

*/

/* system include */
/* local include */
#include "neoip_serial2_nunit.hpp"
#include "neoip_serial2.hpp"
#include "neoip_cpu_timer.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;


#define NEOIP_SERIAL2_INHERITANCE_START(class_name)							\
class class_name : public serial2_t {									\
public:													\
	class_name(size_t new_init_length = INIT_LENGTH_DFL, size_t new_head_length = HEAD_LENGTH_DFL	\
			, size_t new_grow_length = GROW_LENGTH_DFL)	throw() 			\
			: serial2_t(new_init_length, new_head_length, new_grow_length) 		{}	\
	class_name(const void *data, int len)			throw() : serial2_t(data, len)	{}	\
	explicit class_name(const datum_t &datum)		throw() : serial2_t(datum)	{}	\
	template <class T> class_name &unserial2_peek(T &val)	throw(serial2_except_t)			\
							{ serial2_t::unserial_peek(val); return *this;}	\
	template <class T> class_name &serialize(const T &val)	throw()					\
								{ *this << val; return *this;	}	\
	template <class T> class_name &unserialize(T &val)	throw(serial2_except_t)			\
								{ *this >> val; return *this;	}
		
#define NEOIP_SERIAL2_INHERITANCE_END(class_name)							\
};


NEOIP_SERIAL2_INHERITANCE_START(bytearray3_t)
NEOIP_SERIAL2_INHERITANCE_END(bytearray3_t);
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	serial2_testclass_t::inheritance(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
		
	serial2_t	serial(400, 200, 256);
	
	for(size_t j = 0; j < 10; j++ ){
		// serialize 100 uint32_t
		for(size_t i = 0; i < 100; i++)	serial << (uint32_t)i;
	
		// unserialize 100 uint32_t
		for(size_t i = 0; i < 100; i++){
			uint32_t	tmp;
			serial >> tmp;
	 		NUNIT_ASSERT( tmp == (uint32_t)i );
	 	}
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
nunit_res_t	serial2_testclass_t::basic_serial(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
		
	serial2_t	serial;
	// check that the default serial2_t is_null()
	NUNIT_ASSERT( serial.is_null() );
	// serialize 100 uint32_t
	for(size_t i = 0; i < 100; i++)	serial << (uint32_t)i;
	// serialize 100 uint16_t
	for(size_t i = 0; i < 100; i++)	serial << (uint16_t)i;
	// serialize 100 uint8_t
	for(size_t i = 0; i < 100; i++)	serial << (uint8_t)i;

	// unserialize 100 uint32_t
	for(size_t i = 0; i < 100; i++){
		uint32_t	tmp;
		serial >> tmp;
 		NUNIT_ASSERT( tmp == (uint32_t)i );
 	}
	// unserialize 100 uint16_t
 	for(size_t i = 0; i < 100; i++){
		uint16_t	tmp;
		serial >> tmp;
 		NUNIT_ASSERT( tmp == (uint16_t)i );
 	}
	// unserialize 100 uint8_t
	for(size_t i = 0; i < 100; i++){
		uint8_t	tmp;
		serial >> tmp;
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
nunit_res_t	serial2_testclass_t::failed_serial(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	// serialize a uint8_t
	serial2_t	serial;
	uint8_t		tmp	= 0x42;
	serial << tmp;

	// unserialized a uint64_t
	uint64_t	val;
	bool		recv_except	= false;
	try {
		serial >> val;
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
nunit_res_t	serial2_testclass_t::headtail_freelen(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
		
	serial2_t	serial(100, 10, 1024);

	// test the head_freelen()/tail_freelen() method
	NUNIT_ASSERT( serial.head_freelen()	== 10 );
	NUNIT_ASSERT( serial.length() 		==  0 );
	NUNIT_ASSERT( serial.tail_freelen()	== 90 );
	serial << (uint64_t)42;
	NUNIT_ASSERT( serial.head_freelen()	== 10 );
	NUNIT_ASSERT( serial.length()		== sizeof(uint64_t) );
	NUNIT_ASSERT( serial.tail_freelen()	== 90 - sizeof(uint64_t) );

	uint64_t val;
	serial >> val;
	NUNIT_ASSERT( val == 42 );
	NUNIT_ASSERT( serial.head_freelen()	== 10 + sizeof(uint64_t) );
	NUNIT_ASSERT( serial.length()		==  0 );
	NUNIT_ASSERT( serial.tail_freelen()	== 90 - sizeof(uint64_t) );

	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END

