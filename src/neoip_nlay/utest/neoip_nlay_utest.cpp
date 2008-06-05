/*! \file
    \brief Definition of the unit test for the \ref nlay_t

\par Brief description
This module performs an unit test for the nlay_t class

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_nlay_utest.hpp"
#include "neoip_log.hpp"

#include "neoip_nlay_resp.hpp"
using namespace neoip;

/** \brief unit test for the nlay_t class
 */
int neoip_nlay_utest()
{
	int			n_error		= 0;

	nlay_resp_t	nlay_resp;
	nlay_resp_t	nlay_itor;

	if( n_error )	goto error;
	KLOG_ERR("NLAY_UTEST PASSED");
	return 0;
error:;
	KLOG_ERR("NLAY_UTEST FAILED!!!!");
	return -1;
}


