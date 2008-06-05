/*! \file
    \brief Definition of the unit test for the \ref ipcountry_t

*/

/* system include */
/* local include */
#include "neoip_ipcountry_nunit.hpp"
#include "neoip_ipcountry.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ipcountry_testclass_t::ipcountry_testclass_t()	throw()
{
	// zero some field
	ipcountry	= NULL;
}

/** \brief Destructor
 */
ipcountry_testclass_t::~ipcountry_testclass_t()	throw()
{
	// deinit the testclass just in case
	neoip_nunit_testclass_deinit();
}
	
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit init/deinit
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Init the testclass implementation
 */
nunit_err_t	ipcountry_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// sanity check
	DBG_ASSERT( !ipcountry );
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	ipcountry_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// destruct the ipcountry_t
	nipmem_zdelete ipcountry;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	ipcountry_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	inet_err_t	inet_err;
	// log to debug
	KLOG_DBG("enter");

	// start the ipcountry_t
	ipcountry	= nipmem_new ipcountry_t();
	inet_err	= ipcountry->start(ip_addr_t("62.70.27.118"), this, NULL);
	NUNIT_ASSERT( inet_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     ipcountry_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ipcountry_t to provide event
 */
bool	ipcountry_testclass_t::neoip_ipcountry_cb(void *cb_userptr, ipcountry_t &cb_ipcountry
				, const inet_err_t &inet_err, const std::string &country_code)	throw() 
{
	// log to debug
	KLOG_ERR("enter inet_err=" << inet_err << " country_code=" << country_code);

	// delete the ipcountry_t
	nipmem_zdelete	ipcountry;

	// report the result to nunit depending on the notified inet_err_t
	if( inet_err.succeed() )	nunit_ftor( NUNIT_RES_OK	);
	else				nunit_ftor( NUNIT_RES_ERROR	);

	// return dontkeep
	return false;
}



NEOIP_NAMESPACE_END

