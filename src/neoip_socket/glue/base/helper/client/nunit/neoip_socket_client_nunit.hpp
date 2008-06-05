/*! \file
    \brief Header of the test of socket_client_t
*/


#ifndef __NEOIP_SOCKET_CLIENT_NUNIT_HPP__ 
#define __NEOIP_SOCKET_CLIENT_NUNIT_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_socket_resp_cb.hpp"
#include "neoip_socket_nunit_domain.hpp"
#include "neoip_socket_full_cb.hpp"	// NOTE: here only because there is a bug when deleting a non started udp_full_t
					// TODO to fix
#include "neoip_socket_client_cb.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

/** \brief Class which implement a nunit for the socket_client_t
 * 
 * - currently it only test if the connection works
 */
class socket_client_testclass_t : public nunit_testclass_api_t, private socket_client_cb_t
						, private socket_resp_cb_t
						, private socket_full_cb_t
						{
private:
	socket_domain_t		socket_domain;
	socket_type_t		socket_type;
	socket_nunit_domain_t *	nunit_domain;

	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously 
	

	/*************** socket_resp_t	***************************************/
	socket_resp_t *	socket_resp;
	bool		neoip_socket_resp_event_cb(void *userptr, socket_resp_t &cb_socket_resp
						, const socket_event_t &socket_event)	throw();
	// TODO to remove - only to fix a bug which prevent deleting a non start()ed socket_full_t
	bool		neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
						, const socket_event_t &socket_event)	throw();

	/*************** socket_client_t	*******************************/
	socket_client_t*socket_client;
	bool		neoip_socket_client_event_cb(void *userptr, socket_client_t &cb_socket_client
				, const socket_event_t &socket_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	socket_client_testclass_t(const socket_domain_t &socket_domain
					, const socket_type_t &socket_type)	throw();
	~socket_client_testclass_t()						throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	cnx_establishement(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_CLIENT_NUNIT_HPP__  */



