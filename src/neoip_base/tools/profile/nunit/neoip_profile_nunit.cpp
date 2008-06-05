/*! \file
    \brief Definition of the unit test for interface_t template

\par Notes
- coded to dump my brain on the profile thing
- not really usable right now
  - some stuff are missing:
    - compatilibie with current profile arch
    - the definition of forward declarable typedef for the read-only shared copy
- TODO
  - may do struct_field() to return a reference on the actual object with const and non const
  - and struct_field_ptr() to return a ptr_counted_t<struct_field_t> with const and non const
  
*/

/* system include */
/* local include */
#include "neoip_profile_nunit.hpp"
#include "neoip_profile_nunit_example.hpp"
#include "neoip_kad_rpc_profile.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     profile_item_t test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test the profile_item_t ctor
 */
nunit_res_t	profile_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_ERR("enter");

	profile_nunit_example_t	profile;
	
	profile.rpc()->replypkt_maxlen(4242);

	profile.rpc(kad_rpc_profile_t());
	
	
	ptr_counted_t<const kad_rpc_profile_t>	rpc_profile	= profile.rpc();
	KLOG_ERR("profile.rpc.count=" << profile.rpc().count() );
	KLOG_ERR("profile.rpc.get_ptr=" << profile.rpc().get_ptr() );
	KLOG_ERR("rpc_profile.count=" << rpc_profile.count() );
	KLOG_ERR("rpc_profile.rpc.get_ptr=" << rpc_profile.get_ptr() );
	KLOG_ERR("replypkt_maxlen=" << rpc_profile->replypkt_maxlen() );


	// return no error
	return NUNIT_RES_OK;
}




NEOIP_NAMESPACE_END

