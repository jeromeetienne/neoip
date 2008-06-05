/*! \file
    \brief Header of the bt_swarm_itor_t
    
*/


#ifndef __NEOIP_BT_SWARM_ITOR_HPP__ 
#define __NEOIP_BT_SWARM_ITOR_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_swarm_itor_cb.hpp"
#include "neoip_bt_swarm_itor_wikidbg.hpp"
#include "neoip_bt_peersrc_peer.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_socket_client_cb.hpp"
#include "neoip_socket_client.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_peersrc_t;
class	pkt_t;

/** \brief class definition for bt_swarm_itor
 */
class bt_swarm_itor_t : NEOIP_COPY_CTOR_DENY, private socket_client_cb_t 
				, private wikidbg_obj_t<bt_swarm_itor_t, bt_swarm_itor_wikidbg_init>
				{
private:
	bt_swarm_peersrc_t *	m_swarm_peersrc;	//!< backpointer to the linked bt_swarm_peersrc_t
	bt_peersrc_peer_t	m_peersrc_peer;		//!< the bt_peersrc_peer_t contacted by this itor
	bytearray_t		recved_data;
	
	/*************** Internal function	*******************************/
	
	/*************** socket_client_t	*******************************/
	socket_client_t*socket_client;
	bool		neoip_socket_client_event_cb(void *userptr, socket_client_t &cb_socket_client
				, const socket_event_t &socket_event)		throw();
	/*************** packet reception	*******************************/
	bool		handle_cnx_established()				throw();
	bool		handle_recved_data(pkt_t &pkt)				throw();

	/*************** callback stuff	***************************************/
	bt_swarm_itor_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_failure(const std::string &reason)	throw();
	bool			notify_failure(const bt_err_t &bt_err)		throw();
	bool			notify_success(socket_full_t *socket_full
					, const bytearray_t &recved_data
					, const bt_handshake_t &remote_handshake)	throw();
	bool			notify_callback(const bt_err_t &bt_err, socket_full_t *socket_full
					, const bytearray_t &recved_data
					, const bt_handshake_t &remote_handshake)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_itor_t() 		throw();
	~bt_swarm_itor_t()		throw();
	
	/*************** setup function	***************************************/
	bt_err_t	start(bt_swarm_peersrc_t *m_swarm_peersrc
					, const bt_peersrc_peer_t &m_peersrc_peer				
					, bt_swarm_itor_cb_t *callback, void *userptr)	throw();

	/*************** query function	***************************************/
	bt_swarm_peersrc_t *	swarm_peersrc()	const throw()	{ return m_swarm_peersrc;	}
	const bt_peersrc_peer_t&peersrc_peer()	const throw()	{ return m_peersrc_peer;	}
	const socket_addr_t &	remote_addr()	const throw();
	
	/*************** List of friend class	*******************************/
	friend class	bt_swarm_itor_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_ITOR_HPP__  */



