/*! \file
    \brief Header of the \ref bt_swarm_event_t
*/


#ifndef __NEOIP_BT_SWARM_EVENT_HPP__ 
#define __NEOIP_BT_SWARM_EVENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_bt_err.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \ref class to store the event from bt_swarm_t
 */
class bt_swarm_event_t: NEOIP_COPY_CTOR_ALLOW {
public:
	enum type {
		NONE,
		PIECE_NEWLY_AVAIL,
		DISK_ERROR,
		MAX
	};
private:
	bt_swarm_event_t::type	type_val;

	// all event parameters
	size_t		piece_idx;
	bt_err_t	bt_err;
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_event_t()		throw();
	~bt_swarm_event_t()	throw();
	
	/*************** query function	***************************************/
	bt_swarm_event_t::type	get_value()	const throw()	{ return type_val;		}
	bool			is_fatal()	const throw()	{ return is_disk_error();	}

	/*************** specific event ctor/dtor/query	***********************/
	bool			is_piece_newly_avail() 				const throw();
	static bt_swarm_event_t	build_piece_newly_avail(size_t piece_idx)	throw();
	size_t			get_piece_newly_avail()				const throw();

	bool			is_disk_error() 				const throw();
	static bt_swarm_event_t	build_disk_error(const bt_err_t &bt_err)	throw();
	const bt_err_t &	get_disk_error()				const throw();
	
	/*************** display function	*******************************/
	std::string		to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const bt_swarm_event_t &event) throw()
					{ return oss << event.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_EVENT_HPP__  */



