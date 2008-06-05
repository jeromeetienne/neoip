/*! \file
    \brief Definition of the unit test for the \ref tcp_t

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_tcp_utest.hpp"
#include "neoip_tcp.hpp"
#include "neoip_log.hpp"

using namespace neoip;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                             MAIN UTEST
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief unit test for the neoip_socket class
 */
int neoip_tcp_utest()
{
	int			n_error		= 0;
	tcp_resp_t		tcp_resp;
	inet_err_t		inet_err;
	inet_err = tcp_resp.start("127.0.0.1:4000", NULL, NULL);
	DBG_ASSERT( inet_err.succeed() );

// TODO
// - do a class which respond to tcp connection and display the arriving packet
// - do another class which initiate tcp connection and send periodically packet
	
	if( n_error )	goto error;
	KLOG_ERR("INET_ADDR_UTEST PASSED");
	return 0;
error:;
	KLOG_ERR("INET_ADDR_UTEST FAILED!!!!");
	return -1;
}



