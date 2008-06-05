/*! \file
    \brief Header of the \ref casti_swarm_udata_t
    
*/


#ifndef __NEOIP_CASTI_SWARM_UDATA_HPP__ 
#define __NEOIP_CASTI_SWARM_UDATA_HPP__ 
/* system include */
/* local include */
#include "neoip_casti_swarm_udata_wikidbg.hpp"
#include "neoip_casti_swarm_udata_profile.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	casti_swarm_t;
class	bt_cast_udata_t;
class	bt_ezswarm_t;

/** \brief Handle the bt_cast_udata/mdata_t for the casti_swarm_t
 */
class casti_swarm_udata_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t
			, private wikidbg_obj_t<casti_swarm_udata_t, casti_swarm_udata_wikidbg_init> {
private:
	casti_swarm_udata_profile_t m_profile;	//!< the profile for this object
	
	casti_swarm_t *	casti_swarm;		//!< backpointer to the attached casti_swarm_t
	size_t		m_last_pieceq_end;	//!< the pieceq_end at the last xmit

	/*************** internal function	*******************************/
	void		xmit_via_utmsg_bcast(const bt_cast_udata_t &cast_udata)	throw();
	bt_ezswarm_t *	bt_ezswarm()						const throw();

	/*************** xmit_timeout	***************************************/
	timeout_t	xmit_timeout;	//!< to periodically send a packet
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	/*************** ctor/dtor	***************************************/
	casti_swarm_udata_t()	throw();
	~casti_swarm_udata_t()	throw();

	/*************** Setup function	***************************************/
	casti_swarm_udata_t& profile(const casti_swarm_udata_profile_t &profile)throw();
	bt_err_t	start(casti_swarm_t *casti_swarm)			throw();
	
	/*************** Query function	***************************************/
	const casti_swarm_udata_profile_t & profile()	const throw()	{ return m_profile;		}
	size_t		last_pieceq_end()		const throw()	{ return m_last_pieceq_end;	}

	/*************** Action function	*******************************/
	void		notify_pieceq_changed()		throw();

	/*************** List of friend class	*******************************/
	friend class	casti_swarm_udata_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CASTI_SWARM_UDATA_HPP__ */










