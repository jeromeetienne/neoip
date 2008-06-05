/*! \file
    \brief unit test for the object_slotid_t

*/

/* system include */
/* local include */
#include "neoip_object_slotid_nunit.hpp"
#include "neoip_object_slotid.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;


class object_slotid_example_t : public object_slotid_t { };

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test the normal usage 
 */
nunit_res_t	object_slotid_testclass_t::using_tokeep(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// create an object
	object_slotid_t *	object1	= nipmem_new object_slotid_t();
	// get its slotid
	slot_id_t		slotid1	= object1->get_object_slotid();
	// test it still exists
	NUNIT_ASSERT( object_slotid_exist(slotid1) );
	// delete the object
	nipmem_delete object1;
	// test it nomore exists
	NUNIT_ASSERT( !object_slotid_exist(slotid1) );
	// return no error
	return NUNIT_RES_OK;
}

/** \brief test the copy operator
 */
nunit_res_t	object_slotid_testclass_t::copy_operator(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// create the object1
	object_slotid_t object1;
	// get its slotid
	slot_id_t	slotid1 = object1.get_object_slotid();
	// copy the object2
	object_slotid_t object2(object1);
	// get its slotid
	slot_id_t	slotid2 = object2.get_object_slotid();

	// check both slotid are different
	NUNIT_ASSERT( slotid1 != slotid2 );

	// return no error
	return NUNIT_RES_OK;
}

/** \brief test the operator =
 */
nunit_res_t	object_slotid_testclass_t::normal_assignement(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// create object1
	object_slotid_t object1;
	// get its slotid
	slot_id_t	slotid1 = object1.get_object_slotid();
	// assign object2 with object1
	object_slotid_t	object2	= object1;
	// get its slotid
	slot_id_t	slotid2 = object2.get_object_slotid();

	// check both slotid are different
	NUNIT_ASSERT( slotid1 != slotid2 );

	// return no error
	return NUNIT_RES_OK;
}

/** \brief test the operator =
 */
nunit_res_t	object_slotid_testclass_t::self_assignement(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// create object1
	object_slotid_t object1;
	// get its slotid
	slot_id_t	slotid1 = object1.get_object_slotid();

	// do the self assignement
	object1	= object1;

	// check both slotid are different
	NUNIT_ASSERT( slotid1 == object1.get_object_slotid() );

	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END
