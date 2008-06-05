/*! \file
    \brief Definition of the unit test for the \ref socket_client_t

*/

/* system include */
/* local include */
#include "neoip_socket_client_nunit.hpp"
#include "neoip_socket_client.hpp"
#include "neoip_socket_nunit_domain.hpp"
#include "neoip_socket.hpp"
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
socket_client_testclass_t::socket_client_testclass_t(const socket_domain_t &socket_domain
					, const socket_type_t &socket_type)	throw()
{
	// copy the parameter
	this->socket_domain	= socket_domain;
	this->socket_type	= socket_type;
	// zero some field
	socket_resp		= NULL;
	socket_client		= NULL;
	nunit_domain		= NULL;
}

/** \brief Destructor
 */
socket_client_testclass_t::~socket_client_testclass_t()	throw()
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
nunit_err_t	socket_client_testclass_t::neoip_nunit_testclass_init()	throw()
{
	// sanity check
	DBG_ASSERT( !socket_client );
	DBG_ASSERT( !socket_resp );
	
	// allocate the nunit_domain
	nunit_domain	= nipmem_new	socket_nunit_domain_t(socket_domain);
	// return no error
	return nunit_err_t::OK;
}

/** \brief DeInit the testclass implementation
 */
void	socket_client_testclass_t::neoip_nunit_testclass_deinit()	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// destruct the socket client
	nipmem_zdelete socket_client;
	// destruct the socket responder
	nipmem_zdelete socket_resp;
	// delete the socket_nunit_domain_t
	nipmem_zdelete	nunit_domain;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      test_function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Test function
 */
nunit_res_t	socket_client_testclass_t::cnx_establishement(const nunit_testclass_ftor_t &testclass_ftor) throw()
{
	socket_err_t	socket_err;
	// log to debug
	KLOG_DBG("enter");

#if 1
	// init and start socket_resp_t
	socket_resp_arg_t	resp_arg;
	resp_arg	= nunit_domain->get_resp_arg();
	socket_resp	= nipmem_new socket_resp_t();
	socket_err	= socket_resp->start(resp_arg.type(socket_type), this, NULL);
	NUNIT_ASSERT( socket_err.succeed() );
#endif

	// init and start socket_client_t
	socket_itor_arg_t	itor_arg;
	itor_arg	= nunit_domain->get_itor_arg();
	socket_client	= nipmem_new socket_client_t();
	socket_err	= socket_client->start(itor_arg.type(socket_type), this, NULL);
	NUNIT_ASSERT( socket_err.succeed() );

	// copy the functor to report nunit_res_t asynchronously
	nunit_ftor	= testclass_ftor;
	// return a delayed nunit_res_t
	return NUNIT_RES_DELAYED;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_resp_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_resp_t when a connection is established
 */
bool	socket_client_testclass_t::neoip_socket_resp_event_cb(void *userptr, socket_resp_t &cb_socket_resp
							, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_ERR("enter event=" << socket_event);
	// sanity check - the event MUST be resp_ok
	DBG_ASSERT( socket_event.is_resp_ok() );
	
	// handle each possible events from its type
	switch( socket_event.get_value() ){
	case socket_event_t::CNX_ESTABLISHED:{
			// close the socket_full_t immediatly
			socket_full_t *	socket_full = socket_event.get_cnx_established();
#if 1
// TODO apparently there is a bug if i delete a socket_full which has no yet been started
// - due to the closure protocol which send packet
// - it appears with nlay on top of udp, not sure where is the bug
//   - maybe simply in the full_udp_close.cpp
// - and the scnx_full compute some key at this time
//   - it is unclear why this is done in .start()
			socket_err_t	socket_err;
			socket_err	= socket_full->start(this, NULL);
			DBG_ASSERT( socket_err.succeed() );	
#endif
			nipmem_delete	socket_full;
			break;}
	default:	DBG_ASSERT(0);
	}
	// return tokeep
	return true;
}

/** \brief callback notified by \ref socket_full_t when to notify an event
 */
bool	socket_client_testclass_t::neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
						, const socket_event_t &socket_event)	throw()
{
	// NOTE: this function is only here because there is a bug when deleting a non start()ed
	//       socket_full_t
	DBG_ASSERT(0);
	return true;
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     socket_client_t callback
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief callback notified by \ref socket_client_t to provide event
 */
bool	socket_client_testclass_t::neoip_socket_client_event_cb(void *userptr, socket_client_t &cb_socket_client
							, const socket_event_t &socket_event)	throw()
{
	// log to debug
	KLOG_ERR("enter event=" << socket_event);
	// sanity check - the event MUST be client_ok
	DBG_ASSERT( socket_event.is_client_ok() );

	// delete the socket_client
	nipmem_zdelete	socket_client;
	
	// report the result depending on the socket_event
	if( socket_event.is_cnx_established() )	nunit_ftor(NUNIT_RES_OK);
	else					nunit_ftor(NUNIT_RES_ERROR);

	// return dontkeep
	return false;
}



NEOIP_NAMESPACE_END

