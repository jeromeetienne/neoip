/*! \file
    \brief Header of the \ref bt_ecnx_event_t
*/


#ifndef __NEOIP_BT_ECNX_EVENT_HPP__ 
#define __NEOIP_BT_ECNX_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_bt_peersrc_peer.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_sched_request_t;
class	datum_t;


/** \ref class to store the event from bt_ecnx_vapi_t
 * 
 * - aka all the event reported by the connection external to bt_swarm_t
 */
class bt_ecnx_event_t {
public:
	enum type {
		NONE,
		CNX_OPENED,	//!< remote peer connection will be opened immediatly after
				//!< it MUST be always called on connection contruction
		CNX_CLOSED,	//!< remote peer connection will be closed immediatly after
				//!< it MUST be always called on connection destruction
		BLOCK_REP,	//!< a request have been replied
		MAX
	};
private:
	bt_ecnx_event_t::type	type_val;

	bt_swarm_sched_request_t *	sched_req_ptr;
	datum_t	*			recved_data_ptr;
public:
	/*************** ctor/dtor	***************************************/
	bt_ecnx_event_t()	throw();
	~bt_ecnx_event_t()	throw();
	
	/*************** query function	***************************************/
	bt_ecnx_event_t::type	get_value()	const throw()	{ return type_val;	}

	/*************** specific event ctor/dtor/query	***********************/
	bool			is_cnx_opened() 				const throw();
	static bt_ecnx_event_t	build_cnx_opened()				throw();

	bool			is_cnx_closed() 				const throw();
	static bt_ecnx_event_t	build_cnx_closed()				throw();
		
	bool			is_block_rep() 					const throw();
	static bt_ecnx_event_t	build_block_rep(bt_swarm_sched_request_t *sched_req_ptr
							, datum_t *recved_data_ptr)	throw();
	datum_t *		get_block_rep(bt_swarm_sched_request_t **sched_req_out)	const throw();

	/*************** display function	*******************************/
	std::string			to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const bt_ecnx_event_t &event) throw()
					{ return oss << event.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_ECNX_EVENT_HPP__  */



