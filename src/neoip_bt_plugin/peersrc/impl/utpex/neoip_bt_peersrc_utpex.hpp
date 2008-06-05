/*! \file
    \brief Header of the bt_peersrc_utpex_t

- test from ludde over IRC as a description of UTPEX
"Every minute a PEX message is sent out. This is used to let the other peer know about which 
peers that I'm currently connected to. The message contains the diffs since the previous 
message. The very first message contains a full set. If the difference is empty, no message
needs to be sent out.

added contains a concatenation of 6-byte strings (one for each peer to be added to the list
of peers I'm connected to). The added message contains all peers that were ADDED. It's 
ip(4 byte):port(2 byte) in packed big endian form. dropped contains the same thing, but 
the peers that were DROPPED. These should then be removed from the set.

added.f contains one byte per added peer. (so length(added.f) == length(added) / 6) Currently 
the only defined bit is &1, it's 1 if the peer wants us to connect to it using encrypted 
handshake. The other bits must be 0."
*/


#ifndef __NEOIP_BT_PEERSRC_UTPEX_HPP__ 
#define __NEOIP_BT_PEERSRC_UTPEX_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_peersrc_utpex_profile.hpp"
#include "neoip_bt_peersrc_utpex_wikidbg.hpp"
#include "neoip_bt_peersrc_vapi.hpp"
#include "neoip_bt_peersrc_cb.hpp"
#include "neoip_bt_utmsg_vapi.hpp"
#include "neoip_bt_utmsg_cb.hpp"
#include "neoip_bt_utmsgtype.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_peersrc_peer_arr.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_t;
class	bt_peersrc_utpex_cnx_t;
class	pkt_t;

/** \brief class definition for bt_peersrc for http
 * 
 * - it MUST be ctor after bt_swarm_t and dtor before
 */
class bt_peersrc_utpex_t : NEOIP_COPY_CTOR_DENY, public bt_peersrc_vapi_t, public bt_utmsg_vapi_t
			, private wikidbg_obj_t<bt_peersrc_utpex_t, bt_peersrc_utpex_wikidbg_init, bt_peersrc_vapi_t, bt_utmsg_vapi_t>
			{
public:
	typedef	std::map<uint32_t, bt_peersrc_peer_t>	peer_log_t;
private:
	bt_swarm_t *			bt_swarm;	//!< backpointer to the next
	bt_peersrc_utpex_profile_t	profile;
	uint32_t			next_seqnb;

	/************** Internal function	*******************************/
	bool		parsing_error(const std::string &reason)	throw();
	
	/**************	log stuff	***************************************/
	peer_log_t	log_opened;	//!< the log of the opened event
	peer_log_t	log_closed;	//!< the log of the closed event
	uint32_t	get_log_main(uint32_t from_seqnb, bt_peersrc_peer_arr_t &opened_arr
					, bt_peersrc_peer_arr_t &closed_arr)	const throw();
	uint32_t	get_log_init(uint32_t from_seqnb, bt_peersrc_peer_arr_t &opened_arr
					, bt_peersrc_peer_arr_t &closed_arr)	const throw();
	uint32_t	get_log_diff(uint32_t from_seqnb, bt_peersrc_peer_arr_t &opened_arr
					, bt_peersrc_peer_arr_t &closed_arr)	const throw();
	void		purge_log_if_needed()						throw();

	/*************** bt_peersrc_utpex_cnx_t	*******************************/
	std::list<bt_peersrc_utpex_cnx_t *>	cnx_db;
	void cnx_dolink(bt_peersrc_utpex_cnx_t *cnx) 	throw();
	void cnx_unlink(bt_peersrc_utpex_cnx_t *cnx)	throw();
	bool		parse_pkt(bt_peersrc_utpex_cnx_t *utpex_cnx
				, const bt_utmsgtype_t &bt_utmsgtype, const pkt_t &pkt)	throw();
	void		notify_cnx_closed(bt_peersrc_utpex_cnx_t *utpex_cnx)	throw();

	/*************** callback stuff	***************************************/
	bt_peersrc_cb_t*peersrc_cb;	//!< callback used to notify peersrc result
	bt_utmsg_cb_t *	utmsg_cb;	//!< callback used to notify peersrc result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_peersrc_cb(const bt_peersrc_event_t &event)		throw();
	bool		notify_utmsg_cb(const bt_utmsg_event_t &event)			throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_peersrc_utpex_t() 		throw();
	~bt_peersrc_utpex_t()		throw();
	
	/*************** setup function	***************************************/
	bt_peersrc_utpex_t&set_profile(const bt_peersrc_utpex_profile_t &profile)	throw();	
	bt_err_t	start(bt_swarm_t *bt_swarm, bt_peersrc_cb_t *peersrc_cb
				, bt_utmsg_cb_t *utmsg_cb, void *userptr)		throw();

	/*************** Query function	***************************************/
	bt_swarm_t *	get_swarm()		const throw()	{ return bt_swarm;	}

	/*************** bt_peersrc_vapi_t	*******************************/
	size_t		nb_seeder()		const throw()	{ return 0;	}
	size_t		nb_leecher()		const throw()	{ return 0;	}

	/*************** bt_utmsg_vapi_t	*******************************/
	bt_utmsgtype_t	utmsgtype()		const throw()	{ return bt_utmsgtype_t::UTPEX;	}
	std::string	utmsgstr()		const throw()	{ return "ut_pex";		}
	bt_utmsg_cnx_vapi_t *cnx_ctor(bt_swarm_full_utmsg_t *full_utmsg)	throw();

	/*************** List of friend class	*******************************/
	friend class	bt_peersrc_utpex_wikidbg_t;
	friend class	bt_peersrc_utpex_cnx_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PEERSRC_UTPEX_HPP__  */



