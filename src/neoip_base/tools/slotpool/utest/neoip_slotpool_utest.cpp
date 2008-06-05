/*! \file
    \brief Definition of the unit test for the \ref slotpool_t and co

*/

/* system include */
/* local include */
#include "neoip_slotpool_utest.hpp"
#include "neoip_slotpool.hpp"
#include "neoip_log.hpp"
#include "neoip_namespace.hpp"

using namespace neoip;

/** \brief unit test for the slotpool_t class
 */
int neoip_slotpool_utest()
{
	int		n_error = 0;
	slotpool_t	slotpool;
	char *		str1	= "Hello";
	char *		str2	= "new";
	char *		str3	= "world!";
	
	// populate the slotpool_t
	slot_id_t	slot1	= slotpool.allocate(str1);
	slot_id_t	slot2	= slotpool.allocate(str2);
	slot_id_t	slot3	= slotpool.allocate(str3);

	// test if exist() return proper value
	if( !slotpool.exist(slot1) || !slotpool.exist(slot2) || !slotpool.exist(slot3) ){
		KLOG_ERR("slotpool_t::exist() failed to return proper result!!! BUG!!!!");
		n_error++;
	}

	// test if find() return the proper pointer
	if( slotpool.find(slot1) != str1 || slotpool.find(slot2) != str2 || slotpool.find(slot3) != str3){
		KLOG_ERR("slotpool_t::find() failed to return the proper pointer!!! BUG!!!!");
		n_error++;
	}

	// test if exit() do return false and find() return NULL on empty slot
	for( slot_id_t i = slot3+1; i < 10; i++ ){
		if( slotpool.find(i) != NULL || slotpool.exist(i) ){
			KLOG_ERR("slotpool_t::find() or ::exist() return bad result on a empty slot!!! BUG!!!!");
			n_error++;
		}
	}

#if 0
	KLOG_ERR("slot1=" << slot1 << " for " << (char *)slotpool.find(slot1));
	KLOG_ERR("slot2=" << slot2 << " for " << (char *)slotpool.find(slot2));
	KLOG_ERR("slot3=" << slot3 << " for " << (char *)slotpool.find(slot3));
#endif
	
	if( n_error )	goto error;
	KLOG_ERR("slotpool_t unit test PASSED!!!");

	return n_error;
error:;	KLOG_ERR("slotpool_t unit test FAILED!!!");
	return 1;
}

