/*! \file
    \brief Header of the neoip_tcp

- see \ref neoip_tcp_echo.cpp
*/


#ifndef __NEOIP_TCP_ECHO_SERVER_HPP__ 
#define __NEOIP_TCP_ECHO_SERVER_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_tcp.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief a echo server on top of neoip_tcp as a unit test
 */
class tcp_echo_server_t : public tcp_resp_cb_t, public tcp_full_cb_t {
private:
	tcp_resp_t		tcp_resp;
	std::list<tcp_full_t *>	tcp_full_list;

	bool	neoip_tcp_resp_event_cb(void *cb_userptr, tcp_resp_t &cb_tcp_resp
							, const tcp_event_t &tcp_event)
							throw();
	bool	neoip_tcp_full_event_cb(void *cb_userptr, tcp_full_t &cb_tcp_full
							, const tcp_event_t &tcp_event )
							throw();						
public:
	tcp_echo_server_t()		throw();
	~tcp_echo_server_t()		throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_ECHO_SERVER_HPP__  */



