/*! \file
    \brief Header of the \ref rate_limit_t
    
*/


#ifndef __NEOIP_RATE_LIMIT_HPP__ 
#define __NEOIP_RATE_LIMIT_HPP__ 
/* system include */
/* local include */
#include "neoip_rate_limit_cb.hpp"
#include "neoip_rate_limit_arg.hpp"
#include "neoip_rate_limit_wikidbg.hpp"
#include "neoip_rate_prec.hpp"
#include "neoip_rate_err.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	rate_sched_t;

/** \brief Structure to pass information from the data_request() to the data_notify()
 * 
 * - NOTE: this is required as the caller is doing its action (e.g. write/read) 
 *   between the two.
 */
struct rate_limit_tmp_t : NEOIP_COPY_CTOR_ALLOW {
	double	window_delay;	//!< allocated window_delay as a double in second
	double	maxi_len;	//!< the allowed length as a double
};

/** \brief Handle a item of rate_limit_t
 */
class rate_limit_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t
			, private wikidbg_obj_t<rate_limit_t, rate_limit_wikidbg_init> {
private:
	rate_sched_t *		m_rate_sched;	//!< backpointer on the rate_sched_t
	rate_prec_t		m_maxi_prec;
	rate_prec_t		m_used_prec;
	double			m_absrate_max;	//!< the maximum rate in absolute
	rate_limit_profile_t	m_profile;

	/*************** Internal function	*******************************/
	void		used_prec(const rate_prec_t &new_prec)	throw();

	/*************** timeout_t	***************************************/
	timeout_t		inuse_timeout;
	bool			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** callback stuff	***************************************/
	rate_limit_cb_t*	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback()		throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	rate_limit_t()		throw();
	~rate_limit_t()		throw();
	
	/*************** Setup function	***************************************/
	rate_limit_t &	set_profile(const rate_limit_profile_t &limit_profile)			throw();
	rate_err_t	start(rate_sched_t *rate_sched, const rate_prec_t &p_maxi_prec
						, rate_limit_cb_t *callback, void *userptr)	throw();
	rate_err_t	start(const rate_limit_arg_t &arg, rate_limit_cb_t *callback
									, void *userptr)	throw();
	
	/*************** Query function	***************************************/
	rate_sched_t *		rate_sched()		const throw()	{ return m_rate_sched;		}
	const rate_prec_t &	maxi_prec()		const throw()	{ return m_maxi_prec;		}
	const rate_prec_t &	used_prec()		const throw()	{ return m_used_prec;		}
	double			absrate_max()		const throw()	{ return m_absrate_max;		}
	const rate_limit_profile_t &profile()		const throw()	{ return m_profile;		}
	bool			is_used()		const throw()	{ return inuse_timeout.is_running();	}
	bool			is_idle()		const throw()	{ return !is_used();		}
	bool			is_sane()		const throw();
	
	
	/*************** Action function	*******************************/
	void		maxi_prec(const rate_prec_t &new_prec)	throw();
	void		absrate_max(double new_absrate_max)	throw()	{ m_absrate_max = new_absrate_max;	}
	size_t		data_request(size_t request_len, rate_limit_tmp_t &limit_tmp)	const throw();
	void		data_notify(size_t used_len, const rate_limit_tmp_t &limit_tmp)	throw();

	/*************** List of friend class	*******************************/
	friend class	rate_limit_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RATE_LIMIT_HPP__  */










