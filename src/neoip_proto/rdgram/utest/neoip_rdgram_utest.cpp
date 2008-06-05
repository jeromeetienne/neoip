/*! \file
    \brief Definition of the unit test for the \ref socket_itor_t, \ref socket_resp_t and co

\par Brief description
This module performs an unit test for the socket classes

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_rdgram_utest.hpp"
#include "neoip_log.hpp"
#include "neoip_rdgram_pkttype.hpp"
#include "neoip_rdgram_err.hpp"

using namespace neoip;

/** \brief unit test for the neoip_socket class
 */
int neoip_rdgram_utest()
{
	int			n_error		= 0;
	rdgram_pkttype_t	pkttype		= rdgram_pkttype_t::DATA_PKT;
	KLOG_ERR("balabla=" << pkttype );
	
	rdgram_err_t	err = rdgram_err_t(rdgram_err_t::OK, "ALL IT FINE");
	
	KLOG_ERR("err=" << err );
	
	
	if( n_error )	goto error;
	KLOG_ERR("RDGRAM_UTEST PASSED");
	return 0;
error:;
	KLOG_ERR("RDGRAM_UTEST FAILED!!!!");
	return -1;
}


