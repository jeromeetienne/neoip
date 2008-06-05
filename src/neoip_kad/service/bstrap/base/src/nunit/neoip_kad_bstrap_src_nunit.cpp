/*! \file
    \brief unit test for the kad_bstrap_src_t unit test

\par Possible Improvements
- this unit test is way to basic.
  - a lot of thing may go wrong and this test will still pass
  - TODO find a good test and code it
- this nunit should reproduce the usage made by the kad_bstrap.cpp

*/

/* system include */
#include <fstream>
/* local include */
#include "neoip_kad_bstrap_src_nunit.hpp"
#include "neoip_kad_bstrap_src.hpp"
#include "neoip_kad_addr.hpp"
#include "neoip_nslan.hpp"
#include "neoip_file.hpp"
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
kad_bstrap_src_testclass_t::kad_bstrap_src_testclass_t()	throw()
{
	// zero some field
	kad_bstrap_src	= NULL;
	nslan_listener	= NULL;
	nslan_peer	= NULL;
	nslan_publish	= NULL;
}

/** \brief Destructor
 */
kad_bstrap_src_testclass_t::~kad_bstrap_src_testclass_t()	throw()
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
nunit_err_t	kad_bstrap_src_testclass_t::neoip_nunit_testclass_init()	throw()
{
	kad_err_t	kad_err;
	inet_err_t	inet_err;
	// log to debug
	KLOG_DBG("enter");
	
	// sanity check
	DBG_ASSERT( !kad_bstrap_src );

	// create a nslan_listener_t
	nslan_listener	= nipmem_new nslan_listener_t();
	inet_err	= nslan_listener->start("127.255.255.255:4000");
	if( inet_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, inet_err.to_string());
	
	// create a nslan_peer_t
	nslan_realmid_t	nslan_realmid	= "realm_id for kad_bstrap_src_nunit";
	nslan_peer	= nipmem_new nslan_peer_t(nslan_listener, nslan_realmid);

	// create a nslan_rec_t
	nslan_keyid_t	nslan_keyid	= "kad_strap2_src nunit but should be derived for kad_realmid";
	bytearray_t	payload;
	payload << kad_addr_t("20.20.20.20:1", kad_peerid_t::build_random());
	nslan_rec_t	nslan_rec(nslan_keyid, payload.to_datum(), delay_t::from_sec(20));

	// publish the record
	nslan_publish	= nipmem_new nslan_publish_t(nslan_peer, nslan_rec);
	
	// create the temporary files needed for the test
	create_dynastat_file();

	// init the kad_bstrap_src
	kad_bstrap_src	= nipmem_new kad_bstrap_src_t();
	kad_err		= kad_bstrap_src->start(dynafile_path.to_string(), statfile_path.to_string()
								, nslan_peer, nslan_keyid, this, NULL);

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	kad_bstrap_src_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	
	// delete the kad_bstrap_src
	if( kad_bstrap_src ){
		nipmem_delete	kad_bstrap_src;
		kad_bstrap_src	= NULL;
	}
	// delete the nslan_publish
	if( nslan_publish ){
		nipmem_delete	nslan_publish;
		nslan_publish	= NULL;
	}	
	// delete the nslan_peer
	if( nslan_peer ){
		nipmem_delete	nslan_peer;
		nslan_peer	= NULL;
	}	
	// delete the nslan_listener
	if( nslan_listener ){
		nipmem_delete	nslan_listener;
		nslan_listener	= NULL;
	}	

	// delete the temporary files created for the test
	delete_dynastat_file();
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  create/deletes temporary file for this nunit
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Create the temporary files for the dynamic/static file of ipport_strlist_src_t
 */
file_err_t	kad_bstrap_src_testclass_t::create_dynastat_file()	throw()
{
	std::ofstream outfile;
	// build the temporary file_path_t for both files
	dynafile_path	= file_utils_t::get_temp_path("/tmp/neoip_kad_bstrap_nunit_dynafile");
	statfile_path	= file_utils_t::get_temp_path("/tmp/neoip_kad_bstrap_nunit_statfile");

	// create the dynafile
	outfile.open(dynafile_path.to_string().c_str(), std::ofstream::out | std::ofstream::trunc);
#if 1
	outfile << "# this is a comment\n";
	outfile << "1.1.1.1:1\n";
	outfile << "# this is a another comment\n";
	outfile << "1.1.1.1:2\n";
	outfile << "1.1.1.1:3\n";
	outfile << "1.1.1.1:4\n";
#endif
	outfile.close();

	// create the statfile
	outfile.open(statfile_path.to_string().c_str(), std::ofstream::out | std::ofstream::trunc);
#if 1
	outfile << "# this is a comment\n";
	outfile << "99.99.99.99:1\n";
	outfile << "# this is a another comment\n";
	outfile << "99.99.99.99:2\n";
	outfile << "99.99.99.99:3\n";
	outfile << "99.99.99.99:4\n";
#endif
	outfile.close();
	
	// return no error
	return file_err_t::OK;
}

/** \brief Delete the temporary files for the dynamic/static file of ipport_strlist_src_t
 * 
 * - work ok even if the files have not been created
 *   - required for the neoip_nunit_testclass_deinit()
 */
file_err_t	kad_bstrap_src_testclass_t::delete_dynastat_file()	throw()
{
	// remove the dynafile if it exist
	if( !file_stat_t(dynafile_path).is_null() )	file_utils_t::remove_file(dynafile_path);
	// remove the statfile if it exist
	if( !file_stat_t(statfile_path).is_null() )	file_utils_t::remove_file(statfile_path);
	// return no error
	return file_err_t::OK;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     kad_bstrap_src callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////


/** \brief callback notified by \ref kad_bstrap_src_t when to notify an ipport_addr_t
 */
bool kad_bstrap_src_testclass_t::neoip_kad_bstrap_src_cb(void *cb_userptr
					, kad_bstrap_src_t &cb_kad_bstrap_src
					, const kad_addr_t &kad_addr)	throw()
{
	// log to debug
	KLOG_ERR("enter notified kad_addr=" << kad_addr);

// TODO this test will never stop as the kad_bstrap_src_t is a never-ending source
// - find a good way to test kad_bstrap_src_t

	// if the source reached its end, exit the test
	if( kad_addr.is_null() ){
		DBG_ASSERT( 0 );
		// delete the kad_bstrap_src and mark it unused
		nipmem_delete	kad_bstrap_src;
		kad_bstrap_src	= NULL;
		// notify the caller of the end of this testfunction
		nunit_ftor(NUNIT_RES_OK);
		// return 'dontkeep'
		return false;
	}
	
	// ask for more ipport_addr_t
	kad_bstrap_src->get_more();
	
	// return 'tokeep'
	return true;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     list of nunit test functions
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief general test 
 */
nunit_res_t	kad_bstrap_src_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
#if 1
	// TODO find a good way to test kad_bstrap_src_t
	return NUNIT_RES_OK;
#else
	// request one address
	kad_bstrap_src->get_more();
	
	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
#endif
}



NEOIP_NAMESPACE_END
