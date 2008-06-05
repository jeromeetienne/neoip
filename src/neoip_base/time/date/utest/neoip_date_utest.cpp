/*! \file
    \brief Definition of the unit test for the \ref date_t and co

\par Brief description
This module performs an unit test for the \ref date_t classes

*/

/* system include */
/* local include */
#include "neoip_date_utest.hpp"
#include "neoip_date.hpp"
#include "neoip_log.hpp"
#include "neoip_datum.hpp"

using namespace neoip;

/** \brief unit test for the neoip_date class
 */
int neoip_date_utest()
{
	int		n_error = 0;
	date_t		date	= date_t::present();
	delay_t	delay	= delay_t::from_sec(30);
	KLOG_ERR("date present=" << date );
	KLOG_ERR("date present + 30sec=" << date_t(date + delay) );

	if( n_error )	KLOG_ERR("neoip_date unit test FAILED!!!");
	else		KLOG_ERR("neoip_date unit test PASSED!!!");
	return n_error;
}

