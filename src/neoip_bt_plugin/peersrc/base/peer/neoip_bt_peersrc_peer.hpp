/*! \file
    \brief Header of the \ref bt_peersrc_peer_t

*/


#ifndef __NEOIP_BT_PEERSRC_PEER_HPP__ 
#define __NEOIP_BT_PEERSRC_PEER_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_socket_addr.hpp"
#include "neoip_bt_id.hpp"
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward definition
class	bt_tracker_peer_t;

/** \brief to store the parameters replied by a tracker
 */
class bt_peersrc_peer_t : NEOIP_COPY_CTOR_ALLOW {
private:
public:
	/*************** ctor/dtor	***************************************/
	bt_peersrc_peer_t()	throw()	{}
	bt_peersrc_peer_t(const bt_id_t &peerid, const socket_addr_t &dest_addr
				, bool is_seed = false, bool want_jamstd=false)	throw();
	bt_peersrc_peer_t(const bt_tracker_peer_t &tracker_peer)		throw();
	
	/*************** query function	***************************************/
	bool		is_null()		const throw();

	/*************** Comparison operator	*******************************/
	int	compare(const bt_peersrc_peer_t &other)		const throw();
	bool 	operator == (const bt_peersrc_peer_t & other)	const throw() { return compare(other) == 0;	}
	bool 	operator != (const bt_peersrc_peer_t & other)	const throw() { return compare(other) != 0;	}
	bool 	operator <  (const bt_peersrc_peer_t & other)	const throw() { return compare(other) <  0;	}
	bool 	operator <= (const bt_peersrc_peer_t & other)	const throw() { return compare(other) <= 0;	}
	bool 	operator >  (const bt_peersrc_peer_t & other)	const throw() { return compare(other) >  0;	}
	bool 	operator >= (const bt_peersrc_peer_t & other)	const throw() { return compare(other) >= 0;	}

	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	bt_peersrc_peer_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( bt_id_t		, peerid);	
	RES_VAR_DIRECT( socket_addr_t	, dest_addr);
	RES_VAR_DIRECT( bool		, is_seed);
	RES_VAR_DIRECT( bool		, want_jamstd);

	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const bt_peersrc_peer_t &peersrc_peer ) throw()
						{ return os << peersrc_peer.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const bt_peersrc_peer_t &handshake)	throw();
	friend	serial_t& operator >> (serial_t & serial, bt_peersrc_peer_t &handshake) 	throw(serial_except_t);	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_PEERSRC_PEER_HPP__  */



