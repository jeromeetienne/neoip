/*! \file
    \brief Header of the \ref casto_swarm_httpo_t
    
*/


#ifndef __NEOIP_CASTO_SWARM_HTTPO_HPP__ 
#define __NEOIP_CASTO_SWARM_HTTPO_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_casto_swarm_httpo_wikidbg.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_bt_httpo_full_cb.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	casto_swarm_t;
class	bt_cast_mdata_t;
class	file_size_t;

/** \brief Handle the swarm part for the bt_oload_stat_t
 */
class casto_swarm_httpo_t : NEOIP_COPY_CTOR_DENY, private bt_httpo_full_cb_t
			, private wikidbg_obj_t<casto_swarm_httpo_t, casto_swarm_httpo_wikidbg_init, bt_httpo_full_cb_t> {
private:
	casto_swarm_t *		m_casto_swarm;	//!< backpointer to the attached casto_swarm_t

	/*************** Internal function	*******************************/
	bool			autodelete(const std::string &reason = "")	throw();
	const bt_cast_mdata_t &	cast_mdata()					const throw();

	/*************** bt_httpo_full_t	*******************************/
	std::list<bt_httpo_full_t *>	m_httpo_full_db;
	bool			neoip_bt_httpo_full_cb(void *cb_userptr, bt_httpo_full_t &cb_bt_httpo_full
						, const bt_httpo_event_t &httpo_event)	throw();
	void			httpo_full_start_one(bt_httpo_full_t *httpo_full)	throw();
	bt_err_t		httpo_full_do_start(bt_httpo_full_t *httpo_full)	throw();
	file_size_t		httpo_full_initial_pos(bt_httpo_full_t *httpo_full)	throw();
public:
	/*************** ctor/dtor	***************************************/
	casto_swarm_httpo_t()	throw();
	~casto_swarm_httpo_t()	throw();

	/*************** Setup function	***************************************/
	bt_err_t	start(casto_swarm_t *p_casto_swarm)		throw();
	
	/*************** Query function	***************************************/
	casto_swarm_t *				casto_swarm()	const throw()	{ return m_casto_swarm;	}
	const std::list<bt_httpo_full_t *> &	httpo_full_db()	const throw()	{ return m_httpo_full_db;	}

	/*************** Action function	*******************************/
	void		httpo_full_push(bt_httpo_full_t *httpo_full)	throw();
	bool		handle_piece_newly_avail(size_t pieceidx)	throw();
	void		httpo_full_db_dtor(const std::string &reason)	throw();
	void		httpo_full_start_all()				throw();
	void		httpo_full_resync_if_needed()			throw();
	void		httpo_full_check_overrun()			throw();

	/*************** List of friend class	*******************************/
	friend class	casto_swarm_httpo_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CASTO_SWARM_HTTPO_HPP__ */










