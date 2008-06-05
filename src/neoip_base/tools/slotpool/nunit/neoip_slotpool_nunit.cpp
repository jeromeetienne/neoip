/*! \file
    \brief Definition of the unit test for the \ref slotpool_t

*/

/* system include */
/* local include */
#include "neoip_slotpool_nunit.hpp"
#include "neoip_slotpool.hpp"
#include "neoip_rand.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief general test
 */
nunit_res_t	slotpool_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	slotpool_t	slotpool;
	// log to debug
	KLOG_DBG("enter");
	
	// populate the slotpool
	slot_id_t	slotid1	= slotpool.allocate((void *)1);
	slot_id_t	slotid2	= slotpool.allocate((void *)2);
	slot_id_t	slotid3	= slotpool.allocate((void *)3);
	
	// test if the allocated slot_id_t exists in the slotpool_t
	NUNIT_ASSERT( slotpool.exist(slotid1) );
	NUNIT_ASSERT( slotpool.exist(slotid2) );
	NUNIT_ASSERT( slotpool.exist(slotid3) );

	// test if the allocated slot_id_t can be found
	NUNIT_ASSERT( slotpool.find(slotid1) == (void *) 1);
	NUNIT_ASSERT( slotpool.find(slotid2) == (void *) 2);
	NUNIT_ASSERT( slotpool.find(slotid3) == (void *) 3);
	
	// test if non allocated slot_id_t are really unfound
	for( size_t i = 0; i < 50; i++ ){
		// get a random slot_id_t
		slot_id_t rand_slotid = (slot_id_t)neoip_rand(std::numeric_limits<slot_id_t>::max() - 1);
		// if it is an allocated one, skip it
		if( rand_slotid == slotid1 )	continue;
		if( rand_slotid == slotid2 )	continue;
		if( rand_slotid == slotid3 )	continue;
		// test that this rand_slotid is not in the slotpool
		NUNIT_ASSERT( !slotpool.exist(rand_slotid) );
	}
	
	// return no error
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

