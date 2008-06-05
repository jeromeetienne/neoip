/*! \file
    \brief Header of the bt_swarm_sched_piece_t
    
*/


#ifndef __NEOIP_BT_SWARM_SCHED_PIECE_HPP__ 
#define __NEOIP_BT_SWARM_SCHED_PIECE_HPP__ 
/* system include */
#include <vector>
/* local include */
#include "neoip_bt_swarm_sched_piece_wikidbg.hpp"
#include "neoip_bt_piece_cpuhash_cb.hpp"
#include "neoip_file_size_inval.hpp"
#include "neoip_file_range.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_sched_t;
class	bt_swarm_sched_block_t;
class	bt_swarm_sched_cnx_vapi_t;

/** \brief class definition for bt_swarm_sched
 */
class bt_swarm_sched_piece_t : NEOIP_COPY_CTOR_DENY, private bt_piece_cpuhash_cb_t
				, private zerotimer_cb_t
				, private wikidbg_obj_t<bt_swarm_sched_piece_t, bt_swarm_sched_piece_wikidbg_init>
				{
private:
	bt_swarm_sched_t *	m_swarm_sched;	//!< backpointer on the bt_swarm_sched_t
	size_t			m_pieceidx;	//!< the piece_idx of this piece
	file_range_t		totfile_range;	
	file_size_inval_t	isavail_inval;	//!< an interval of the part already available
	file_size_inval_t	nevereq_inval;	//!< an interval of the part never requested
	
	/*************** Internal function	*******************************/
	bool			is_fully_dloaded()		const throw();
	void			fully_dloaded_post_process()	throw();
	
	/*************** state stuff	***************************************/
	enum state_t {
		STATE_NONE,
		STATE_DO_NEEDREQ,		//!< the piece DOES need more bt_swarm_sched_request_t
		STATE_NO_NEEDREQ,		//!< the piece DOESNT need more bt_swarm_sched_request_t
		STATE_MAX
	};
	state_t			state_cur;	//!< the current state_t of this bt_swarm_sched_piece_t
	state_t			state_cpu()			const throw();
	void			state_change_if_needed()	throw();
	void			state_goto(state_t new_state)	throw();

	/*************** bt_swarm_sched_block_t	*******************************/
	typedef std::map<file_size_t, bt_swarm_sched_block_t *>	block_db_t;
	block_db_t		block_do_needreq_db;
	void			block_do_needreq_dolink(bt_swarm_sched_block_t *sched_block)	throw();
	void			block_do_needreq_unlink(bt_swarm_sched_block_t *sched_block)	throw();
	block_db_t		block_no_needreq_db;
	void			block_no_needreq_dolink(bt_swarm_sched_block_t *sched_block)	throw();
	void			block_no_needreq_unlink(bt_swarm_sched_block_t *sched_block)	throw();
	bt_swarm_sched_block_t *get_block_do_needreq(bt_swarm_sched_cnx_vapi_t *sched_cnx)	const throw();

	/*************** zerotimer_t	***************************************/
	zerotimer_t		nohash_zerotimer;
	bool			neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)throw();
	
	/*************** bt_piece_cpuhash_t	***************************************/
	bt_piece_cpuhash_t *	piece_cpuhash;
	bool 			neoip_bt_piece_cpuhash_cb(void *cb_userptr, bt_piece_cpuhash_t &cb_piece_cpuhash
					, const bt_err_t &bt_err, const bt_id_t &piecehash) throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_sched_piece_t(bt_swarm_sched_t *m_swarm_sched, size_t piece_idx
					, const file_size_inval_t &isavail_inval) 	throw();
	~bt_swarm_sched_piece_t()							throw();
	
	/*************** query function	***************************************/
	size_t			pieceidx()		const throw()	{ return m_pieceidx;		}
	bt_swarm_sched_t *	swarm_sched()		const throw()	{ return m_swarm_sched;		}
	const file_size_inval_t&get_isavail_inval()	const throw()	{ return isavail_inval;		}
	file_size_t		partavail_len()		const throw()	{ return isavail_inval.sum();	}
	bt_swarm_sched_block_t *block_by_totfile_beg(const file_size_t &totfile_beg)		const throw();
	bool			may_request_block_thru(bt_swarm_sched_cnx_vapi_t *sched_cnx)	const throw();
	bt_swarm_sched_block_t *get_block_to_request(bt_swarm_sched_cnx_vapi_t *sched_cnx)	throw();

	/*************** action function	*******************************/
	void			notify_block_completion(bt_swarm_sched_block_t *sched_block)	throw();
	void			restart_timeout_for_allreq(const delay_t &expire_delay)		throw();

	/*************** List of friend class	*******************************/
	friend class	bt_swarm_sched_piece_wikidbg_t;
	friend class	bt_swarm_sched_block_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_SCHED_PIECE_HPP__  */



