/*! \file
    \brief Header of the test of tcp_client_t
*/


#ifndef __NEOIP_TCP_CHARGEN_SERVER_CNX_HPP__ 
#define __NEOIP_TCP_CHARGEN_SERVER_CNX_HPP__ 
/* system include */
/* local include */
#include "neoip_tcp_full_cb.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	tcp_chargen_server_t;
class	pkt_t;

/** \brief Class which implement a chargen server
 */
class tcp_chargen_server_cnx_t : NEOIP_COPY_CTOR_DENY, private tcp_full_cb_t {
private:
	tcp_chargen_server_t *	chargen_server;	//!< backpointer on the chargen_server
	size_t				counter;
	/*************** tcp_full_t	***************************************/
	tcp_full_t *	tcp_full;
	bool		neoip_tcp_full_event_cb(void *userptr, tcp_full_t &cb_tcp_full
						, const tcp_event_t &tcp_event)	throw();

	/*************** Internal function	*******************************/
	void		fill_xmitbuf()			throw();
public:
	/*************** ctor/dtor	***************************************/
	tcp_chargen_server_cnx_t(tcp_chargen_server_t *chargen_server)		throw();
	~tcp_chargen_server_cnx_t()						throw();
	
	/*************** Setup function	***************************************/
	inet_err_t	start(tcp_full_t *tcp_full)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_CHARGEN_SERVER_CNX_HPP__  */



