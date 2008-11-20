/*! \file
    \brief Header of the rtmp_resp_cnx_t

*/


#ifndef __NEOIP_RTMP_RESP_CNX_HPP__
#define __NEOIP_RTMP_RESP_CNX_HPP__
/* system include */
/* local include */
#include "neoip_rtmp_resp_cnx_wikidbg.hpp"
#include "neoip_rtmp_resp_cnx_state.hpp"
#include "neoip_rtmp_err.hpp"
#include "neoip_socket_full_cb.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	rtmp_resp_t;
class	pkt_t;

/** \brief class definition for rtmp_resp_cnx_t
 *
 * - read the data from the responded connection until the infohash is known and
 *   then send it to the proper bt_swarm_t
 */
class rtmp_resp_cnx_t : NEOIP_COPY_CTOR_DENY, private socket_full_cb_t
			, private wikidbg_obj_t<rtmp_resp_cnx_t, rtmp_resp_cnx_wikidbg_init> {

private:
	rtmp_resp_t *		rtmp_resp;	//!< backpointer to the linked rtmp_resp_t
	bytearray_t		m_recved_data;	//!< the already received data
	rtmp_resp_cnx_state_t	m_state;	//!< the current flv_parse_state_t

	/*************** Internal function	*******************************/
	bool		autodelete()	throw();

	/*************** socket_full_t	***************************************/
	socket_full_t *	socket_full;
	bool		neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
						, const socket_event_t &socket_event)	throw();
	/*************** packet reception	*******************************/
	bool		handle_recved_data(pkt_t &pkt)		throw();
	bool		handle_waiting_itorsyn()		throw();
	bool		handle_waiting_itorack()		throw();
public:
	/*************** ctor/dtor	***************************************/
	rtmp_resp_cnx_t() 		throw();
	~rtmp_resp_cnx_t()		throw();

	/*************** setup function	***************************************/
	rtmp_err_t	start(rtmp_resp_t *rtmp_resp, socket_full_t *socket_full)	throw();

	/*************** list of friend class	*******************************/
	friend class	rtmp_resp_cnx_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RTMP_RESP_CNX_HPP__  */



