/*! \file
    \brief Header of the bt_swarm_sched_block_t
    
*/


#ifndef __NEOIP_BT_SWARM_SCHED_BLOCK_HPP__ 
#define __NEOIP_BT_SWARM_SCHED_BLOCK_HPP__ 
/* system include */
#include <set>
/* local include */
#include "neoip_bt_swarm_sched_block_wikidbg.hpp"
#include "neoip_bt_io_write_cb.hpp"
#include "neoip_file_range.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	bt_swarm_sched_request_t;
class	bt_swarm_sched_piece_t;
class	bt_swarm_sched_cnx_vapi_t;
class	pkt_t;

/** \brief class definition for bt_swarm_sched
 */
class bt_swarm_sched_block_t : NEOIP_COPY_CTOR_DENY, private bt_io_write_cb_t
				, private wikidbg_obj_t<bt_swarm_sched_block_t, bt_swarm_sched_block_wikidbg_init>
				{
public:
	typedef	std::set<bt_swarm_sched_request_t *> request_db_t;
private:
	bt_swarm_sched_piece_t *sched_piece;	//!< backpointer on the bt_swarm_sched_piece_t
	file_range_t		m_totfile_range;

	/*************** state stuff	***************************************/
	enum state_t {
		STATE_NONE,
		STATE_DO_NEEDREQ,		//!< the block DOES need more bt_swarm_sched_request_t
		STATE_NO_NEEDREQ,		//!< the block DOESNT need more bt_swarm_sched_request_t
		STATE_MAX
	};
	state_t			state_cur;	//!< the current state_t of this bt_swarm_sched_piece_t
	state_t			state_cpu()			const throw();
	void			state_change_if_needed()	throw();
	void			state_goto(state_t new_state)	throw();

	/*************** request_t in progress	*******************************/
	request_db_t		pending_req_db;
	void			pending_req_dolink(bt_swarm_sched_request_t *sched_request) 	throw();
	void			pending_req_unlink(bt_swarm_sched_request_t *sched_request)	throw();
	request_db_t		expired_req_db;
	void			expired_req_dolink(bt_swarm_sched_request_t *sched_request) 	throw();
	void			expired_req_unlink(bt_swarm_sched_request_t *sched_request)	throw();

	/*************** bt_io_write_t	***************************************/
	bt_io_write_t *		bt_io_write;
	bool 			neoip_bt_io_write_cb(void *cb_userptr, bt_io_write_t &cb_io_write
						, const bt_err_t &bt_err)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_swarm_sched_block_t(bt_swarm_sched_piece_t *sched_piece
					, const file_range_t &m_file_range)	 	throw();
	~bt_swarm_sched_block_t()							throw();

	/*************** query function	***************************************/
	size_t			nb_request()	const throw()	{ return pending_req_db.size() + expired_req_db.size();	}
	const file_range_t &	totfile_range()	const throw()	{ return m_totfile_range;	}
	const file_size_t &	totfile_beg()	const throw()	{ return totfile_range().beg();	}
	const file_size_t &	totfile_end()	const throw()	{ return totfile_range().end();	}
	file_size_t 		length()	const throw()	{ return totfile_range().length();}
	bool			is_reqed_by(bt_swarm_sched_cnx_vapi_t *sched_cnx)	const throw();

	/*************** action function	*******************************/
	void		notify_complete_from(bt_swarm_sched_cnx_vapi_t *sched_cnx, datum_t &reply_datum) throw();
	void		restart_timeout_for_allreq(const delay_t &expire_delay)		throw();

	/*************** List of friend class	*******************************/
	friend class	bt_swarm_sched_block_wikidbg_t;	
	friend class	bt_swarm_sched_request_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_SWARM_SCHED_BLOCK_HPP__  */



