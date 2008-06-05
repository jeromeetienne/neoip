/*! \file
    \brief Header of the test of tcp_client_t
*/


#ifndef __NEOIP_TCP_CHARGEN_SERVER_HPP__ 
#define __NEOIP_TCP_CHARGEN_SERVER_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_tcp_resp_cb.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;


// list of forward declaration
class	tcp_resp_arg_t;
class	tcp_chargen_server_cnx_t;

/** \brief Class which implement a chargen server
 */
class tcp_chargen_server_t : NEOIP_COPY_CTOR_DENY, private tcp_resp_cb_t {
private:
	/*************** tcp_resp_t	***************************************/
	tcp_resp_t *	tcp_resp;
	bool		neoip_tcp_resp_event_cb(void *userptr, tcp_resp_t &cb_tcp_resp
						, const tcp_event_t &tcp_event)	throw();

	/*************** remote connection	*******************************/
	std::list<tcp_chargen_server_cnx_t *>		cnx_db;
	void cnx_dolink(tcp_chargen_server_cnx_t *cnx)	throw()	{ cnx_db.push_back(cnx);	}
	void cnx_unlink(tcp_chargen_server_cnx_t *cnx)	throw()	{ cnx_db.remove(cnx);		}
public:
	/*************** ctor/dtor	***************************************/
	tcp_chargen_server_t()		throw();
	~tcp_chargen_server_t()	throw();
	
	/*************** Setup function	***************************************/
	inet_err_t	start(const tcp_resp_arg_t &resp_arg)			throw();
	
	/*************** List of friend class	*******************************/
	friend class	tcp_chargen_server_cnx_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_CHARGEN_SERVER_HPP__  */



