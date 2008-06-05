/*! \file
    \brief Definition of the unit test for the \ref asyncexe_t

\par Brief description
This module performs an unit test for the asyncexe classes

*/

/* system include */
#include <iostream>
/* local include */
#include "neoip_asyncexe_utest.hpp"
#include "neoip_asyncexe.hpp"
#include "neoip_timeout.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

using namespace neoip;


NEOIP_NAMESPACE_BEGIN;

class asyncexe_utest_t : private asyncexe_cb_t {
private:
	asyncexe_t *	asyncexe;
	
	/** \brief callback notified by asyncexe_t when the result is known
	 */
	void neoip_asyncexe_cb(void *cb_userptr, asyncexe_t &asyncexe, const std::string &launch_error
					, const std::string &std_output, const std::string &std_error
					, const int &exit_status)		throw()
	{
		// display the result
		KLOG_ERR("asyncexe returned std_output=" << std_output << " std_error=" << std_error
							<< " exit_status=" << exit_status
							<< " launch_error=" << launch_error);
	}
public:
	/** \brief Constructor
	 */
	asyncexe_utest_t()		throw()
	{
		// start the asyncexe_t
		asyncexe	= nipmem_new asyncexe_t();
		bool	failed = asyncexe->start("bin_aux/neoip_get_public_ipaddr.py", this, NULL);
		DBG_ASSERT( !failed );
	}
	/** \brief Destructor
	 */
	~asyncexe_utest_t()		throw()
	{
		// delete the asyncexe_t
		nipmem_delete asyncexe;
	}
};

NEOIP_NAMESPACE_END

/** \brief unit test for the neoip_asyncexe class
 */
int neoip_asyncexe_utest()
{
	int		n_error		= 0;


	if( n_error )	goto error;
	KLOG_ERR("asyncexe_t utest PASSED");
	return 0;
	
error:;	KLOG_ERR("asyncexe_t utest FAILED!!!!");
	return -1;
}


asyncexe_utest_t *	asyncexe_utest	= NULL;

/** \brief unit test for the asyncexe_t class
 */
int neoip_asyncexe_utest_start()
{
	KLOG_ERR("enter");
	asyncexe_utest	= nipmem_new asyncexe_utest_t();
	return 0;
}

int neoip_asyncexe_utest_end()
{
	KLOG_ERR("enter");
	nipmem_delete	asyncexe_utest;
	return 0;
}

