/*! \file
    \brief Header of the \ref simuwan_t

- see \ref neoip_simuwan.cpp
*/


#ifndef __NEOIP_SIMUWAN_HPP__ 
#define __NEOIP_SIMUWAN_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_simuwan_cb.hpp"
#include "neoip_simuwan_err.hpp"
#include "neoip_simuwan_event.hpp"
#include "neoip_simuwan_profile.hpp"
#include "neoip_pkt.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief A layer to compress packet
 */
class simuwan_t : NEOIP_COPY_CTOR_DENY {
public:	// declaration internal constant
	static const double	DROP_RATE_DFL;
	static const double	DELAYED_RATE_DFL;
	static const delay_t	DELAYED_MIN_DFL;
	static const delay_t	DELAYED_MAX_DFL;
	static const double	DUPLICATE_RATE_DFL;
	static const int	DUPLICATE_MIN_DFL;
	static const int	DUPLICATE_MAX_DFL;
	
private:
	// drop packet variable
	double		drop_rate;		//!< rate of dropped packets

	// delayed packet variable
	double		delayed_rate;		//!< rate of delayed packets
	delay_t		delayed_min;		//!< minimum delay for delayed packets
	delay_t		delayed_max;		//!< maximum delay for delayed packets

	// duplicate packet variable
	double		duplicate_rate;		//!< rate of duplicate packets
	int		duplicate_min;		//!< minimum of duplicate for one packet
	int		duplicate_max;		//!< maximum of duplicate for one packets

	simuwan_cb_t *	callback;		//!< a pointer on the event callback
	void *		userptr;		//!< the userptr associated with the above callback

	// pending_pkt stuff (used for delayed_rate and duplicate_rate)
	class	pending_pkt_t;
	std::list<pending_pkt_t *>	pending_pkt_db;		//!< list of pending packet
	void pending_pkt_link(pending_pkt_t *pending_pkt)	throw()
				{ pending_pkt_db.push_back(pending_pkt);	}
	void pending_pkt_unlink(pending_pkt_t *pending_pkt)	throw()
				{ pending_pkt_db.remove(pending_pkt);		}
public:
	// ctor/dtor
	simuwan_t()								throw();
	~simuwan_t()								throw();

	// initialization functions
	simuwan_err_t	set_from_profile(const simuwan_profile_t &profile)	throw();
	simuwan_err_t	set_callback(simuwan_cb_t * callback, void *userptr)	throw();
	simuwan_err_t	start()							throw();
	// helper function for start()
	simuwan_err_t	start(simuwan_cb_t *callback, void *userptr)		throw();

	simuwan_err_t	pkt_from_upper(pkt_t &pkt)				throw();
};

/** \brief Handle a pending datagram for \ref simuwan_t
 */
class	simuwan_t::pending_pkt_t : NEOIP_COPY_CTOR_ALLOW, public timeout_cb_t {
private:
	simuwan_t *	simuwan;		//!< backpointer to simuwan_t
	pkt_t		pkt;			//!< the pending packet itself
	timeout_t	expire_timeout;		//!< the expiration timeout
	bool		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	// ctor/dtor
	pending_pkt_t(simuwan_t *simuwan, const pkt_t &pkt, const delay_t &delay)	throw();
	~pending_pkt_t()								throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SIMUWAN_HPP__  */



