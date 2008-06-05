/*! \file
    \brief Declaration of the ntudp_npos_natlback_t
    
*/


#ifndef __NEOIP_NTUDP_NPOS_NATLBACK_CNX_HPP__ 
#define __NEOIP_NTUDP_NPOS_NATLBACK_CNX_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_npos_natlback.hpp"
#include "neoip_pkt.hpp"
#include "neoip_serial.hpp"
#include "neoip_udp_full_cb.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;


// list of forward declaration
class ntudp_nonce_t;

/** \brief handle a given connection received by the ntudp_npos_natlback_t udp_resp_t
 * 
 * - usefull IIF under attack or receiving intended packet 
 */
class ntudp_npos_natlback_t::cnx_t : private udp_full_cb_t, NEOIP_COPY_CTOR_DENY {
private:
	ntudp_npos_natlback_t *ntudp_npos_natlback;	//!< backpointer to the ntudp_npos_natlback_t

	/*************** udp_full_t	***************************************/
	udp_full_t *	udp_full;	//!< the udp_full_t 
	bool		neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
							, const udp_event_t &udp_event)	throw();
	/*************** packet reception	*******************************/
	bool		handle_recved_data(pkt_t &pkt)				throw();
public:
	/*************** ctor/dtor	***************************************/
	cnx_t(ntudp_npos_natlback_t *ntudp_npos_natlback, udp_full_t *udp_full)	throw();
	~cnx_t()								throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_NPOS_NATLBACK_CNX_HPP__  */



