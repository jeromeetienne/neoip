/*! \file
    \brief Definition of the unit test for the \ref neoip_negorange and co

\par Brief description
This module performs an unit test for the neoip_negorange classes

*/

/* system include */
/* local include */
#include "neoip_negorange_utest.hpp"
#include "neoip_negorange.hpp"
#include "neoip_delay.hpp"
#include "neoip_log.hpp"

using namespace neoip;

/** \brief unit test for the neoip_negorange class
 */
int neoip_negorange_utest()
{
	int				n_error = 0;
	negorange_t<delay_t>		negorange1;
	negorange_t<delay_t>		negorange2;
	negorange_t<delay_t>		delay_range_common;
	
	negorange1 = negorange_t<delay_t>(delay_t::from_sec(10), delay_t::from_sec(40)
							, delay_t::from_sec(20));
	negorange2 = negorange_t<delay_t>(delay_t::from_sec(10), delay_t::from_sec(60)
							, delay_t::from_sec(55));
	KLOG_ERR("opt1=" << negorange1);
	KLOG_ERR("opt2=" << negorange2);
	
	delay_range_common = negorange1.get_common_remote_wish(negorange2);
	KLOG_ERR("common=" << delay_range_common);

	if( n_error )	KLOG_ERR("neoip_negorange unit test FAILED!!!");
	else		KLOG_ERR("neoip_negorange unit test PASSED!!!");
	return n_error;
}

