/*! \file
    \brief Header of the \ref bt_oload0_t
    
*/


#ifndef __NEOIP_BT_OLOAD0_HPP__ 
#define __NEOIP_BT_OLOAD0_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_oload0_wikidbg.hpp"
#include "neoip_file_path.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_oload0_swarm_t;
class	bt_oload0_herr_t;
class	bt_oload0_mlink_t;
class	bt_oload0_stat_t;
class	bt_oload0_torr_t;
class	bt_httpo_t;
class	http_listener_t;
class	http_uri_t;
class	bt_session_t;
class	udp_vresp_t;
class	kad_listener_t;
class	kad_peer_t;
class	clineopt_arr_t;

/** \brief Handle the http offload capability
 */
class bt_oload0_t : NEOIP_COPY_CTOR_DENY, private wikidbg_obj_t<bt_oload0_t, bt_oload0_wikidbg_init> {
private:
	http_listener_t *	http_listener;		//!< the http_listener_t to receive external http

	bt_oload0_herr_t *	oload0_herr;
	bt_httpo_t *		bt_httpo;

	bt_oload0_mlink_t *	oload0_mlink;
	bt_oload0_stat_t *	oload0_stat;
	bt_oload0_torr_t *	oload0_torr;

// TODO to port on top of bt_ezsession_t - look at neoip-get for that
	bt_session_t *		bt_session;

	/*************** stuff for bt_peersrc_kad_t	***********************/
	udp_vresp_t *		udp_vresp;	//!< the core udp_vresp_t listening on a ipport_addr_t
	kad_listener_t *	kad_listener;	//!< the kad_listener for this ntudp_peer_t 
	kad_peer_t *		kad_peer;	//!< the kad_peer_t for the underlying NS

	/*************** bt_oload0_swarm_t	*******************************/
	std::list<bt_oload0_swarm_t *>		oload0_swarm_db;
	void oload0_swarm_dolink(bt_oload0_swarm_t *swarm)throw()	{ oload0_swarm_db.push_back(swarm);}
	void oload0_swarm_unlink(bt_oload0_swarm_t *swarm)throw()	{ oload0_swarm_db.remove(swarm);	}
public:
	/*************** ctor/dtor	***************************************/
	bt_oload0_t()		throw();
	~bt_oload0_t()		throw();
	
	/*************** Setup function	***************************************/
	bt_err_t	start()		throw();
	
	/*************** Query function	***************************************/
	bt_httpo_t *		get_bt_httpo()		const throw()	{ return bt_httpo;		}
	bt_session_t *		get_bt_session()	const throw()	{ return bt_session;		}
	kad_peer_t *		get_kad_peer()		const throw()	{ return kad_peer;		}
	bt_oload0_herr_t *	get_oload0_herr()	const throw()	{ return oload0_herr;		}
	file_path_t		io_pfile_dirpath()	const throw();
	bt_oload0_swarm_t *	swarm_by_nested_uri(const http_uri_t &nested_uri)	const throw();
	static clineopt_arr_t	clineopt_arr()		throw();

	/*************** List of friend class	*******************************/
	friend class	bt_oload0_wikidbg_t;	
	friend class	bt_oload0_swarm_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_OLOAD0_HPP__  */










