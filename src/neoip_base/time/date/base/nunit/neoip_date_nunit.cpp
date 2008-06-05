/*! \file
    \brief Definition of the unit test for the \ref date_t

*/

/* system include */
/* local include */
#include "neoip_date_nunit.hpp"
#include "neoip_date.hpp"
#include "neoip_date_helper.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief function to test a date_t
 */
nunit_res_t	date_testclass_t::canonical_string(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// log to debug
	KLOG_DBG("enter");
	// get the date for the 'present'
	date_t		date1	= date_t::present();
	// convert it to a canonical_string
	std::string	abs_str	= date_helper_t::to_canonical_string(date1);
	// convert it back to a date_t
	date_t		date2	= date_helper_t::from_canonical_string(abs_str);
	// compute the difference (it is not equal because the string doesnt contains the msec)
	delay_t		delta	= (date1 > date2 ? (date1 - date2) : (date2 - date1));
	// log to debug
	KLOG_ERR("blabla=[" << abs_str	<< "]");
	KLOG_ERR("blabla=[" << date2	<< "]");
	KLOG_ERR("blabla=[" << delta	<< "]");
	// the difference MUST NOT be more than 1sec 
	NUNIT_ASSERT( delta < delay_t::from_sec(1) );
	
	// return no error	
	return NUNIT_RES_OK;
}


NEOIP_NAMESPACE_END

