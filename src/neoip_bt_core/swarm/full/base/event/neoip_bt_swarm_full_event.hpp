/*! \file
    \brief Header of the \ref bt_swarm_full_event_t
*/


#ifndef __NEOIP_BT_SWARM_FULL_EVENT_HPP__ 
#define __NEOIP_BT_SWARM_FULL_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_bt_cmd.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	pkt_t;
class	bitfield_t;

/** \ref class to store the event from bt_swarm_full_t
 */
class bt_swarm_full_event_t {
public:
	enum type {
		NONE,
		CNX_OPENED,		//!< remote peer connection will be opened immediatly after
		CNX_CLOSED,		//!< remote peer connection will be closed immediatly after
		REQAUTH_TIMEDOUT,	//!< when the request authorisation timedout
		UNAUTH_REQ,		//!< this bt_cmdtype_t has just been received from remote peer
		DOAUTH_REQ,		//!< this bt_cmdtype_t has just been received from remote peer
		UNWANT_REQ,		//!< this bt_cmdtype_t has just been received from remote peer
		DOWANT_REQ,		//!< this bt_cmdtype_t has just been received from remote peer
		PIECE_ISAVAIL,		//!< this bt_cmdtype_t has just been received from remote peer
		PIECE_UNAVAIL,		//!< this bt_cmdtype_t has just been received from remote peer
		PIECE_BFIELD,		//!< this bt_cmdtype_t has just been received from remote peer
		BLOCK_REP,		//!< this bt_cmdtype_t has just been received from remote peer
		PWISH_DOINDEX,		//!< this bt_cmdtype_t has just been received from remote peer
		PWISH_NOINDEX,		//!< this bt_cmdtype_t has just been received from remote peer
		PWISH_DOFIELD,		//!< this bt_cmdtype_t has just been received from remote peer
		MAX
	};
private:
	bt_swarm_full_event_t::type	type_val;

	// all event parameters
	bt_cmd_t		bt_cmd;
	pkt_t *			pkt;		//!< for BLOCK_REP
	size_t			piece_idx;	//!< for PIECE_ISAVAIL/PIECE_UNAVAIL/PWISH_DOINDEX/PWISH_NOINDEX
	const bitfield_t *	old_bitfield;	
	const bitfield_t *	new_bitfield;	
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_full_event_t()		throw();
	~bt_swarm_full_event_t()	throw();
	
	/*************** query function	***************************************/
	bt_swarm_full_event_t::type	get_value()	const throw()	{ return type_val;	}

	/*************** specific event ctor/dtor/query	***********************/
	bool				is_cnx_opened() 				const throw();
	static bt_swarm_full_event_t	build_cnx_opened()				throw();

	bool				is_cnx_closed() 				const throw();
	static bt_swarm_full_event_t	build_cnx_closed()				throw();
	
	bool				is_reqauth_timedout() 				const throw();
	static bt_swarm_full_event_t	build_reqauth_timedout()			throw();

	bool				is_unauth_req() 				const throw();
	static bt_swarm_full_event_t	build_unauth_req()				throw();
	
	bool				is_doauth_req() 				const throw();
	static bt_swarm_full_event_t	build_doauth_req()				throw();
	
	bool				is_unwant_req() 				const throw();
	static bt_swarm_full_event_t	build_unwant_req()				throw();
	
	bool				is_dowant_req() 				const throw();
	static bt_swarm_full_event_t	build_dowant_req()				throw();
	
	bool				is_piece_isavail() 				const throw();
	static bt_swarm_full_event_t	build_piece_isavail(const size_t &piece_idx)	throw();
	const size_t &			get_piece_isavail()				const throw();

	bool				is_piece_unavail() 				const throw();
	static bt_swarm_full_event_t	build_piece_unavail(const size_t &piece_idx)	throw();
	const size_t &			get_piece_unavail()				const throw();
		
	bool				is_piece_bfield() 				const throw();
	static bt_swarm_full_event_t	build_piece_bfield()				throw();
	
	bool				is_block_rep() 					const throw();
	static bt_swarm_full_event_t	build_block_rep(const bt_cmd_t &bt_cmd, pkt_t *pkt)   throw();
	pkt_t *				get_block_rep(bt_cmd_t &bt_cmd_out)		const throw();

	bool				is_pwish_doindex() 				const throw();
	static bt_swarm_full_event_t	build_pwish_doindex(const size_t &piece_idx)	throw();
	const size_t &			get_pwish_doindex()				const throw();

	bool				is_pwish_noindex() 				const throw();
	static bt_swarm_full_event_t	build_pwish_noindex(const size_t &piece_idx)	throw();
	const size_t &			get_pwish_noindex()				const throw();

	bool				is_pwish_dofield() 				const throw();
	static bt_swarm_full_event_t	build_pwish_dofield(const bitfield_t *old_bitfield
						, const bitfield_t *new_bitfield)	throw();
	const bitfield_t *		get_pwish_dofield(const bitfield_t **old_out)	const throw();
	/*************** display function	*******************************/
	std::string			to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const bt_swarm_full_event_t &event) throw()
					{ return oss << event.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_FULL_EVENT_HPP__  */



