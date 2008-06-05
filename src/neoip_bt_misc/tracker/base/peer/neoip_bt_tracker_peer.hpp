/*! \file
    \brief Header of the \ref bt_tracker_peer_t
    
*/


#ifndef __NEOIP_BT_TRACKER_PEER_HPP__ 
#define __NEOIP_BT_TRACKER_PEER_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_id.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for peer for bt_tracker_client_t
 */
class bt_tracker_peer_t : NEOIP_COPY_CTOR_ALLOW {
private:
	ipport_addr_t	m_ipport;		//!< the ip:port of the kad peer
	bt_id_t		m_peerid;		//!< the peerid of the kad peer
	bool		m_is_seed;		//!< true if this peer is seed
	bool		m_want_jamstd;		//!< true if this peer want jamstd
public:
	/*************** ctor/dtor	***************************************/
	bt_tracker_peer_t()							throw() {}
	bt_tracker_peer_t(const ipport_addr_t &m_ipport, const bt_id_t &m_peerid
			, bool m_is_seed = false, bool m_want_jamstd = false)	throw();
	
	/*************** comparison operator	*******************************/
	int	compare(const bt_tracker_peer_t &other)	const throw();
	bool 	operator == (const bt_tracker_peer_t & other)	const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const bt_tracker_peer_t & other)	const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const bt_tracker_peer_t & other)	const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const bt_tracker_peer_t & other)	const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const bt_tracker_peer_t & other)	const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const bt_tracker_peer_t & other)	const throw()	{ return compare(other) >= 0;	}
	
	/*************** query function		*******************************/	
	bool			is_null()	const throw() { return m_ipport.is_null();	}
	const ipport_addr_t &	ipport()	const throw() { return m_ipport;		}
	const bt_id_t &		peerid()	const throw() { return m_peerid;		}
	bool			want_jamstd()	const throw() { return m_want_jamstd;		}
	bool			is_seed()	const throw() { return m_is_seed;		}
	
	/*************** Compatibility function	*******************************/
	const ipport_addr_t &	get_ipport()	const throw() { return ipport();	}
	const bt_id_t &		get_peerid()	const throw() { return peerid();	}
	
	/*************** display function	*******************************/	
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const bt_tracker_peer_t &bt_tracker_peer)	throw()
				{ return os << bt_tracker_peer.to_string();	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_TRACKER_PEER_HPP__  */










