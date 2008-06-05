/*! \file
    \brief unit test for the bfilter_t

*/

/* system include */
/* local include */
#include "neoip_bfilter_nunit.hpp"
#include "neoip_bfilter.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test to insert elements in the bloom filter and retrieve them
 */
nunit_res_t	bfilter_testclass_t::insert(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// build bfilter1
	bfilter_t	bfilter1(bfilter_param_t().filter_width(512).nb_hash(5));
	bfilter1.insert(datum_t("alice"));
	bfilter1.insert(datum_t("bob"));
	bfilter1.insert(datum_t("change"));
	// test that some elements are present
	NUNIT_ASSERT( bfilter1.contain(datum_t("alice"))	== true);
	NUNIT_ASSERT( bfilter1.contain(datum_t("bob"))		== true);
	NUNIT_ASSERT( bfilter1.contain(datum_t("change"))	== true);
	// test that some elements are not present
	NUNIT_ASSERT( bfilter1.contain(datum_t("grab"))		== false);
	NUNIT_ASSERT( bfilter1.contain(datum_t("grop"))		== false);
	NUNIT_ASSERT( bfilter1.contain(datum_t("zlot"))		== false);
	// return no error
	return NUNIT_RES_OK;
}

/** \brief test to unify 2 bloom filter
 */
nunit_res_t	bfilter_testclass_t::unify(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	// build bfilter1
	bfilter_t	bfilter1(bfilter_param_t().filter_width(512).nb_hash(5));
	bfilter1.insert(datum_t("alice"));
	bfilter1.insert(datum_t("bob"));
	bfilter1.insert(datum_t("change"));

	// build bfilter2
	bfilter_t	bfilter2(bfilter_param_t().filter_width(512).nb_hash(5));
	bfilter2.insert(datum_t("herbert"));
	bfilter2.insert(datum_t("slota"));
	bfilter2.insert(datum_t("beep"));

	// test unify
	bfilter_t	bfilter_unify	= bfilter1.unify(bfilter2);
	NUNIT_ASSERT( bfilter_unify.contain(datum_t("alice"))	== true);
	NUNIT_ASSERT( bfilter_unify.contain(datum_t("bob"))	== true);
	NUNIT_ASSERT( bfilter_unify.contain(datum_t("change"))	== true);
	NUNIT_ASSERT( bfilter_unify.contain(datum_t("herbert"))	== true);
	NUNIT_ASSERT( bfilter_unify.contain(datum_t("slota"))	== true);
	NUNIT_ASSERT( bfilter_unify.contain(datum_t("beep"))	== true);

	// return no error
	return NUNIT_RES_OK;
}

/** \brief test intersect 2 bloom filter
 */
nunit_res_t	bfilter_testclass_t::intersect(const nunit_testclass_ftor_t &testclass_ftor) throw()
{	
	// build bfilter1
	bfilter_t	bfilter1(bfilter_param_t().filter_width(512).nb_hash(5));
	bfilter1.insert(datum_t("alice"));
	bfilter1.insert(datum_t("bob"));
	bfilter1.insert(datum_t("change"));
	// build bfilter2
	bfilter_t	bfilter2(bfilter_param_t().filter_width(512).nb_hash(5));
	bfilter2.insert(datum_t("herbert"));
	bfilter2.insert(datum_t("alice"));
	bfilter2.insert(datum_t("bob"));
	// test intersect
	bfilter_t	bfilter_intersect	= bfilter1.intersect(bfilter2);
	NUNIT_ASSERT( bfilter_intersect.contain(datum_t("alice"))	== true);
	NUNIT_ASSERT( bfilter_intersect.contain(datum_t("bob"))		== true);
	NUNIT_ASSERT( bfilter_intersect.contain(datum_t("change"))	== false);
	NUNIT_ASSERT( bfilter_intersect.contain(datum_t("herbert"))	== false);

	// return no error
	return NUNIT_RES_OK;
}

NEOIP_NAMESPACE_END
