/*! \file
    \brief Declaration of the tcp_resp_t
    
*/


#ifndef __NEOIP_TCP_UTEST_RESP_HPP__ 
#define __NEOIP_TCP_UTEST_RESP_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_tcp.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	utest_tcp_resp_t;

/** \brief handle a given connection received by the utest_tcp_resp_t
 */
class utest_tcp_resp_cnx_t : private tcp_full_cb_t {
private:
	utest_tcp_resp_t *	utest_tcp_resp;
	tcp_full_t *		tcp_full;
	bool			neoip_tcp_full_event_cb(void *userptr, tcp_full_t &cb_tcp_full
							, const tcp_event_t &tcp_event)		throw();
public:
	utest_tcp_resp_cnx_t(utest_tcp_resp_t *utest_tcp_resp, tcp_full_t *tcp_full)	throw();
	~utest_tcp_resp_cnx_t()								throw();
};

/** \brief do a unit test on \ref tcp_resp_t
 * 
 * - it respond to connection and log when packet are received
 */
class utest_tcp_resp_t : public tcp_resp_cb_t {
private:
	tcp_resp_t	tcp_resp;
	bool		neoip_tcp_resp_event_cb(void *userptr, tcp_resp_t &cb_tcp_resp
							, const tcp_event_t &tcp_event)		throw();

	// handle the list of incoming connections
	std::list<utest_tcp_resp_cnx_t *>	cnx_list;
	void cnx_link(utest_tcp_resp_cnx_t *cnx)	throw()	{ cnx_list.push_back(cnx);	}
	void cnx_unlink(utest_tcp_resp_cnx_t *cnx)	throw()	{ cnx_list.remove(cnx);		}
	
public:
	utest_tcp_resp_t()		throw();
	~utest_tcp_resp_t()		throw();
friend	class utest_tcp_resp_cnx_t;
};

NEOIP_NAMESPACE_END

#endif // __NEOIP_TCP_UTEST_RESP_HPP__ 



