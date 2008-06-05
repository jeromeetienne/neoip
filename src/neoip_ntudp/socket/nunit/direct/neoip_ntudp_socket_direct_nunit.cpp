/*! \file
    \brief Definition of the unit test for the ntudp socket with DIRECT connection
    
*/

/* system include */
/* local include */
#include "neoip_ntudp_socket_direct_nunit.hpp"
#include "neoip_ntudp_socket_nunit_client_close.hpp"
#include "neoip_ntudp_socket_nunit_server_close.hpp"
#include "neoip_ntudp_peer.hpp"
#include "neoip_ntudp_nunit_helper.hpp"
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
ntudp_socket_direct_testclass_t::ntudp_socket_direct_testclass_t()	throw()
{
	// zero some field
	peer_4client	= NULL;
	peer_4server	= NULL;
	server_close	= NULL;
}

/** \brief Destructor
 */
ntudp_socket_direct_testclass_t::~ntudp_socket_direct_testclass_t()	throw()
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
nunit_err_t	ntudp_socket_direct_testclass_t::neoip_nunit_testclass_init()	throw()
{
	ntudp_err_t	ntudp_err;
	// log to debug
	KLOG_DBG("enter");
	
	// init the peer_4client
	DBG_ASSERT( !peer_4client );
	peer_4client	= nipmem_new ntudp_peer_t();
	ntudp_err	= ntudp_nunit_peer_start_inetreach(peer_4client, "127.0.0.1:4000");
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());

	// init the peer_4server
	DBG_ASSERT( !peer_4server );
	peer_4server	= nipmem_new ntudp_peer_t();
	ntudp_err	= ntudp_nunit_peer_start_inetreach(peer_4server, "127.0.0.1:4001");
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());

	// init the server_close
	server_close	= nipmem_new ntudp_socket_nunit_server_close_t();
	ntudp_err	= server_close->start(peer_4server);
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());

	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	ntudp_socket_direct_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");

	// delete all the pending client_close
	delete_all_client_close();

	// delete the server_close
	if( server_close ){
		nipmem_delete	server_close;
		server_close	= NULL;
	}
	// delete the peer_4client
	if( peer_4client ){
		nipmem_delete	peer_4client;
		peer_4client	= NULL;
	}
	// delete the peer_4server
	if( peer_4server ){
		nipmem_delete	peer_4server;
		peer_4server	= NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     nunit test function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief test with 1 client
 */
nunit_res_t	ntudp_socket_direct_testclass_t::one_client(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// launch 1 client
	ntudp_err_t	ntudp_err;
	ntudp_err	= launch_nb_client_close(1);
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

/** \brief test with 10 client simultaneously
 */
nunit_res_t	ntudp_socket_direct_testclass_t::ten_client(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	// launch 1 client
	ntudp_err_t	ntudp_err;
	ntudp_err	= launch_nb_client_close(10);
	if( ntudp_err.failed() )	return nunit_err_t(nunit_err_t::ERROR, ntudp_err.to_string());

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                          utility function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Launch nb_client ntudp_socket_nunit_client_close_t simultaneously
 */
ntudp_err_t	ntudp_socket_direct_testclass_t::launch_nb_client_close(size_t nb_launch)	throw()
{
	ntudp_err_t	ntudp_err;
	// create all the needed client
	for(size_t i = 0; i < nb_launch; i++){
		ntudp_socket_nunit_client_close_t *	client_close;
		// create the client_close
		client_close	= nipmem_new ntudp_socket_nunit_client_close_t();
		// link the client_close
		client_close_db.insert(client_close);
		// start the client_close
		ntudp_err	= client_close->start(peer_4client, peer_4server->local_peerid(), this, NULL);
		if( ntudp_err.failed() )	return ntudp_err;
	}
	// return no error
	return ntudp_err_t::OK;
}

/** \brief Delete all client_close
 */
void	ntudp_socket_direct_testclass_t::delete_all_client_close()	throw()
{
	// delete all the pending client_close
	while( !client_close_db.empty() ){
		ntudp_socket_nunit_client_close_t *	client_close = *client_close_db.begin();
		// remove it from the database
		client_close_db.erase(client_close);
		// delete the object itself
		nipmem_delete client_close;
	}
}
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//            ntudp_socket_nunit_client_close_t callback
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref ntudp_socket_nunit_client_close_t when to notify a result
 */
bool ntudp_socket_direct_testclass_t::neoip_ntudp_socket_nunit_client_close_cb(void *cb_userptr
				, ntudp_socket_nunit_client_close_t &cb_client_close
				, bool succeed)	throw()
{
	ntudp_socket_nunit_client_close_t *	client_close = &cb_client_close;
	// sanity check - the notifier MUST be in the database
	DBG_ASSERT( client_close_db.find(client_close) != client_close_db.end() );
	// remote the notifier from the database
	client_close_db.erase(client_close);
	// delete the notifier
	nipmem_delete client_close;		
	
	// if the client_close failed, notify nunit a error
	if( succeed == false ){
		delete_all_client_close();
		nunit_ftor(NUNIT_RES_ERROR);
		return false;
	}
		
	// if the database is now empty, notify nunit a suceess
	if( client_close_db.empty() ){
		nunit_ftor(NUNIT_RES_OK);
		return false;
	}
	
	// return 'dontkeep'
	return false;
}


NEOIP_NAMESPACE_END

