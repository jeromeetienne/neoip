/*! \file
    \brief Definition of the unit test for the \ref socket_itor_t, \ref socket_resp_t and co

\par Brief description
This module performs an unit test for the socket classes

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_reachpeer_utest.hpp"
#include "neoip_reachpeer.hpp"
#include "neoip_log.hpp"

#include "neoip_timer_probing.hpp"

using namespace neoip;

/** \brief unit test for the neoip_socket class
 */
int neoip_reachpeer_utest()
{
	int		n_error		= 0;
	reachpeer_t	reachpeer;

	if( n_error )	goto error;
	KLOG_ERR("REACHPEER_UTEST PASSED");
	return 0;
error:;
	KLOG_ERR("REACHPEER_UTEST FAILED!!!!");
	return -1;
}


