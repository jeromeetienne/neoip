/*! \file
    \brief Definition of the unit test for the \ref cnxstat_t

\par Brief description
This module performs an unit test for the cnxstat_t classes

*/

/* system include */
/* local include */
#include "neoip_cnxstat_utest.hpp"
#include "neoip_cnxstat.hpp"
#include "neoip_log.hpp"
#include "neoip_datum.hpp"

using namespace neoip;

/** \brief unit test for the \ref cnxstat_t class
 */
int neoip_cnxstat_utest()
{
	int		n_error = 0;
	cnxstat_t	cnxstat;

	KLOG_ERR("before cnxstat=" << cnxstat);
	
	cnxstat.count_incoming(50);
	cnxstat.count_incoming(150);

	cnxstat.count_outgoing(200);
	cnxstat.count_outgoing(300);

	KLOG_ERR("after cnxstat=" << cnxstat);

	if( n_error )	goto error;
	KLOG_ERR("CNXSTAT_UTEST PASSED");
	return 0;
error:;
	KLOG_ERR("CNXSTAT_UTEST FAILED!!!!");
	return -1;
}

