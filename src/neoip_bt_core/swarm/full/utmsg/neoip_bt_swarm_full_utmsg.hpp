/*! \file
    \brief Header of the bt_swarm_full_t
    
*/


#ifndef __NEOIP_BT_SWARM_FULL_UTMSG_HPP__ 
#define __NEOIP_BT_SWARM_FULL_UTMSG_HPP__ 
/* system include */
#include <map>
/* local include */
#include "neoip_bt_swarm_full_utmsg_wikidbg.hpp"
#include "neoip_bt_utmsgtype.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_utmsg_cnx_vapi_t;
class	bt_swarm_full_t;
class	bt_peersrc_peer_t;
class	bt_err_t;
class	pkt_t;

/** \brief class definition for bt_swarm_full_t::utmsg_t
 */
class bt_swarm_full_utmsg_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<bt_swarm_full_utmsg_t, bt_swarm_full_utmsg_wikidbg_init>
			{
private:
	bt_swarm_full_t*swarm_full;		//!< the bt_swarm_full_t to which it is attached
	bool		notified_opened;	//!< true if this connection has been notified opened
	
	/*************** handshake fields	*******************************/
	std::string	remote_prog_version;
	ipport_addr_t	remote_tcp_listen_ipport;

	typedef	std::map<bt_utmsgtype_t, uint8_t>		convtable_db_t;
	typedef	std::map<bt_utmsgtype_t, bt_utmsg_cnx_vapi_t *> cnx_vapi_db_t;
	convtable_db_t	convtable_db;
	cnx_vapi_db_t	cnx_vapi_db;	//bt_utmsg_cnx_vapi_t link *ITSELF* in this database

	/*************** Internal function	*******************************/
	datum_t		generate_handshake()				const throw();
	bool		parsing_error(const std::string &reason)	throw();
	
	/*************** bt_utmsgtype_t parsing	*******************************/
	bool		parse_handshake(pkt_t &pkt)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_full_utmsg_t(bt_swarm_full_t *swarm_full)throw();
	~bt_swarm_full_utmsg_t()			throw();

	/*************** query function		*******************************/
	const std::string &	prog_version()		const throw()	{ return remote_prog_version;	}
	const ipport_addr_t &	tcp_listen_ipport()	const throw()	{ return remote_tcp_listen_ipport;}
	bt_peersrc_peer_t	to_peersrc_peer()	const throw();
	bt_swarm_full_t *	get_swarm_full()	throw()		{ return swarm_full;		}
	bool			do_support(const bt_utmsgtype_t &utmsgtype) 	
							const throw()	{ return cnx_vapi(utmsgtype);	}
	bool			no_support(const bt_utmsgtype_t &utmsgtype)
							const throw()	{ return !do_support(utmsgtype);} 	
	/*************** action function	*******************************/
	bool		parse_utmsg_payl(pkt_t &pkt)	throw();
	bt_err_t	send_handshake()		throw();
	
	bt_utmsg_cnx_vapi_t *cnx_vapi(const bt_utmsgtype_t &bt_utmsgtype)	const throw();
	// NOTE: those function are used by bt_utmsg_cnx_vapi_t and MUST be public
	// - im not sure why, doing a friend class bt_utmg_cnx_vapi_t doesnt work
	void		cnx_vapi_dolink(bt_utmsg_cnx_vapi_t *cnx_vapi)	throw();
	void		cnx_vapi_unlink(bt_utmsg_cnx_vapi_t *cnx_vapi)	throw();
	
	/*************** List of friend class	*******************************/
	friend class	bt_swarm_full_utmsg_wikidbg_t;
	friend class	bt_swarm_full_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_FULL_UTMSG_HPP__  */



