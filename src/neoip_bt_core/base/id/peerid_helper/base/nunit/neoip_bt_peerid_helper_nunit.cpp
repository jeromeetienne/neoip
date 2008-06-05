/*! \file
    \brief Definition of the unit test for the \ref bt_peerid_helper_t

*/

/* system include */
#include <string>
/* local include */
#include "neoip_bt_peerid_helper_nunit.hpp"
#include "neoip_bt_peerid_helper.hpp"
#include "neoip_bt_id.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bt_peerid_helper_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
#if 0
	KLOG_ERR("azureus peerid=" << bt_peerid_helper_t::build("azureus", "1.2.3.4").to_datum());
	KLOG_ERR("shadow peerid=" << bt_peerid_helper_t::build("shadow", "5.8.7").to_datum());
	KLOG_ERR("mainline peerid=" << bt_peerid_helper_t::build("mainline", "5.3.1").to_datum());
	KLOG_ERR("xbt peerid=" << bt_peerid_helper_t::build("xbt", "5.3.3").to_datum());
	KLOG_ERR("mldonkey peerid=" << bt_peerid_helper_t::build("mldonkey", "2.7.2").to_datum());
	KLOG_ERR("bitonwheels peerid=" << bt_peerid_helper_t::build("bitonwheels", "1.2").to_datum());

	bt_id_t	azureus_peerid	= bt_peerid_helper_t::build("ktorrent", "1.2.3.4");
	KLOG_ERR("azureus peerid="	<< azureus_peerid.to_datum());
	KLOG_ERR("style guess peerid="	<< bt_peerid_helper_t::parse_style(azureus_peerid));
	KLOG_ERR("progname guess peerid="<< bt_peerid_helper_t::parse_progname(azureus_peerid));
	KLOG_ERR("progvers guess peerid="<< bt_peerid_helper_t::parse_progvers(azureus_peerid));

	bt_id_t	shadow_peerid	= bt_peerid_helper_t::build("shadow", "1.2.4");
	KLOG_ERR("shadow peerid="	<< shadow_peerid.to_datum());
	KLOG_ERR("style guess peerid="	<< bt_peerid_helper_t::parse_style(shadow_peerid));
	KLOG_ERR("progname guess peerid="<< bt_peerid_helper_t::parse_progname(shadow_peerid));
	KLOG_ERR("progvers guess peerid="<< bt_peerid_helper_t::parse_progvers(shadow_peerid));

	bt_id_t	mainline_peerid	= bt_peerid_helper_t::build("mainline", "2.3.4");
	KLOG_ERR("mainline peerid="	<< mainline_peerid.to_datum());
	KLOG_ERR("style guess peerid="	<< bt_peerid_helper_t::parse_style(mainline_peerid));
	KLOG_ERR("progname guess peerid="<< bt_peerid_helper_t::parse_progname(mainline_peerid));
	KLOG_ERR("progvers guess peerid="<< bt_peerid_helper_t::parse_progvers(mainline_peerid));

	bt_id_t	mldonkey_peerid	= bt_peerid_helper_t::build("mldonkey", "2.3.4");
	KLOG_ERR("mldonkey peerid="	<< mldonkey_peerid.to_datum());
	KLOG_ERR("style guess peerid="	<< bt_peerid_helper_t::parse_style(mldonkey_peerid));
	KLOG_ERR("progname guess peerid="<< bt_peerid_helper_t::parse_progname(mldonkey_peerid));
	KLOG_ERR("progvers guess peerid="<< bt_peerid_helper_t::parse_progvers(mldonkey_peerid));
	
	bt_id_t	xbt_peerid	= bt_peerid_helper_t::build("xbt", "2.3.4");
	KLOG_ERR("xbt peerid="	<< xbt_peerid.to_datum());
	KLOG_ERR("style guess peerid="	<< bt_peerid_helper_t::parse_style(xbt_peerid));
	KLOG_ERR("progname guess peerid="<< bt_peerid_helper_t::parse_progname(xbt_peerid));
	KLOG_ERR("progvers guess peerid="<< bt_peerid_helper_t::parse_progvers(xbt_peerid));

	
	bt_id_t	bitonwheels_peerid	= bt_peerid_helper_t::build("bitonwheels", "2.3");
	KLOG_ERR("bitonwheels peerid="	<< bitonwheels_peerid.to_datum());
	KLOG_ERR("style guess peerid="	<< bt_peerid_helper_t::parse_style(bitonwheels_peerid));
	KLOG_ERR("progname guess peerid="<< bt_peerid_helper_t::parse_progname(bitonwheels_peerid));
	KLOG_ERR("progvers guess peerid="<< bt_peerid_helper_t::parse_progvers(bitonwheels_peerid));
#endif

	NUNIT_ASSERT( is_buildparse_consistent("azureus"	, "1.2.3.4"	) );
	NUNIT_ASSERT( is_buildparse_consistent("ktorrent"	, "1.2.3.4"	) );
	NUNIT_ASSERT( is_buildparse_consistent("shadow"		, "1.2.3"	) );
	NUNIT_ASSERT( is_buildparse_consistent("mainline"	, "5.3.1"	) );
	NUNIT_ASSERT( is_buildparse_consistent("xbt"		, "5.3.3"	) );
	NUNIT_ASSERT( is_buildparse_consistent("opera"		, "2.7.1.0"	) );
	NUNIT_ASSERT( is_buildparse_consistent("mldonkey"	, "2.7.1"	) );
	NUNIT_ASSERT( is_buildparse_consistent("bitonwheels"	, "1.2"		) );

	// return no error
	return NUNIT_RES_OK;
}


/** \brief Return true if the building and parsing of the peerid for progname/progvers is consistent
 */
bool	bt_peerid_helper_testclass_t::is_buildparse_consistent(const std::string &progname
						, const std::string &progvers)	throw()
{
	// build the peerid bt_id_t from the progname/progvers
	bt_id_t	peerid	= bt_peerid_helper_t::build(progname, progvers);
	// if the building failed, return false
	if( peerid.is_null() )								return false;
	// if it is impossible to parse the same progname from the peerid, return false
	if( string_t::casecmp(progname, bt_peerid_helper_t::parse_progname(peerid)))	return false;
	// if it is impossible to parse the same progvers from the peerid, return false
	if( string_t::casecmp(progvers, bt_peerid_helper_t::parse_progvers(peerid)))	return false;
	// if all previous tests passed, return true
	return true;
}



NEOIP_NAMESPACE_END

