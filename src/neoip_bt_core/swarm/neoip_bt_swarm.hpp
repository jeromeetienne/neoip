/*! \file
    \brief Header of the bt_swarm_t
    
*/


#ifndef __NEOIP_BT_SWARM_HPP__ 
#define __NEOIP_BT_SWARM_HPP__ 
/* system include */
#include <list>
#include <map>
/* local include */
#include "neoip_bt_swarm_cb.hpp"
#include "neoip_bt_swarm_wikidbg.hpp"
#include "neoip_bt_swarm_profile.hpp"
#include "neoip_bt_swarm_stats.hpp"
#include "neoip_bt_pieceavail.hpp"	// TODO remove this include
#include "neoip_bt_pselect_vapi.hpp"	// TODO convert it in forward declaration
#include "neoip_bt_err.hpp"
#include "neoip_bt_mfile.hpp"
#include "neoip_file_size.hpp"		// TODO why is this include there
#include "neoip_slotpool_id.hpp"	// TODO why is this include there
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_session_t;
class	bt_io_vapi_t;
class	bt_swarm_full_t;
class	bt_swarm_peersrc_t;
class	bt_swarm_utmsg_t;
class	bt_swarm_sched_t;
class	bt_swarm_resumedata_t;
class	bt_handshake_t;
class	bt_ecnx_vapi_t;
class	socket_addr_t;
class	ipport_addr_t;


/** \brief class definition for bt_swarm
 */
class bt_swarm_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<bt_swarm_t, bt_swarm_wikidbg_init> {
private:
	bt_session_t *		bt_session;	//!< backpointer to the linked bt_session_t
	bt_io_vapi_t *		m_io_vapi;	//!< backpointer on the bt_io_vapi_t to use
	bt_pselect_vapi_t *	m_pselect_vapi;	//!< the piece selector to use
	bt_mfile_t		bt_mfile;	//!< the bt_mfile_t of this swarm
	bt_swarm_profile_t	m_profile;	//!< the bt_swarm_profile_t for this bt_swarm_t
	bt_swarm_sched_t *	m_swarm_sched;	//!< pointer on the bt_swarm_sched_t of this bt_swarm_t
	bt_swarm_stats_t	m_swarm_stats;	//!< the statistic of this bt_swarm_t
	
	/*************** peersrc stuff	***************************************/
	bt_swarm_peersrc_t *	m_swarm_peersrc;//!< pointer on the bt_swarm_peersrc_t of this bt_swarm_t

	/*************** utmsg stuff	***************************************/
	bt_swarm_utmsg_t *	m_swarm_utmsg;	//!< pointer on the bt_swarm_utmsg_t of this bt_swarm_t

	/*************** store the bt_swarm_full_t	***********************/
	std::list<bt_swarm_full_t *>	full_db;
	void full_dolink(bt_swarm_full_t *full)	throw()	{ full_db.push_back(full);	}
	void full_unlink(bt_swarm_full_t *full)	throw()	{ full_db.remove(full);		}

	/*************** store the bt_ecnx_vapi_t	***********************/
	typedef	std::map<slot_id_t, bt_ecnx_vapi_t *>	ecnx_vapi_db_t;
	ecnx_vapi_db_t			ecnx_vapi_db;
	void ecnx_vapi_dolink(bt_ecnx_vapi_t *ecnx_vapi)	throw();
	void ecnx_vapi_unlink(bt_ecnx_vapi_t *ecnx_vapi)	throw();
	ecnx_vapi_db_t &		get_ecnx_vapi_db()	throw()	{ return ecnx_vapi_db;	}
	bt_ecnx_vapi_t *		ecnx_vapi_by_object_id(slot_id_t object_slotid)	throw();
	
	/*************** callback stuff	***************************************/
	bt_swarm_cb_t *		callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const bt_swarm_event_t &event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_t() 		throw();
	~bt_swarm_t()		throw();
	
	/*************** setup function	***************************************/
	bt_swarm_t &	set_profile(const bt_swarm_profile_t &profile)			throw();
	bt_err_t	start(bt_session_t *bt_session, const bt_swarm_resumedata_t &swarm_resumedata
					, bt_io_vapi_t *m_io_vapi
					, bt_swarm_cb_t *callback, void *userptr)	throw();					
	bt_err_t	start(bt_session_t *bt_session, const bt_mfile_t &bt_mfile
					, bt_io_vapi_t *m_io_vapi
					, bt_swarm_cb_t *callback, void *userptr)	throw();

	/*************** query function	***************************************/
	bt_session_t *		get_session()	const throw()	{ return bt_session;	}
	const bt_mfile_t &	get_mfile()	const throw()	{ return bt_mfile;	}
	const bt_swarm_profile_t &profile()	const throw()	{ return m_profile;	}
	bt_swarm_stats_t &	swarm_stats()	throw()		{ return m_swarm_stats;	}
	const bt_swarm_stats_t &swarm_stats()	const throw()	{ return m_swarm_stats;	}

	bt_io_vapi_t *		io_vapi()	const throw()	{ return m_io_vapi;	}
	bt_pselect_vapi_t *	pselect_vapi()	const throw()	{ return m_pselect_vapi;}
	bt_swarm_sched_t *	swarm_sched()	const throw()	{ return m_swarm_sched;	}
	bt_swarm_peersrc_t *	swarm_peersrc()	const throw()	{ return m_swarm_peersrc;}
	bt_swarm_utmsg_t *	swarm_utmsg()	const throw()	{ return m_swarm_utmsg;	}

	const bt_pieceavail_t &	local_pavail()	const throw();
	bool			is_seed()	const throw()	{ return local_pavail().is_fully_avail();	}
	bool			is_leech()	const throw()	{ return !is_seed();	}

	double			recv_rate()		const throw();
	double			recv_rate_full()	const throw();
	double			recv_rate_ecnx()	const throw();
	double			xmit_rate()		const throw();
	double			xmit_rate_full()	const throw();
	file_size_t		totfile_anyavail()	const throw();

	const bt_id_t &		local_peerid()		const throw();
	bt_swarm_resumedata_t	get_current_resumedata()const throw();
	bt_handshake_t		get_handshake()		const throw();

	/*************** bt connection stuff	*******************************/
	std::list<bt_swarm_full_t *> &		get_full_db()	throw()		{ return full_db;	}
	const std::list<bt_swarm_full_t *> &	get_full_db()	const throw()	{ return full_db;	}
	bt_swarm_full_t	*	full_by_remote_addr(const socket_addr_t &remote_addr)	throw();
	bt_swarm_full_t	*	full_by_remote_peerid(const bt_id_t &remote_peerid)	throw();
	bool			is_new_full_allowed()					const throw();

	/*************** action function	*******************************/
	void		notify_pieceprec_change()					throw();
	bool		declare_piece_newly_avail(size_t piece_idx)			throw();
	bt_swarm_t &	declare_piece_nomore_avail(size_t piece_idx)			throw();
	bool		notify_disk_error(const bt_err_t &bt_err)			throw();
	void		update_listen_pview(const ipport_addr_t &new_listen_pview)	throw();
	
	/*************** List of friend class	*******************************/
	friend class	bt_swarm_wikidbg_t;
	friend class	bt_swarm_itor_t;
	friend class	bt_swarm_full_t;
	friend class	bt_swarm_sched_ecnx_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_HPP__  */



