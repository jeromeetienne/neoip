/*! \file
    \brief Declaration of the udp_resp_t
    
*/


#ifndef __NEOIP_NC2_SERVER_HPP__ 
#define __NEOIP_NC2_SERVER_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_udp.hpp"
#include "neoip_nlay.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	nc2_server_t;

/** \brief do a unit test on \ref udp_resp_t
 * 
 * - it respond to connection and log when packet are received
 */
class nc2_server_t : public udp_resp_cb_t {
private:
	class		cnx_udp_t;
	class		cnx_nlay_t;
	
	udp_resp_t	udp_resp;
	bool		neoip_inet_udp_resp_event_cb(void *userptr, udp_resp_t &cb_udp_resp
					, const udp_event_t &udp_event)		throw();
	nlay_profile_t	nlay_profile;
	nlay_resp_t	nlay_resp;

	// handle the list of incoming udp connections
	std::list<cnx_udp_t *>	cnx_udp_list;
	void cnx_udp_link(cnx_udp_t *cnx)	throw()	{ cnx_udp_list.push_back(cnx);	}
	void cnx_udp_unlink(cnx_udp_t *cnx)	throw()	{ cnx_udp_list.remove(cnx);	}

	// handle the list of incoming nlay connections
	std::list<cnx_nlay_t *>	cnx_nlay_list;
	void cnx_nlay_link(cnx_nlay_t *cnx)	throw()	{ cnx_nlay_list.push_back(cnx);	}
	void cnx_nlay_unlink(cnx_nlay_t *cnx)	throw()	{ cnx_nlay_list.remove(cnx);	}
	
public:
	nc2_server_t(const std::string &addr_str)		throw();
	~nc2_server_t()		throw();
};

/** \brief handle a given udp connection received by the nc2_server_t
 */
class nc2_server_t::cnx_udp_t : private udp_full_cb_t {
private:
	nc2_server_t *	nc2_server;		//!< backpoint to nc2_server
	udp_full_t *	udp_full;		//!< udp_full_t struct
	bool		neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
					, const udp_event_t &udp_event)		throw();
	bool		recv_udp_pkt(pkt_t &pkt)				throw();
public:
	cnx_udp_t(nc2_server_t *nc2_server, udp_full_t *udp_full)	throw();
	~cnx_udp_t()							throw();
};

/** \brief handle a given nlay connection received by the nc2_server_t
 */
class nc2_server_t::cnx_nlay_t : private udp_full_cb_t, private nlay_full_cb_t {
private:
	nc2_server_t *	nc2_server;		//!< backpoint to nc2_server

	udp_full_t *	udp_full;
	bool		neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
					, const udp_event_t &udp_event)		throw();

	nlay_full_t *	nlay_full;
	bool		neoip_nlay_full_event_cb(void *cb_userptr, nlay_full_t &cb_nlay_full
					, const nlay_event_t &nlay_event)	throw();
public:
	cnx_nlay_t(nc2_server_t *nc2_server, udp_full_t *udp_full, nlay_full_t *nlay_full)	throw();
	~cnx_nlay_t()							throw();
};


NEOIP_NAMESPACE_END

#endif // __NEOIP_NC2_SERVER_HPP__ 



