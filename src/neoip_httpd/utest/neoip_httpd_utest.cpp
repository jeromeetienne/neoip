/*! \file
    \brief Definition of the unit test for the \ref neoip::httpd_t

\par Brief description
This module performs an unit test for the neoip::httpd_t classes

*/

/* system include */
/* local include */
#include "neoip_httpd_utest.hpp"
#include "neoip_httpd.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

#include "neoip_httpd_err.hpp"

using namespace neoip;

/** \brief unit test for the neoip::httpd_t class
 */
int neoip_httpd_utest()
{
	int		n_error = 0;
	std::stringstream	oss1;
	std::stringstream	oss2;
	
	oss1 << oss2.str();

	httpd_err_t	err = httpd_err_t::NOT_FOUND;
	
	KLOG_ERR("error = " << err );

	httpd_t	*	httpd	= nipmem_new httpd_t();
	bool		failed	= httpd->start("127.0.0.1:1234");
	if( failed )	goto error;

	if( n_error )	goto error;
	KLOG_ERR("HTTPD_UTEST PASSED");
	return 0;
error:;
	KLOG_ERR("HTTPD_UTEST FAILED!!!!");
	return -1;
}

