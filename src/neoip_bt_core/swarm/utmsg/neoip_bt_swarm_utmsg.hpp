/*! \file
    \brief Header of the bt_swarm_utmsg_t
    
*/


#ifndef __NEOIP_BT_SWARM_UTMSG_HPP__ 
#define __NEOIP_BT_SWARM_UTMSG_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_swarm_utmsg_wikidbg.hpp"
#include "neoip_bt_utmsg_cb.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_t;
class	bt_utmsg_vapi_t;
class	bt_utmsgtype_t;
class	bt_swarm_full_utmsg_t;

class	bt_utmsg_piecewish_t;

/** \brief class definition for bt_swarm_utmsg_t
 */
class bt_swarm_utmsg_t : NEOIP_COPY_CTOR_DENY, public bt_utmsg_cb_t
				, private wikidbg_obj_t<bt_swarm_utmsg_t, bt_swarm_utmsg_wikidbg_init>
				{
public:
	typedef std::list<bt_utmsg_vapi_t *>	utmsg_vapi_db_t;
private:
	bt_swarm_t *	m_bt_swarm;		//!< backpointer to the linked bt_swarm_t

	/*************** bt_utmsg_vapi_t	*******************************/
	utmsg_vapi_db_t	m_utmsg_vapi_db;
	bool	 	neoip_bt_utmsg_cb(void *cb_userptr, bt_utmsg_vapi_t &cb_utmsg_vapi
					, const bt_utmsg_event_t &utmsg_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_utmsg_t() 		throw();
	~bt_swarm_utmsg_t()		throw();
	
	/*************** setup function	***************************************/
	bt_err_t	start(bt_swarm_t *m_bt_swarm)	throw();

	/*************** query function	***************************************/
	bt_swarm_t *		bt_swarm()	const throw()	{ return m_bt_swarm;	}
	const utmsg_vapi_db_t &	utmsg_vapi_db()	const throw()	{ return m_utmsg_vapi_db;}
	bt_utmsg_vapi_t *	utmsg_vapi(const bt_utmsgtype_t &bt_utmsgtype)	const throw();
	
	/*************** query helper function	*******************************/
	bt_utmsg_piecewish_t *	utmsg_piecewish()	const throw();

	/*************** List of friend class	*******************************/
	friend class	bt_swarm_utmsg_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_UTMSG_HPP__  */



