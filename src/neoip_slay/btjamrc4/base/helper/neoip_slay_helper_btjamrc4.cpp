/*! \file
    \brief Definition of static helper functions for the tls layer

*/

/* system include */
/* local include */
#include "neoip_slay_helper_btjamrc4.hpp"
#include "neoip_slay_full_btjamrc4.hpp"
#include "neoip_slay_full.hpp"
#include "neoip_slay_err.hpp"
#include "neoip_slay_domain.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

// list of include for the insertion in factory_plant_t
#include "neoip_slay_profile_btjamrc4.hpp"
#include "neoip_slay_resp_btjamrc4.hpp"
#include "neoip_slay_itor_btjamrc4.hpp"
#include "neoip_slay_full_btjamrc4.hpp"

NEOIP_NAMESPACE_BEGIN;

// definition of \ref slay_helper_btjamrc4_t constant
const slay_domain_t	slay_helper_btjamrc4_t::DOMAIN_VAR	= strtype_slay_domain_t::BTJAMRC4;
// end of constants definition

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return a slay_full_t from a btjamrc4_full_t
 */
slay_full_t *	slay_helper_btjamrc4_t::build_slay_full(bt_jamrc4_full_t *p_btjamrc4_full)	throw()
{
	slay_err_t	slay_err;
	// sanity check - p_btjamrc4_full MUST NOT be null
	DBG_ASSERT( p_btjamrc4_full );
	
	// build the slay_full_btjamrc4_t
	slay_full_btjamrc4_t *	full_dom;
	full_dom	= nipmem_new slay_full_btjamrc4_t();
	slay_err	= full_dom->start(p_btjamrc4_full);
	if( slay_err.failed() ){
		KLOG_ERR("cant start slay_full_btjamrc4_t due to " << slay_err);
		nipmem_zdelete	full_dom;
		return NULL;
	}

	// build the slay_full
	slay_full_t *	slay_full;
	slay_full	= nipmem_new slay_full_t();
	slay_err	= slay_full->start(full_dom);
	if( slay_err.failed() ){
		KLOG_ERR("cant start slay_full_t due to " << slay_err);
		nipmem_zdelete	slay_full;
		return NULL;
	}
	
	// return the just-built slay_full_t
	return slay_full;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			factory_domain_insert
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Populate the slay_*_t factory with all the slay_domain_t::BTJAMRC4 class
 */
void	slay_helper_btjamrc4_t::factory_domain_insert()				throw()
{
	// insert all the factory_product_t for strtype_slay_domain_t::BTJAMRC4
	FACTORY_PRODUCT_INSERT(slay_profile_factory, slay_domain_t, slay_profile_vapi_t
				, strtype_slay_domain_t::BTJAMRC4, slay_profile_btjamrc4_t);
	FACTORY_PRODUCT_INSERT(slay_resp_factory, slay_domain_t, slay_resp_vapi_t
				, strtype_slay_domain_t::BTJAMRC4, slay_resp_btjamrc4_t);
	FACTORY_PRODUCT_INSERT(slay_itor_factory, slay_domain_t, slay_itor_vapi_t
				, strtype_slay_domain_t::BTJAMRC4, slay_itor_btjamrc4_t);
	FACTORY_PRODUCT_INSERT(slay_full_factory, slay_domain_t, slay_full_vapi_t
				, strtype_slay_domain_t::BTJAMRC4, slay_full_btjamrc4_t);
}

NEOIP_NAMESPACE_END;






