/*! \file
    \brief unit test for the asyncexe_t unit testt

*/

/* system include */
/* local include */
#include "neoip_asyncexe_nunit.hpp"
#include "neoip_asyncexe.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
asyncexe_testclass_t::asyncexe_testclass_t()	throw()
{
	// zero some field
	asyncexe	= NULL;
}

/** \brief Destructor
 */
asyncexe_testclass_t::~asyncexe_testclass_t()	throw()
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
nunit_err_t	asyncexe_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// sanity check
	DBG_ASSERT( !asyncexe );

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	asyncexe_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// delete the asyncexe_t
	nipmem_zdelete	asyncexe;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief general test 
 */
nunit_res_t	asyncexe_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	libsess_err_t		libsess_err;
	// build the command line to execute
	item_arr_t<std::string>	cmdline_arr;
	cmdline_arr	= item_arr_t<std::string>().append("echo").append("-n").append("bonjour");
	// launch the asyncexe_t	
	asyncexe	= nipmem_new asyncexe_t();
	libsess_err	= asyncexe->start(cmdline_arr, this, NULL);
	NUNIT_ASSERT( libsess_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                    asyncexe_query_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified when asyncexe_t has an event to report
 */
bool asyncexe_testclass_t::neoip_asyncexe_cb(void *cb_userptr, asyncexe_t &cb_asyncexe
					, const libsess_err_t &libsess_err
					, const bytearray_t &stdout_barray
					, const int &exit_status)	throw()
{
	// log to debug
	KLOG_ERR("enter libsess_err="<< libsess_err << " exit_status=" << exit_status
						<< " stdout_barray=" << stdout_barray);

	// if the recieved record IS the expected one, report a positive
	nunit_res_t	nunit_res;
	if( exit_status == 0 && stdout_barray.to_stdstring() == "bonjour"){
		nunit_res	= NUNIT_RES_OK;
	}else{
		nunit_res	= NUNIT_RES_ERROR;
	}

	// delete the asyncexe_query and mark it unused
	nipmem_zdelete	asyncexe;

	// notify the caller of the end of this testfunction
	nunit_ftor(nunit_res);
	// return tokeep
	return false;	
}

NEOIP_NAMESPACE_END
