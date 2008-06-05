/*! \file
    \brief Header of the bt_relay_t
    
*/


#ifndef __NEOIP_BT_RELAY_HPP__ 
#define __NEOIP_BT_RELAY_HPP__ 
/* system include */
#include <set>
/* local include */
#include "neoip_bt_relay_profile.hpp"
#include "neoip_bt_relay_wikidbg.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declarations
class	bt_swarm_t;

/** \brief class definition for bt_relay_t
 */
class bt_relay_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t
			, private wikidbg_obj_t<bt_relay_t, bt_relay_wikidbg_init> {
private:
	bt_relay_profile_t	m_profile;	//!< the profile to use for this bt_relay_t

	/************** swarmpiece_db	***************************************/
	//! swarmpiece_t holds the information for a given piece in a given bt_swarm_t
	struct swarmpiece_t {
		bt_swarm_t *	bt_swarm;	//!< the bt_swarm_t for this piece 
		size_t		pieceidx;	//!< the index for this piece
		double		gainexpect;	//!< the gain expectation for this piece
		swarmpiece_t(bt_swarm_t *bt_swarm, size_t pieceidx, double gainexpect)	throw()
			: bt_swarm(bt_swarm), pieceidx(pieceidx), gainexpect(gainexpect) {}
		bool 	operator <  (const swarmpiece_t & other)	const throw()
					{ return gainexpect > other.gainexpect;	}	 
	};
	typedef std::vector<swarmpiece_t>	swarmpiece_db_t;
	void	swarmpiece_db_eval()					throw();
	void	swarmpiece_db_build(swarmpiece_db_t &swarmpiece_db)	throw();
	void	swarmpiece_db_sort(swarmpiece_db_t &swarmpiece_db)	throw();
	void	swarmpiece_db_process(swarmpiece_db_t &swarmpiece_db)	throw();
	
	/*************** swarm_db	***************************************/
	typedef std::set<bt_swarm_t *>	swarm_db_t;
	swarm_db_t		m_swarm_db;
	const swarm_db_t &	swarm_db()	const throw()	{ return m_swarm_db;	}

	/*************** expire_timeout	***************************************/
	delaygen_t		eval_delaygen;	//!< the delay_t generator for retrying 	
	timeout_t		eval_timeout;
	bool 			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	/*************** ctor/dtor	***************************************/
	bt_relay_t() 		throw();
	~bt_relay_t()		throw();
	
	/*************** setup function	***************************************/
	bt_relay_t&	profile(const bt_relay_profile_t &profile)		throw();
	bt_err_t	start()	throw();
	
	/*************** query function	***************************************/
	const bt_relay_profile_t &profile()	const throw()	{ return m_profile;	}

	/*************** Action function	*******************************/
	void 		swarm_dolink(bt_swarm_t *bt_swarm)	throw();
	void 		swarm_unlink(bt_swarm_t *bt_swarm)	throw();
	void		reevaluate_now()			throw()	{ swarmpiece_db_eval();	}

	/*************** function to compute the gainexpect	***************/	
	static size_t	remote_pwish(bt_swarm_t *bt_swarm, size_t pieceidx)	throw();
	static size_t	remote_pavail(bt_swarm_t *bt_swarm, size_t pieceidx)	throw();
	static size_t	piecelen(bt_swarm_t *bt_swarm, size_t pieceidx)		throw();
	static size_t	pieceavaillen(bt_swarm_t *bt_swarm, size_t pieceidx)	throw();
	static size_t	takecost(bt_swarm_t *bt_swarm, size_t pieceidx)		throw();
	static double	giveexpect(bt_swarm_t *bt_swarm, size_t pieceidx)	throw();
	static double	gainexpect(bt_swarm_t *bt_swarm, size_t pieceidx)	throw();

	/*************** List of friend class	*******************************/
	friend class	bt_relay_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_RELAY_HPP__  */



