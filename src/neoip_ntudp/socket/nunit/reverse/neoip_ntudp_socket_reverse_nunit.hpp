

#ifndef __NEOIP_NTUDP_SOCKET_REVERSE_NUNIT_HPP__ 
#define __NEOIP_NTUDP_SOCKET_REVERSE_NUNIT_HPP__ 
/* system include */
#include <set>
/* local include */
#include "neoip_nunit_testclass_api.hpp"
#include "neoip_nunit_testclass_ftor.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_ntudp_socket_nunit_client_close_cb.hpp"
#include "neoip_ntudp_rdvpt_ftor.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ntudp_peer_t;
class	ntudp_socket_nunit_server_close_t;

/** \brief Class which implement a nunit for the ntudp_socket2_t
 */
class ntudp_socket_reverse_testclass_t : public nunit_testclass_api_t
					, private ntudp_rdvpt_ftor_cb_t
					, private ntudp_socket_nunit_client_close_cb_t
					{
private:
	nunit_testclass_ftor_t	nunit_ftor;	//!< the functor to notify nunit_res_t asynchronously
	ntudp_peer_t *		peer_4client;
	ntudp_peer_t *		peer_4relay;
	ntudp_peer_t *		peer_4server;
	
	/*************** ntudp_rdvpt_ftor_t	*******************************/
	bool		ntudp_rdvpt_ftor_cb(void *userptr, const ntudp_rdvpt_t &ntudp_rdvpt)throw();

	/*************** server close stuff	*******************************/
	ntudp_socket_nunit_server_close_t *	server_close;

	/*************** Internal function	*******************************/
	ntudp_err_t	launch_nb_client_close(size_t nb_launch)	throw();
	void		delete_all_client_close()			throw();

	/*************** client close stuff	*******************************/
	std::set<ntudp_socket_nunit_client_close_t *>	client_close_db;
	bool neoip_ntudp_socket_nunit_client_close_cb(void *cb_userptr
					, ntudp_socket_nunit_client_close_t &cb_client_close
					, bool succeed)	throw();
public:
	/*************** ctor/dtor	***************************************/
	ntudp_socket_reverse_testclass_t()	throw();
	~ntudp_socket_reverse_testclass_t()	throw();

	/*************** nunit_testclass init/deinit	***********************/
	nunit_err_t	neoip_nunit_testclass_init()	throw();
	void		neoip_nunit_testclass_deinit()	throw();

	/*************** nunit test function	*******************************/
	nunit_res_t	one_client(const nunit_testclass_ftor_t &testclass_ftor)	throw();
	nunit_res_t	ten_client(const nunit_testclass_ftor_t &testclass_ftor)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_SOCKET_REVERSE_NUNIT_HPP__  */



