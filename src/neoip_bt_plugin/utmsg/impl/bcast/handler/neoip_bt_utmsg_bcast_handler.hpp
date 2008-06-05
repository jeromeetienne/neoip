/*! \file
    \brief Header of the bt_utmsg_bcast_handler_t

*/


#ifndef __NEOIP_BT_UTMSG_BCAST_HANDLER_HPP__ 
#define __NEOIP_BT_UTMSG_BCAST_HANDLER_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_utmsg_bcast_handler_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_pkt.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_t;
class	bt_utmsg_bcast_t;

/** \brief class definition for bt_peersrc for http
 */
class bt_utmsg_bcast_handler_t : NEOIP_COPY_CTOR_DENY, private zerotimer_cb_t {
private:
	bt_utmsg_bcast_t *	m_utmsg_bcast;
	std::string		m_opstr;

	/*************** zerotimer_t	***************************************/
	std::list<pkt_t>	notified_pkt_db;
	zerotimer_t		notify_zerotimer;
	bool			neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)throw();
	
	/*************** callback stuff	***************************************/
	bt_utmsg_bcast_handler_cb_t *callback;	//!< callback used to notify peersrc result
	void *		userptr;		//!< userptr associated with the callback
	bool		notify_callback(pkt_t &recved_pkt)			throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_utmsg_bcast_handler_t() 		throw();
	~bt_utmsg_bcast_handler_t()		throw();
	
	/*************** setup function	***************************************/
	bt_err_t	start(bt_utmsg_bcast_t *m_utmsg_bcast, const std::string &m_opstr
				, bt_utmsg_bcast_handler_cb_t *callback, void *userptr)	throw();
				
	/*************** action function	*******************************/
	void		notify_recved_pkt(pkt_t &recved_pkt)			throw();

	/*************** query function	***************************************/
	const std::string &	opstr()		const throw()	{ return m_opstr;	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_UTMSG_BCAST_HANDLER_HPP__  */



