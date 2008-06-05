/*! \file
    \brief Header of the bt_utmsg_fstart_t

*/


#ifndef __NEOIP_BT_UTMSG_FSTART_HPP__ 
#define __NEOIP_BT_UTMSG_FSTART_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_bt_utmsg_fstart_wikidbg.hpp"
#include "neoip_bt_utmsg_vapi.hpp"
#include "neoip_bt_utmsg_cb.hpp"
#include "neoip_bt_utmsgtype.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_t;
class	bt_utmsg_fstart_cnx_t;
class	bt_cmd_t;

/** \brief class definition for bt_peersrc for http
 * 
 * - it MUST be ctor after bt_swarm_t and dtor before
 */
class bt_utmsg_fstart_t : NEOIP_COPY_CTOR_DENY, public bt_utmsg_vapi_t
			, private wikidbg_obj_t<bt_utmsg_fstart_t, bt_utmsg_fstart_wikidbg_init, bt_utmsg_vapi_t>
			{
private:
	bt_swarm_t *	bt_swarm;	//!< backpointer to the next
	bool		m_local_fstart;	//!< true if the local peer is in faststart, false otherwise

	/*************** Internalfunction	*******************************/
	bt_cmd_t	generate_xmit_cmd()		throw();

	/*************** bt_utmsg_fstart_cnx_t	*******************************/
	std::list<bt_utmsg_fstart_cnx_t *>	cnx_db;
	void cnx_dolink(bt_utmsg_fstart_cnx_t *cnx) 	throw()	{ cnx_db.push_back(cnx);}
	void cnx_unlink(bt_utmsg_fstart_cnx_t *cnx)	throw()	{ cnx_db.remove(cnx);	}
	
	/*************** callback stuff	***************************************/
	bt_utmsg_cb_t *	utmsg_cb;	//!< callback used to notify peersrc result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_utmsg_cb(const bt_utmsg_event_t &event)		throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_utmsg_fstart_t() 		throw();
	~bt_utmsg_fstart_t()		throw();
	
	/*************** setup function	***************************************/
	bt_err_t	start(bt_swarm_t *bt_swarm, bt_utmsg_cb_t *utmsg_cb
							, void *userptr)	throw();

	/*************** Query function	***************************************/
	bt_swarm_t *	get_swarm()	const throw()	{ return bt_swarm;	}
	bool		local_fstart()	const throw()	{ return m_local_fstart;}
	
	/*************** Action function	*******************************/
	void		local_fstart(bool new_value)	throw();

	/*************** bt_utmsg_vapi_t	*******************************/
	bt_utmsgtype_t	utmsgtype()		const throw()	{ return bt_utmsgtype_t::FSTART;	}
	std::string	utmsgstr()		const throw();
	bt_utmsg_cnx_vapi_t *	cnx_ctor(bt_swarm_full_utmsg_t *full_utmsg)		throw();

	/*************** List of friend class	*******************************/
	friend class	bt_utmsg_fstart_wikidbg_t;
	friend class	bt_utmsg_fstart_cnx_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_UTMSG_FSTART_HPP__  */



