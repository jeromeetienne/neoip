/*! \file
    \brief Header of the bt_swarm_full_t
    
*/


#ifndef __NEOIP_BT_SWARM_FULL_HPP__ 
#define __NEOIP_BT_SWARM_FULL_HPP__ 
/* system include */
#include <set>
/* local include */
#include "neoip_bt_swarm_full_cb.hpp"
#include "neoip_bt_swarm_full_wikidbg.hpp"
#include "neoip_bt_peersrc_peer.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_id.hpp"
#include "neoip_bt_protoflag.hpp"
#include "neoip_socket_full_cb.hpp"
#include "neoip_socket_addr.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_rate_estim.hpp"
#include "neoip_rate_limit.hpp"
#include "neoip_timeout.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_bt_pieceavail.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_full_sched_t;
class	bt_swarm_full_prec_t;
class	bt_swarm_full_sendq_t;
class	bt_swarm_full_utmsg_t;
class	bt_swarm_t;
class	bt_handshake_t;
class	bt_cmd_t;
class	pkt_t;
class	ipport_addr_t;
//class	socket_rate_vapi_t;

/** \brief class definition for bt_swarm_full_t
 */
class bt_swarm_full_t : NEOIP_COPY_CTOR_DENY, private socket_full_cb_t, private zerotimer_cb_t
				, private timeout_cb_t
				, private wikidbg_obj_t<bt_swarm_full_t, bt_swarm_full_wikidbg_init>
				{
private:
	bt_swarm_t *	bt_swarm;		//!< backpointer to the linked bt_swarm_t
	bt_id_t		m_remote_peerid;	//!< the peer id of the remote peer
	bt_protoflag_t	remote_protoflag;	//!< the bt_protoflag_t of the remote peer
	bt_pieceavail_t	m_remote_pavail;	//!< the available pieces of the remote peer
	bitfield_t	m_remote_pwish;		//!< bitfield_t of pieces remotly wished

	bool		m_local_doauth_req;	//!< true if the local peer allows remote peer to request
	bool		m_other_doauth_req;	//!< true if the remote peer allows local peer to request
	bool		m_local_dowant_req;	//!< true if the local peer want to request remote peer
	bool		m_other_dowant_req;	//!< true if the remote peer want to request local peer

	rate_estim_t<size_t>	m_recv_rate;	//!< estimate the recv_rate
	rate_estim_t<size_t>	m_xmit_rate;	//!< estimate the recv_rate
	bt_swarm_full_sched_t *	m_full_sched;	//!< the context for the scheduling of this swarm_full

	bytearray_t	recved_data;	//!< the receved data not yet parsed as bt_cmdtype_t
	bool		autodelete()		throw();

	/*************** sub structure	***************************************/
	bt_swarm_full_prec_t *	m_full_prec;
	bt_swarm_full_sendq_t *	sendq;	
	bt_swarm_full_utmsg_t *	m_full_utmsg;

	/*************** idle_timeout	***************************************/
	timeout_t	idle_timeout;	//!< triggered when the remote peer has been idle too long
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** first_parse_zerotimer	*******************************/
	zerotimer_t	first_parse_zerotimer;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();

	bool		notified_as_open;	//!< true is cnx_opened has been notified
	bool		notify_cnx_open()	throw();

	/************** socket_full_t	***************************************/
	socket_full_t*	socket_full;
	bool		neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
				, const socket_event_t &socket_event)		throw();

	/************** socket_event_t handling	*******************************/
	bool		parse_recved_data()					throw();
	bool		parse_recved_cmd(pkt_t &pkt)				throw();

	/************** command parsing	***************************************/
	bool		parse_doauth_req_cmd(pkt_t &pkt)			throw();
	bool		parse_unauth_req_cmd(pkt_t &pkt)			throw();
	bool		parse_dowant_req_cmd(pkt_t &pkt)			throw();
	bool		parse_unwant_req_cmd(pkt_t &pkt)			throw();
	bool		parse_piece_isavail_cmd(pkt_t &pkt)			throw();
	bool		parse_piece_bfield_cmd(pkt_t &pkt)			throw();
	bool		parse_block_req(pkt_t &pkt)				throw();
	bool		parse_block_rep(pkt_t &pkt)				throw();
	bool		parse_block_del(pkt_t &pkt)				throw();
	bool		parse_utmsg_payl(pkt_t &pkt)				throw();

	/*************** callback stuff	***************************************/
	bt_swarm_full_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const bt_swarm_full_event_t &event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_full_t() 		throw();
	~bt_swarm_full_t()		throw();
	
	/*************** setup function	***************************************/
	bt_err_t	start(bt_swarm_t *bt_swarm, socket_full_t *socket_full
					, const bytearray_t &recved_data
					, const bt_handshake_t &remote_handshake)	throw();
					
	/*************** query function	***************************************/
	bt_swarm_t *		get_swarm()		const throw()	{ return bt_swarm;		}
	bt_swarm_full_prec_t *	full_prec()		const throw()	{ return m_full_prec;		}
	bt_swarm_full_sched_t *	full_sched()		const throw()	{ return m_full_sched;		}
	bt_swarm_full_utmsg_t *	full_utmsg()		const throw()	{ return m_full_utmsg;		}
	const bt_id_t &		remote_peerid()		const throw()	{ return m_remote_peerid;	}
	const bt_protoflag_t &	protoflag()		const throw()	{ return remote_protoflag;	}
	const bt_pieceavail_t &	remote_pavail()		const throw()	{ return m_remote_pavail;	}
	const bitfield_t &	remote_pwish()		const throw()	{ return m_remote_pwish;	}
	bool			is_seed()		const throw()	{ return remote_pavail().is_fully_avail();}
	bool			is_leech()		const throw()	{ return !is_seed();		}
	double			recv_rate_avg()		const throw()	{ return m_recv_rate.average();	}
	double			xmit_rate_avg()		const throw()	{ return m_xmit_rate.average();	}
	bool			local_doauth_req()	const throw()	{ return m_local_doauth_req;	}
	bool			other_doauth_req()	const throw()	{ return m_other_doauth_req;	}
	bool			local_dowant_req()	const throw()	{ return m_local_dowant_req;	}
	bool			other_dowant_req()	const throw()	{ return m_other_dowant_req;	}
	const socket_addr_t &	remote_addr()		const throw();
	bt_peersrc_peer_t	peersrc_peer()		const throw();

	/*************** action function	*******************************/
	bt_swarm_full_t &set_doauth_req(bool value)				throw();
	void		send_cmd(const bt_cmd_t &bt_cmd)			throw();
	void		declare_piece_newly_avail(size_t piece_idx)		throw();
	void		declare_piece_nomore_avail(size_t piece_idx)		throw();
	void		update_listen_pview(const ipport_addr_t &new_listen_pview)throw();

	/*************** Special case for handling bt_utmsg_cnx_t	*******/
	bool		notify_utmsg_punavail(size_t piece_idx)			throw();
	bool		notify_utmsg_dowish_index(size_t pieceidx)		throw();
	bool		notify_utmsg_nowish_index(size_t pieceidx)		throw();
	bool		notify_utmsg_dowish_field(const bitfield_t &new_remote_pwish)throw();

	/*************** List of friend class	*******************************/
	friend class	bt_swarm_full_wikidbg_t;
	friend class	bt_swarm_full_prec_t;
	friend class	bt_swarm_full_sched_t;
	friend class	bt_swarm_full_sendq_t;
	friend class	bt_swarm_full_utmsg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_FULL_HPP__  */



