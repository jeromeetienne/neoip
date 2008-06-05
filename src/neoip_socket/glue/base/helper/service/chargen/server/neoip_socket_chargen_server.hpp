/*! \file
    \brief Header of the test of socket_client_t
*/


#ifndef __NEOIP_SOCKET_CHARGEN_SERVER_HPP__ 
#define __NEOIP_SOCKET_CHARGEN_SERVER_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_socket_resp_cb.hpp"
#include "neoip_socket_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;


// list of forward declaration
class	socket_resp_arg_t;
class	socket_chargen_server_cnx_t;

/** \brief Class which implement a chargen server
 */
class socket_chargen_server_t : NEOIP_COPY_CTOR_DENY, private socket_resp_cb_t {
private:
	/*************** socket_resp_t	***************************************/
	socket_resp_t *	socket_resp;
	bool		neoip_socket_resp_event_cb(void *userptr, socket_resp_t &cb_socket_resp
						, const socket_event_t &socket_event)	throw();

	/*************** remote connection	*******************************/
	std::list<socket_chargen_server_cnx_t *>		cnx_db;
	void cnx_dolink(socket_chargen_server_cnx_t *cnx)	throw()	{ cnx_db.push_back(cnx);	}
	void cnx_unlink(socket_chargen_server_cnx_t *cnx)	throw()	{ cnx_db.remove(cnx);		}
public:
	/*************** ctor/dtor	***************************************/
	socket_chargen_server_t()		throw();
	~socket_chargen_server_t()		throw();
	
	/*************** Setup function	***************************************/
	socket_err_t	start(const socket_resp_arg_t &resp_arg)	throw();
	
	/*************** List of friend class	*******************************/
	friend class	socket_chargen_server_cnx_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_CHARGEN_SERVER_HPP__  */



