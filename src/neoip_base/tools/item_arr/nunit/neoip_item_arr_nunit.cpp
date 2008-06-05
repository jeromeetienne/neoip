/*! \file
    \brief Definition of the unit test for the \ref item_arr_t
*/

/* system include */
/* local include */
#include "neoip_item_arr_nunit.hpp"
#include "neoip_item_arr.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief function to test a item_arr_t
 */
nunit_res_t	item_arr_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// create the object
	item_arr_t<uint32_t>	item_arr;
	// check the is_null()
	NUNIT_ASSERT( item_arr.is_null() );
	// check the size/empty are the one of a empty object
	NUNIT_ASSERT( item_arr.size() == 0 );
	NUNIT_ASSERT( item_arr.empty() );
	// add one item with .append()
	item_arr.append(42);
	// add one item with +=
	item_arr += 99;
	// check the size/empty are the one of a non empty object
	NUNIT_ASSERT( item_arr.size() == 2 );
	NUNIT_ASSERT( !item_arr.empty() );
	// check the is_null()
	NUNIT_ASSERT( !item_arr.is_null() );
	// check the values themselves
	NUNIT_ASSERT( item_arr[0] == 42 );
	NUNIT_ASSERT( item_arr[1] == 99 );
	
	// build a second item_arr
	item_arr_t<uint32_t>	item_arr2;
	item_arr2.append(142).append(199);
	// append it to the first item_arr_t with +=
	item_arr += item_arr2;
	// check the values themselves
	NUNIT_ASSERT( item_arr[0] == 42 );
	NUNIT_ASSERT( item_arr[1] == 99 );
	NUNIT_ASSERT( item_arr[2] == 142 );
	NUNIT_ASSERT( item_arr[3] == 199 );
	NUNIT_ASSERT( item_arr.size() == 4 );

	// return no error
	return NUNIT_RES_OK;
}


/** \brief function to test a item_arr_t
 */
nunit_res_t	item_arr_testclass_t::remove(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// create the object
	item_arr_t<uint32_t>	item_arr;
	// populate the item_arr
	item_arr += 42;
	item_arr += 99;
	// remove the item of index 1
	item_arr.remove(1);
	// check the size/empty are the one of a non empty object
	NUNIT_ASSERT( item_arr.size() == 1 );
	// check the values themselves
	NUNIT_ASSERT( item_arr[0] == 42 );
	// return no error
	return NUNIT_RES_OK;
}


/** \brief function to test a item_arr_t
 */
nunit_res_t	item_arr_testclass_t::search_function(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// create the object
	item_arr_t<uint32_t>	item_arr1;
	// populate item_arr1
	item_arr1	+= 42;
	item_arr1	+= 99;
	
	// test the item_arr_t::contain
	NUNIT_ASSERT( item_arr1.contain(42)	== true );
	NUNIT_ASSERT( item_arr1.contain(99)	== true );
	NUNIT_ASSERT( item_arr1.contain(142)	== false );

	// populate item_arr2
	item_arr_t<uint32_t>	item_arr2;
	item_arr2	+= 11;
	item_arr2	+= 142;
	
	// test the item_arr_t::find_first_common with no match
	NUNIT_ASSERT( item_arr1.find_first_common(item_arr2) == NULL );
	
	// add a common item
	item_arr2	+= 42;

	// test the item_arr_t::find_first_common with match
	NUNIT_ASSERT( item_arr1.find_first_common(item_arr2) );
	NUNIT_ASSERT( *item_arr1.find_first_common(item_arr2) == 42 );
	
	// return no error
	return NUNIT_RES_OK;
}

/** \brief test serial_t consistency of item_arr_t
 */
nunit_res_t	item_arr_testclass_t::serial_consistency(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	item_arr_t<uint32_t>	item_arr_toserial;
	item_arr_t<uint32_t>	item_arr_unserial;
	serial_t		serial;
	// set item_arr_toserial
	item_arr_toserial.append(42).append(99);
	// do the serial/unserial
	serial << item_arr_toserial;
	serial >> item_arr_unserial;
	// test the serialization consistency
	NUNIT_ASSERT( item_arr_toserial == item_arr_unserial );

	// return no error
	return NUNIT_RES_OK;
}





NEOIP_NAMESPACE_END

