/*! \file
    \brief Definition of the unit test for interface_t template
    
*/

/* system include */
/* local include */
#include "neoip_interval_nunit.hpp"
#include "neoip_interval.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     interval_item_t test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test the interval_item_t ctor
 */
nunit_res_t	interval_testclass_t::item_ctor(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	interval_item_t<uint32_t>	item;
	// test the ctor with distinct min_value/max_value	
	item	= interval_item_t<uint32_t>(2,4);
	NUNIT_ASSERT( item.min_value() == 2 );
	NUNIT_ASSERT( item.max_value() == 4 );
	// test the ctor with equal min_value/max_value	
	item	= interval_item_t<uint32_t>(7);
	NUNIT_ASSERT( item.min_value() == 7 );
	NUNIT_ASSERT( item.max_value() == 7 );
	// return no error
	return NUNIT_RES_OK;
}

/** \brief test the interval_item_t serialization consistency
 */
nunit_res_t	interval_testclass_t::item_serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	interval_item_t<uint32_t>	item_toserial;
	interval_item_t<uint32_t>	item_unserial;
	serial_t			serial;
	// set item_toserial
	item_toserial	= interval_item_t<uint32_t>(99, 104);
	// do the serial/unserial
	serial << item_toserial;
	serial >> item_unserial;
	// test the serialization consistency
	NUNIT_ASSERT( item_toserial == item_unserial );	
	// return no error
	return NUNIT_RES_OK;
}

/** \brief test the interval_item_t::is_distinct()
 */
nunit_res_t	interval_testclass_t::item_is_distinct(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	interval_item_t<uint32_t>	item(4,7);
	NUNIT_ASSERT( item.is_distinct(interval_item_t<uint32_t>(8, 9)) );
	NUNIT_ASSERT(!item.is_distinct(interval_item_t<uint32_t>(7, 9)) );
	NUNIT_ASSERT(!item.is_distinct(interval_item_t<uint32_t>(2, 4)) );
	NUNIT_ASSERT( item.is_distinct(interval_item_t<uint32_t>(0, 3)) );
	// return no error
	return NUNIT_RES_OK;
}

/** \brief test the interval_item_t::fully_include()
 */
nunit_res_t	interval_testclass_t::item_fully_include(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	interval_item_t<uint32_t>	item(4,7);
	NUNIT_ASSERT( item.fully_include(interval_item_t<uint32_t>(4,7)) );
	NUNIT_ASSERT( item.fully_include(interval_item_t<uint32_t>(5,6)) );
	NUNIT_ASSERT(!item.fully_include(interval_item_t<uint32_t>(4,8)) );
	NUNIT_ASSERT(!item.fully_include(interval_item_t<uint32_t>(7,10)) );
	NUNIT_ASSERT(!item.fully_include(interval_item_t<uint32_t>(8,10)) );
	NUNIT_ASSERT(!item.fully_include(interval_item_t<uint32_t>(2,10)) );
	NUNIT_ASSERT(!item.fully_include(interval_item_t<uint32_t>(2,5)) );
	NUNIT_ASSERT(!item.fully_include(interval_item_t<uint32_t>(2,4)) );
	// return no error
	return NUNIT_RES_OK;
}

/** \brief test the interval_t += operator for interval_item_t
 */
nunit_res_t	interval_testclass_t::interval_add_item(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	interval_t<uint32_t>	interval1;
	interval_t<uint32_t>	interval2;
	// create the basic interval_t
	interval1	+= interval_item_t<uint32_t>(10,13);
	interval1	+= interval_item_t<uint32_t>(18,19);
	// check the basic interval_t
	NUNIT_ASSERT( interval1.size() == 2 );
	NUNIT_ASSERT( interval1[0] == interval_item_t<uint32_t>(10,13) );
	NUNIT_ASSERT( interval1[1] == interval_item_t<uint32_t>(18,19) );
	
// full distinct before any existing item
	interval2	= interval1 + interval_item_t<uint32_t>( 2, 5);
	// check the basic interval_t
	NUNIT_ASSERT( interval2.size() == 3 );
	NUNIT_ASSERT( interval2[0] == interval_item_t<uint32_t>( 2, 5) );
	NUNIT_ASSERT( interval2[1] == interval_item_t<uint32_t>(10,13) );
	NUNIT_ASSERT( interval2[2] == interval_item_t<uint32_t>(18,19) );

// full distinct but before and contiguous to the first item
	interval2	= interval1 + interval_item_t<uint32_t>( 8, 9);
	// check the basic interval_t
	NUNIT_ASSERT( interval2.size() == 2 );
	NUNIT_ASSERT( interval2[0] == interval_item_t<uint32_t>( 8,13) );
	NUNIT_ASSERT( interval2[1] == interval_item_t<uint32_t>(18,19) );

// full distinct in the middle of existing items but not contiguous
	interval2	= interval1 + interval_item_t<uint32_t>(15,16);
	// check the basic interval_t
	NUNIT_ASSERT( interval2.size() == 3 );
	NUNIT_ASSERT( interval2[0] == interval_item_t<uint32_t>(10,13) );
	NUNIT_ASSERT( interval2[1] == interval_item_t<uint32_t>(15,16) );
	NUNIT_ASSERT( interval2[2] == interval_item_t<uint32_t>(18,19) );

// full distinct in the middle of existing items but contiguous to the second item
	interval2	= interval1 + interval_item_t<uint32_t>(16,17);
	// check the basic interval_t
	NUNIT_ASSERT( interval2.size() == 2 );
	NUNIT_ASSERT( interval2[0] == interval_item_t<uint32_t>(10,13) );
	NUNIT_ASSERT( interval2[1] == interval_item_t<uint32_t>(16,19) );

// full distinct in the middle of existing items but contiguous to the second item
	interval2	= interval1 + interval_item_t<uint32_t>(16,17);
	// check the basic interval_t
	NUNIT_ASSERT( interval2.size() == 2 );
	NUNIT_ASSERT( interval2[0] == interval_item_t<uint32_t>(10,13) );
	NUNIT_ASSERT( interval2[1] == interval_item_t<uint32_t>(16,19) );

// full distinct after any existing item
	interval2	= interval1 + interval_item_t<uint32_t>(21, 25);
	// check the basic interval_t
	NUNIT_ASSERT( interval2.size() == 3 );
	NUNIT_ASSERT( interval2[0] == interval_item_t<uint32_t>(10,13) );
	NUNIT_ASSERT( interval2[1] == interval_item_t<uint32_t>(18,19) );
	NUNIT_ASSERT( interval2[2] == interval_item_t<uint32_t>(21,25) );

// containing the whole first item
	interval2	= interval1 + interval_item_t<uint32_t>( 0,14);
	// check the basic interval_t
	NUNIT_ASSERT( interval2.size() == 2 );
	NUNIT_ASSERT( interval2[0] == interval_item_t<uint32_t>( 0,14) );
	NUNIT_ASSERT( interval2[1] == interval_item_t<uint32_t>(18,19) );

// containing the whole second item
	interval2	= interval1 + interval_item_t<uint32_t>(15,25);
	// check the basic interval_t
	NUNIT_ASSERT( interval2.size() == 2 );
	NUNIT_ASSERT( interval2[0] == interval_item_t<uint32_t>(10,13) );
	NUNIT_ASSERT( interval2[1] == interval_item_t<uint32_t>(15,25) );

// containing the whole interval
	interval2	= interval1 + interval_item_t<uint32_t>(0, 25);
	// check the basic interval_t
	NUNIT_ASSERT( interval2.size() == 1 );
	NUNIT_ASSERT( interval2[0] == interval_item_t<uint32_t>( 0,25) );

#if 0
// TODO to remvoe - just some testing with ip_addr_t	
	interval_t<ip_addr_t>	interval_addr;
	interval_addr	+= interval_item_t<ip_addr_t>("127.0.0.0", "127.0.0.10");
	interval_addr	+= interval_item_t<ip_addr_t>("127.0.0.20", "127.0.0.30");	
	KLOG_ERR("interval_addr=" << interval_addr);
	interval_addr	+= interval_item_t<ip_addr_t>("127.0.0.12", "127.0.0.20");
	KLOG_ERR("interval_addr=" << interval_addr);
	interval_addr	+= interval_item_t<ip_addr_t>("127.0.0.11");
	KLOG_ERR("interval_addr=" << interval_addr);
#endif	
	// return no error
	return NUNIT_RES_OK;
}
/** \brief test the interval_t -= operator for interval_item_t
 */
nunit_res_t	interval_testclass_t::interval_sub_item(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	interval_t<uint32_t>	interval1;
	interval_t<uint32_t>	interval2;
	// create the basic interval_t
	interval1	+= interval_item_t<uint32_t>(10,20);
	interval1	+= interval_item_t<uint32_t>(30,40);
	// check the basic interval_t
	NUNIT_ASSERT( interval1.size() == 2 );
	NUNIT_ASSERT( interval1[0] == interval_item_t<uint32_t>(10,20) );
	NUNIT_ASSERT( interval1[1] == interval_item_t<uint32_t>(30,40) );
	
// remove a single element in the middle
	interval2	= interval1 - interval_item_t<uint32_t>(15);
	// check the result
	NUNIT_ASSERT( interval2.size() == 3 );
	NUNIT_ASSERT( interval2[0] == interval_item_t<uint32_t>(10,14) );
	NUNIT_ASSERT( interval2[1] == interval_item_t<uint32_t>(16,20) );
	NUNIT_ASSERT( interval2[2] == interval_item_t<uint32_t>(30,40) );

// remove the whole interval
	interval2	= interval1 - interval_item_t<uint32_t>( 0,50);
	// check the basic interval_t
	NUNIT_ASSERT( interval2.size() == 0 );

// remove the last element
	interval2	= interval1 - interval_item_t<uint32_t>(20);
	// check the result
	NUNIT_ASSERT( interval2.size() == 2 );
	NUNIT_ASSERT( interval2[0] == interval_item_t<uint32_t>(10,19) );
	NUNIT_ASSERT( interval2[1] == interval_item_t<uint32_t>(30,40) );
	
// remove the middle item overlapping the first and the last
	interval2	= interval1 - interval_item_t<uint32_t>(15,35);
	// check the result
	NUNIT_ASSERT( interval2.size() == 2 );
	NUNIT_ASSERT( interval2[0] == interval_item_t<uint32_t>(10,14) );
	NUNIT_ASSERT( interval2[1] == interval_item_t<uint32_t>(36,40) );

	// return no error
	return NUNIT_RES_OK;
}

/** \brief test the interval_t serialization consistency
 */
nunit_res_t	interval_testclass_t::interval_serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	interval_t<uint32_t>	interval_toserial;
	interval_t<uint32_t>	interval_unserial;
	serial_t		serial;
	// set interval_toserial
	interval_toserial	+= interval_item_t<uint32_t>(10, 20);
	interval_toserial	+= interval_item_t<uint32_t>(30, 40);
	interval_toserial	+= interval_item_t<uint32_t>(50, 60);
	// do the serial/unserial
	serial << interval_toserial;
	serial >> interval_unserial;
	// test the serialization consistency
	NUNIT_ASSERT( interval_toserial == interval_unserial );	
	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

