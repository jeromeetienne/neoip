/*! \file
    \brief Header of the bt_swarm_sched_t
    
*/


#ifndef __NEOIP_BT_SWARM_SCHED_HPP__ 
#define __NEOIP_BT_SWARM_SCHED_HPP__ 
/* system include */
/* local include */
#include "neoip_bt_swarm_sched_profile.hpp"
#include "neoip_bt_swarm_sched_wikidbg.hpp"
#include "neoip_bt_pselect_vapi.hpp"
#include "neoip_file_size_inval.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_t;
class	bt_swarm_sched_ecnx_t;
class	bt_swarm_sched_full_t;
class	bt_swarm_sched_piece_t;
class	bt_swarm_sched_cnx_vapi_t;

/** \brief class definition for bt_swarm_sched
 */
class bt_swarm_sched_t : NEOIP_COPY_CTOR_DENY
				, private wikidbg_obj_t<bt_swarm_sched_t, bt_swarm_sched_wikidbg_init>
				{
private:
	bt_swarm_t *		m_bt_swarm;	//!< backpointer to the linked bt_swarm_t
	bt_swarm_sched_profile_t m_profile;	//!< the profile for this scheduler
	bt_swarm_sched_ecnx_t *	m_sched_ecnx;	//!< pointer on the scheduler for bt_swarm_full_t
	bt_swarm_sched_full_t *	m_sched_full;	//!< pointer on the scheduler for bt_ecnx_vapi_t
	size_t			m_next_pidx_hint;//!< if != size_t max, this is an hint for bt_pselect_vapi_t
						//!< it is equal to the pidx wished to be selected, if equ
	
	/*************** piece_t in progress	*******************************/
	typedef	std::map<size_t, bt_swarm_sched_piece_t *>	piece_db_t;
	piece_db_t		piece_do_needreq_db;
	void			piece_do_needreq_dolink(bt_swarm_sched_piece_t *sched_piece) 	throw();
	void			piece_do_needreq_unlink(bt_swarm_sched_piece_t *sched_piece)	throw();
	piece_db_t		piece_no_needreq_db;
	void			piece_no_needreq_dolink(bt_swarm_sched_piece_t *sched_piece) 	throw();
	void			piece_no_needreq_unlink(bt_swarm_sched_piece_t *sched_piece)	throw();

	/*************** internal function	*******************************/
	bt_swarm_sched_piece_t *select_piece_to_request(bt_swarm_sched_cnx_vapi_t *sched_cnx
						, const bt_pieceavail_t &remote_pieceavail
						, size_t pieceidx_hint)		throw();
	bt_swarm_sched_piece_t *select_piece_in_piece_db(const piece_db_t &piece_db
					, const bt_pieceavail_t &remote_pieceavail
					, bt_swarm_sched_cnx_vapi_t *sched_cnx)	throw();
	void			try_send_request_on_idle_cnx()			throw();
	void			restart_timeout_for_allreq(const delay_t &delay)throw();
	void			partavail_piece_ctor(const file_size_inval_t &partavail_piece_inval) throw();
	void			remove_notneeded_pieceprec_sched_piece()	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_sched_t() 		throw();
	~bt_swarm_sched_t()		throw();
	
	/*************** setup function	***************************************/
	bt_swarm_sched_t &	set_profile(const bt_swarm_sched_profile_t &m_profile)	throw();	
	bt_err_t		start(bt_swarm_t *bt_swarm
					, const file_size_inval_t &partavail_piece_inval)throw();

	/*************** query function	***************************************/
	const bt_swarm_sched_profile_t &profile()	const throw()	{ return m_profile;		}
	bt_swarm_t *		bt_swarm()		throw()		{ return m_bt_swarm;		}
	bt_swarm_sched_ecnx_t *	sched_ecnx()		throw()		{ return m_sched_ecnx;		}
	bt_swarm_sched_full_t *	sched_full()		throw()		{ return m_sched_full;		}
	size_t 			next_pidx_hint()	throw()		{ return m_next_pidx_hint;	}
	void 			next_pidx_hint(size_t new_val)	throw()	{ m_next_pidx_hint = new_val;	}
	file_size_inval_t	get_partavail_piece_inval()	const throw();
	bt_swarm_sched_piece_t *piece_by_idx(size_t piece_idx)	const throw();
	bool			in_endgame()			const throw();
	
	/*************** action function	*******************************/
	void			notify_pieceprec_change()	throw();

	/*************** List of friend class	*******************************/
	friend class	bt_swarm_sched_wikidbg_t;
	friend class	bt_swarm_sched_full_t;
	friend class	bt_swarm_sched_ecnx_t;
	friend class	bt_swarm_sched_piece_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_SCHED_HPP__  */



