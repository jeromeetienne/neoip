	/*! \file
    \brief Header of the bt_session_t
    
*/


#ifndef __NEOIP_BT_SESSION_HPP__ 
#define __NEOIP_BT_SESSION_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_session_wikidbg.hpp"
#include "neoip_bt_session_profile.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_id.hpp"
#include "neoip_socket_resp_cb.hpp"
#include "neoip_ipport_aview.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_t;
class	bt_session_cnx_t;
class	bt_session_http_t;
class	socket_itor_arg_t;
class	socket_resp_arg_t;
class	bt_peersrc_peer_t;

/** \brief class definition for bt_session
 */
class bt_session_t : NEOIP_COPY_CTOR_DENY, private socket_resp_cb_t
					, private wikidbg_obj_t<bt_session_t, bt_session_wikidbg_init>
					{
private:
	bt_id_t			m_local_peerid;		//!< the local peerid
	bool			is_tcp_inetreach;	//!< true if bt_session_t has responded to a tcp 
							//!< connection from a public ip address.
	ipport_aview_t		m_listen_aview;		//!< the local view of the listen_ipport
	bt_session_profile_t	m_profile;

	/*************** store the bt_swarm_t	*******************************/
	std::list<bt_swarm_t *>		swarm_db;
	void swarm_dolink(bt_swarm_t *swarm) 	throw()	{ swarm_db.push_back(swarm);	}
	void swarm_unlink(bt_swarm_t *swarm)	throw()	{ swarm_db.remove(swarm);	}
	bt_swarm_t *	swarm_by_infohash(const bt_id_t &infohash)	throw();

	/*************** store the bt_session_cnx_t	*******************************/
	std::list<bt_session_cnx_t *>	cnx_db;
	void cnx_dolink(bt_session_cnx_t *cnx) 	throw()	{ cnx_db.push_back(cnx);	}
	void cnx_unlink(bt_session_cnx_t *cnx)	throw()	{ cnx_db.remove(cnx);		}

	/*************** socket_resp_t	***************************************/
	socket_resp_t *	socket_resp;
	bool		neoip_socket_resp_event_cb(void *userptr, socket_resp_t &cb_socket_resp
					, const socket_event_t &socket_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_session_t() 		throw();
	~bt_session_t()		throw();
	
	/*************** setup function	***************************************/
	bt_session_t&	profile(const bt_session_profile_t &profile)		throw();
	bt_err_t	start(const ipport_aview_t &m_listen_aview
				, const bt_id_t &p_local_peerid = bt_id_t())	throw();
	
	/*************** query function	***************************************/
	const bt_session_profile_t& profile()		const throw()	{ return m_profile;		}
	const bt_id_t &		local_peerid()		const throw()	{ return m_local_peerid;	}
	const ipport_aview_t &	listen_aview()		const throw()	{ return m_listen_aview;	}
	const ipport_addr_t &	listen_lview()		const throw()	{ return listen_aview().lview();}
	const ipport_addr_t &	listen_pview()		const throw()	{ return listen_aview().pview();}

	/*************** action function	*******************************/
	void		update_listen_pview(const ipport_addr_t &new_listen_pview)	throw();
	
	/*************** List of friend class	*******************************/
	friend class	bt_session_cnx_t;
	friend class	bt_session_wikidbg_t;
	friend class	bt_swarm_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SESSION_HPP__  */



