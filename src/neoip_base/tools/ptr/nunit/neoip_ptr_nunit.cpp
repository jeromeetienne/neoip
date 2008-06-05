/*! \file
    \brief Definition of the unit test for the \ref ptr_t
*/

/* system include */
/* local include */
#include "neoip_ptr_nunit.hpp"
#include "neoip_ptr_counted.hpp"
#include "neoip_ptr_cow.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a ptr_t
 */
nunit_res_t	ptr_testclass_t::ptr_counted(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");

	// create a ptr_counted_t
	ptr_counted_t<const std::string>	ptr1(nipmem_new std::string("hello world."));
	// run some check on ptr1
	NUNIT_ASSERT( ptr1.get_ptr() != NULL );
	NUNIT_ASSERT( ptr1.count() == 1 );
	NUNIT_ASSERT( *ptr1 == "hello world." );

	// copy the ptr1 into ptr2 INSIDE bracket to ctor/dtor ptr2
{
	ptr_counted_t<const std::string>	ptr2(ptr1);
	// run some check on ptr2
	NUNIT_ASSERT( ptr2.get_ptr() == ptr1.get_ptr() );
	NUNIT_ASSERT( ptr1.count() == 2 );
	NUNIT_ASSERT( ptr2.count() == 2 );
	NUNIT_ASSERT( *ptr2 == "hello world." );
}

	// now that ptr2 has been destructed, ptr1.count() MUST be 1 again	
	NUNIT_ASSERT( ptr1.count() == 1 );
	NUNIT_ASSERT( *ptr1 == "hello world." );

	// return no error
	return NUNIT_RES_OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                   
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a ptr_t
 */
nunit_res_t	ptr_testclass_t::ptr_cow(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// create a ptr_cow_t
	const ptr_cow_t<std::string>	ptr1(nipmem_new std::string("hello world."));
	// run some check on ptr1
	NUNIT_ASSERT( ptr1.get_ptr() != NULL );
	NUNIT_ASSERT( ptr1.count() == 1 );
	NUNIT_ASSERT( *ptr1 == "hello world." );
	
	// copy the ptr1 into const ptr2 INSIDE bracket to ctor/dtor
{
	const ptr_cow_t<std::string>	ptr2(ptr1);
	// run some check on ptr2
	NUNIT_ASSERT( ptr2.get_ptr() == ptr1.get_ptr() );
	NUNIT_ASSERT( *ptr2 == "hello world." );
	// as ptr2 is const, using it doesnt trigger the copy-on-write
	NUNIT_ASSERT( ptr1.count() == 2 );
	NUNIT_ASSERT( ptr2.count() == 2 );
}

	// copy the ptr1 into ptr3 INSIDE bracket to ctor/dtor
{
	ptr_cow_t<std::string>	ptr3(ptr1);
	// at this point, ptr3 is still shared with ptr1
	NUNIT_ASSERT( ptr1.count() == 2 );
	NUNIT_ASSERT( ptr3.count() == 2 );
	// this trigger the usage of ptr3, as it is non const, it trigger the copy on write too
	NUNIT_ASSERT( *ptr3 == "hello world." );
	NUNIT_ASSERT( ptr1.count() == 1 );
	NUNIT_ASSERT( ptr3.count() == 1 );
}
	
	// return no error
	return NUNIT_RES_OK;
}
NEOIP_NAMESPACE_END

