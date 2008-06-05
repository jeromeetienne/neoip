/*! \file
    \brief unit test for the tokeep_check_t

*/

/* system include */
/* local include */
#include "neoip_tokeep_check_nunit.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;


/** \brief the class which is notifying
 */
class tokeep_check_notifying_t {
private:
	TOKEEP_CHECK_DECL_DFL();
public:
	void	do_test()	throw()
	{
		// backup the necessary info for the tokeep_check
		TOKEEP_CHECK_BACKUP_DFL(*this);
		// test the returned tokeep
		TOKEEP_CHECK_MATCH_DFL(true);
		// test the returnd dontkeep
		nipmem_delete this;
		TOKEEP_CHECK_MATCH_DFL(false);
	}
};

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief general test
 */
nunit_res_t	tokeep_check_testclass_t::using_internal(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	slot_id_t		slot_id;
	tokeep_check_t *	test;
	
	// create the object
	test	= nipmem_new tokeep_check_t();
	// backup its slotid
	slot_id	= test->get_slot_id();
	// check that it DOES exists in the global database
	NUNIT_ASSERT(tokeep_check_slotpool->exist(slot_id) == true);
	// delete the object
	nipmem_delete	test;
	// check that it NOMORE exists in the global database
	NUNIT_ASSERT(tokeep_check_slotpool->exist(slot_id) == false);

	// return no error
	return NUNIT_RES_OK;
}


/** \brief general test
 */
nunit_res_t	tokeep_check_testclass_t::using_define(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// create the object
	tokeep_check_notifying_t * notifying	= nipmem_new tokeep_check_notifying_t();
	// do the test
	notifying->do_test();
	// NOTE: no nipmem_delete because the class autodelete itself
	
	// return no error
	return NUNIT_RES_OK;
}

/** \brief test the copy operator
 */
nunit_res_t	tokeep_check_testclass_t::copy_operator(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// create the source
	tokeep_check_t *	tokeep_check1	= nipmem_new tokeep_check_t();
	// copy the object
	tokeep_check_t *	tokeep_check2	= nipmem_new tokeep_check_t(*tokeep_check1);
	// delete the source of the copy BEFORE the destination
	// - so if tokeep_check is not properly copied (== just copying slot_id), instead
	//   of reassigning a complete new object, it will assert during the delete
	nipmem_delete tokeep_check1;
	nipmem_delete tokeep_check2;
	// return no error
	return NUNIT_RES_OK;
}

/** \brief test the operator =
 */
nunit_res_t	tokeep_check_testclass_t::assignement(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	tokeep_check_t*	tokeep_check1	= nipmem_new tokeep_check_t();
	// copy the object
	tokeep_check_t	tokeep_check2	= *tokeep_check1;
	// delete the source of the copy BEFORE the destination
	// - so if tokeep_check is not properly copied (== just copying slot_id), instead
	//   of reassigning a complete new object, it will assert during the destruction
	//   of tokeep_check2, so at the end of this function
	nipmem_delete tokeep_check1;
	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END
