/*! \file
    \brief Header of the \ref casti_swarm_httpi_t
    
*/


#ifndef __NEOIP_CASTI_SWARM_HTTPI_HPP__ 
#define __NEOIP_CASTI_SWARM_HTTPI_HPP__ 
/* system include */
/* local include */
#include "neoip_casti_swarm_httpi_wikidbg.hpp"
#include "neoip_casti_swarm_httpi_profile.hpp"
#include "neoip_bt_httpi_cb.hpp"
#include "neoip_file_size_arr.hpp"
#include "neoip_rate_estim.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	casti_swarm_t;
class	bt_err_t;
class	bt_ezswarm_t;
class	casti_swarm_udata_t;
class	casti_swarm_spos_t;

/** \brief Handle the httpi_arr for the casti_swarm_t
 */
class casti_swarm_httpi_t : NEOIP_COPY_CTOR_DENY, private bt_httpi_cb_t
		, private wikidbg_obj_t<casti_swarm_httpi_t, casti_swarm_httpi_wikidbg_init> {
private:
	casti_swarm_t *		m_casti_swarm;	//!< backpointer to the attached casti_swarm_t
	casti_swarm_httpi_profile_t m_profile;	//!< the profile for this object
	rate_estim_t<size_t>	m_rate_estim;	//!< estimate the recv_rate

	/*************** internal function	*******************************/
	bt_ezswarm_t *		bt_ezswarm()		const throw();
	casti_swarm_udata_t *	swarm_udata()		const throw();
	casti_swarm_spos_t*	swarm_spos()	const throw();

	/*************** bt_httpi_t	***************************************/
	bt_httpi_t *	m_bt_httpi;
	bool 		neoip_bt_httpi_cb(void *cb_userptr, bt_httpi_t &cb_bt_httpi
					, const bt_httpi_event_t &httpi_event)	throw();
	bool		handle_chunk_avail(const file_size_t &chunk_len)	throw();
	bool		handle_chunk_avail2(const file_size_t &chunk_len)	throw();
	bool		handle_mod_updated()					throw();
public:
	/*************** ctor/dtor	***************************************/
	casti_swarm_httpi_t()	throw();
	~casti_swarm_httpi_t()	throw();

	/*************** Setup function	***************************************/
	casti_swarm_httpi_t& profile(const casti_swarm_httpi_profile_t &profile)throw();
	bt_err_t	start(casti_swarm_t *casti_swarm)	throw();

	/*************** Query function	***************************************/
	const casti_swarm_httpi_profile_t & profile()	const throw()	{ return m_profile;		}
	casti_swarm_t *	casti_swarm()	const throw()	{ return m_casti_swarm;		}
	bt_httpi_t *	bt_httpi()	const throw()	{ return m_bt_httpi;		}
	double		rate_average()	const throw()	{ return m_rate_estim.average();}


	/*************** List of friend class	*******************************/
	friend class	casti_swarm_httpi_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_CASTI_SWARM_HTTPI_HPP__ */










