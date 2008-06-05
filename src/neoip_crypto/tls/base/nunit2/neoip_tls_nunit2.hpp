/*! \file
    \brief Header of the test of socket_client_t
*/


#ifndef __NEOIP_TLS_NUNIT2_HPP__ 
#define __NEOIP_TLS_NUNIT2_HPP__ 
/* system include */
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_socket_client_cb.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	pkt_t;
class	tls_itor_t;
class	tls_full_t;

/** \brief Class which implement a nunit for the tls
 */
class tls_testclass2_t : public nunit_testclass_api_t, private socket_client_cb_t {
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously
	tls_itor_t *		tls_itor;
	tls_full_t *		tls_full;

	/*************** Internal function	*******************************/
	void		xmit_ifneeded()		throw();

	/*************** socket_client_t	*******************************/
	socket_client_t*socket_client;
	bool		neoip_socket_client_event_cb(void *userptr, socket_client_t &cb_socket_client
					, const socket_event_t &socket_event)	throw();
	bool		handle_cnx_established()				throw();
	bool		handle_recved_data(pkt_t &pkt)				throw();
	bool		handle_maysend_on()					throw();
public:
	/*************** ctor/dtor	***************************************/
	tls_testclass2_t()	throw();
	~tls_testclass2_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();
	
	/*************** nunit test function	*******************************/
	nunit_res_t	cnx_establishement(const nunit_testclass_ftor_t &testclass_ftor)throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TLS_NUNIT2_HPP__  */



