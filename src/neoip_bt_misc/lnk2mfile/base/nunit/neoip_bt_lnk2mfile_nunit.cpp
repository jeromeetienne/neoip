/*! \file
    \brief Definition of the unit test for the \ref bt_lnk2mfile_t

*/

/* system include */
/* local include */
#include "neoip_bt_lnk2mfile_nunit.hpp"
#include "neoip_bt_lnk2mfile.hpp"
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
bt_lnk2mfile_testclass_t::bt_lnk2mfile_testclass_t()	throw()
{
	// zero some field
	bt_lnk2mfile	= NULL;
}

/** \brief Destructor
 */
bt_lnk2mfile_testclass_t::~bt_lnk2mfile_testclass_t()	throw()
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
nunit_err_t	bt_lnk2mfile_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	bt_lnk2mfile_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// destruct the bt_lnk2mfile_t
	nipmem_zdelete	bt_lnk2mfile;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	bt_lnk2mfile_testclass_t::general(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	bt_err_t		bt_err;	
	std::string		link_addr;
	bt_lnk2mfile_type_t	link_type;

#if 0
	link_type	= bt_lnk2mfile_type_t();
	link_addr	= "/home/jerome/downloaded/ubuntu-6.10-desktop-i386.iso.torrent";
#endif
#if 1
	link_type	= "torrent";
	link_addr	= "http://torrent.ubuntu.com:6969/file?info_hash=%ED%8A%B7%FB%3AL%D3Nd%8EH%A4%0F%8F%DD%97%A4%F5%40%D2";
#endif
	// start the bt_lnk2mfile_t
	bt_lnk2mfile	= nipmem_new bt_lnk2mfile_t();
	bt_err		= bt_lnk2mfile->start(link_type, link_addr, this, NULL);
	NUNIT_ASSERT( bt_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	return NUNIT_RES_DELAYED;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     bt_lnk2mfile_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref bt_lnk2mfile_t to provide event
 */
bool	bt_lnk2mfile_testclass_t::neoip_bt_lnk2mfile_cb(void *cb_userptr, bt_lnk2mfile_t &cb_bt_lnk2mfile
				, const bt_err_t &bt_err, const bt_lnk2mfile_type_t &lnk2mfile_type
				, const bt_mfile_t &bt_mfile)			throw() 
{
	// log to debug
	KLOG_WARN("enter bt_err=" << bt_err << " lnk2mfile_type=" << lnk2mfile_type);

	// delete the bt_lnk2mfile_t
	nipmem_zdelete bt_lnk2mfile;
	
	// report the result to nunit depending on the notified bt_err_t
	if( bt_err.succeed() )	nunit_ftor( NUNIT_RES_OK	);
	else			nunit_ftor( NUNIT_RES_ERROR	);

	// return dontkeep
	return false;
}



NEOIP_NAMESPACE_END

