/*! \file
    \brief Header of the bt_session_cnx_t
    
*/


#ifndef __NEOIP_BT_SESSION_CNX_HPP__ 
#define __NEOIP_BT_SESSION_CNX_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_bt_handshake.hpp"
#include "neoip_socket_full_cb.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_session_t;
class	pkt_t;

/** \brief class definition for bt_session_cnx_t
 * 
 * - read the data from the responded connection until the infohash is known and 
 *   then send it to the proper bt_swarm_t
 */
class bt_session_cnx_t : NEOIP_COPY_CTOR_DENY, private socket_full_cb_t {
private:
	bt_session_t *	bt_session;	//!< backpointer to the linked bt_session_t
	bytearray_t	recved_data;
	bt_handshake_t	remote_handshake;
	
	/*************** Internal function	*******************************/
	bool		autodelete()	throw();

	/*************** socket_full_t	***************************************/
	socket_full_t *	socket_full;
	bool		neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
						, const socket_event_t &socket_event)	throw();
	/*************** packet reception	*******************************/
	bool		handle_recved_data(pkt_t &pkt)	throw();
	bool		parse_first_half_handshake()	throw();
	bool		parse_second_half_handshake()	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_session_cnx_t() 		throw();
	~bt_session_cnx_t()		throw();
	
	/*************** setup function	***************************************/
	bt_err_t	start(bt_session_t *bt_session, socket_full_t *socket_full)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SESSION_CNX_HPP__  */



