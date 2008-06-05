/*! \file
    \brief Header of the bt_utmsg_bcast_t

*/


#ifndef __NEOIP_BT_UTMSG_BCAST_HPP__ 
#define __NEOIP_BT_UTMSG_BCAST_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_utmsg_bcast_profile.hpp"
#include "neoip_bt_utmsg_bcast_wikidbg.hpp"
#include "neoip_bt_utmsg_bcast_nonce.hpp"
#include "neoip_bt_utmsg_vapi.hpp"
#include "neoip_bt_utmsg_cb.hpp"
#include "neoip_bt_utmsgtype.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_expireset.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_t;
class	bt_utmsg_bcast_cnx_t;
class	bt_utmsg_bcast_handler_t;
class	bt_cmd_t;
class	pkt_t;

/** \brief class definition for bt_utmsg_bcast_t
 * 
 * - it MUST be ctor after bt_swarm_t and dtor before
 */
class bt_utmsg_bcast_t : NEOIP_COPY_CTOR_DENY, public bt_utmsg_vapi_t
			, private wikidbg_obj_t<bt_utmsg_bcast_t, bt_utmsg_bcast_wikidbg_init, bt_utmsg_vapi_t>
			{
public:
	typedef expireset_t<bt_utmsg_bcast_nonce_t>	seen_nonce_db_t;
private:
	bt_swarm_t *			bt_swarm;	//!< backpointer to the next
	bt_utmsg_bcast_profile_t	profile;
	seen_nonce_db_t *		seen_nonce_db;	//!< the sent nonce database

	/*************** Internal functions	*******************************/
	bt_cmd_t	build_cmd(const bt_utmsg_bcast_nonce_t &bcast_nonce
				, const std::string &opstr, const pkt_t &payload)	throw();
	void		forward_cmd(const bt_cmd_t &bt_cmd, bt_utmsg_bcast_cnx_t *src_cnx) throw();
	bt_err_t	parse_pkt(const pkt_t &pkt, bt_utmsg_bcast_cnx_t *bcast_cnx)	throw();

	/*************** bt_utmsg_bcast_cnx_t	*******************************/
	std::list<bt_utmsg_bcast_cnx_t *>	cnx_db;
	void cnx_dolink(bt_utmsg_bcast_cnx_t *cnx) 	throw()	{ cnx_db.push_back(cnx);	}
	void cnx_unlink(bt_utmsg_bcast_cnx_t *cnx)	throw()	{ cnx_db.remove(cnx);		}

	/*************** bt_utmsg_bcast_handler_t	*******************************/
	std::list<bt_utmsg_bcast_handler_t *>	handler_db;
	void handler_dolink(bt_utmsg_bcast_handler_t *handler) 	throw()	{ handler_db.push_back(handler);	}
	void handler_unlink(bt_utmsg_bcast_handler_t *handler)	throw()	{ handler_db.remove(handler);		}
	bt_utmsg_bcast_handler_t * handler_by_opstr(const std::string &opstr)	const throw();
	
	/*************** callback stuff	***************************************/
	bt_utmsg_cb_t *	utmsg_cb;	//!< callback used to notify peersrc result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_utmsg_cb(const bt_utmsg_event_t &event)			throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_utmsg_bcast_t() 		throw();
	~bt_utmsg_bcast_t()		throw();
	
	/*************** setup function	***************************************/
	bt_utmsg_bcast_t&set_profile(const bt_utmsg_bcast_profile_t &profile)	throw();	
	bt_err_t	start(bt_swarm_t *bt_swarm, bt_utmsg_cb_t *utmsg_cb
							, void *userptr)		throw();

	/*************** Query function	***************************************/
	bt_swarm_t *	get_swarm()		const throw()	{ return bt_swarm;	}

	/*************** Action function	*******************************/
	void		originate_pkt(const std::string &opstr, const pkt_t &payload)	throw();

	/*************** bt_utmsg_vapi_t	*******************************/
	bt_utmsgtype_t	utmsgtype()		const throw()	{ return bt_utmsgtype_t::BCAST;	}
	std::string	utmsgstr()		const throw();
	bt_utmsg_cnx_vapi_t *cnx_ctor(bt_swarm_full_utmsg_t *full_utmsg)	throw();

	/*************** List of friend class	*******************************/
	friend class	bt_utmsg_bcast_wikidbg_t;
	friend class	bt_utmsg_bcast_cnx_t;
	friend class	bt_utmsg_bcast_handler_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_UTMSG_BCAST_HPP__  */



