/*! \file
    \brief Definition of the unit test for the \ref pktcomp_t

\par Brief description
This module performs an unit test for the pktcomp_t class

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_pktcomp_utest.hpp"
#include "neoip_pktcomp.hpp"
#include "neoip_pktcomp_pkttype.hpp"
#include "neoip_log.hpp"

using namespace neoip;

/** \brief unit test for the pktcomp_t class
 */
int neoip_pktcomp_utest()
{
	int			n_error		= 0;

	pktcomp_err_t		pktcomp_err	= pktcomp_err_t::OK;
	KLOG_ERR("err=" << pktcomp_err );
	pktcomp_pkttype_t	pkttype		= pktcomp_pkttype_t::COMPRESSED_PKT;
	KLOG_ERR("pkttype="<< pkttype );
	
	pktcomp_t	pktcomp;
	pktcomp_t	pktcomp2("bzip");
	
	if( n_error )	goto error;
	KLOG_ERR("PKTCOMP_UTEST PASSED");
	return 0;
error:;
	KLOG_ERR("PKTCOMP_UTEST FAILED!!!!");
	return -1;
}


