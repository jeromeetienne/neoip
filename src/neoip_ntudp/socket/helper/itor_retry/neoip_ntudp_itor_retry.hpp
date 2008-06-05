/*! \file
    \brief Declaration of the ntudp_itor_retry_t
    
*/


#ifndef __NEOIP_NTUDP_ITOR_RETRY_HPP__ 
#define __NEOIP_NTUDP_ITOR_RETRY_HPP__ 
/* system include */
#include <set>
/* local include */
#include "neoip_ntudp_itor_retry_cb.hpp"
#include "neoip_ntudp_itor.hpp"
#include "neoip_ntudp_addr.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_ntudp_event.hpp"
#include "neoip_timeout.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	ntudp_peer_t;

/** \brief class definition for ntudp_itor_retry_t
 * 
 * - \ref ntudp_itor_retry_t is a helper on top of ntudp_itor_t which retry to 
 *   ntudp_itor_t as long as they keeps reporting ntudp_event_t::UNKNOWN_HOST
 *   with an delaygen_t between each attempts of ntudp_itor_t
 */
class ntudp_itor_retry_t  : NEOIP_COPY_CTOR_DENY, private ntudp_itor_cb_t, private timeout_cb_t {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default amount of time before ntudp_itor_retry_t timeout
	static const delay_t		EXPIRE_DELAY_DFL;
	//! default delaygen arguments to generate the delay before retrying ntudp_itor_t
	static const delaygen_arg_t	DELAYGEN_ARG_RETRY_DFL;
private:
	ntudp_peer_t *	ntudp_peer;	//!< backpointer on the attached ntudp_peer_t

	std::set<ntudp_event_t::type>	retry_event_db;	//!< the database of event to retry

	/*************** ntudp_itor_t	***************************************/
	ntudp_itor_t *	ntudp_itor;	//!< the ntudp_itor_t running
	bool 		neoip_ntudp_itor_event_cb(void *cb_userptr, ntudp_itor_t &cb_ntudp_itor
							, const ntudp_event_t &ntudp_event)	throw();

	/*************** main timeout_t callback	***********************/
	bool 		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** retry_timeout	***************************************/
	delaygen_t	retry_delaygen;	//!< the delaygen_t used between the attempts of itor
	timeout_t	retry_timeout;	//!< to be notified with the ntudp_itor_retry_t expire
	bool 		retry_timeout_cb(void *userptr, timeout_t &cb_timeout)		throw();

	/*************** expire_timeout	***************************************/
	delay_t		expire_delay;
	timeout_t	expire_timeout;	//!< to be notified with the ntudp_itor_retry_t expire
	bool 		expire_timeout_cb(void *userptr, timeout_t &cb_timeout)		throw();

	/*************** callback	***************************************/
	ntudp_itor_retry_cb_t *	callback;	//!< callback used to notify result
	void *			userptr;	//!< userptr associated with the callback
	bool			notify_callback(const ntudp_event_t &ntudp_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	ntudp_itor_retry_t()			throw();
	~ntudp_itor_retry_t()			throw();

	/*************** Setup function	***************************************/
	ntudp_itor_retry_t &	set_callback(ntudp_itor_retry_cb_t *callback, void *userptr)	throw();
	ntudp_itor_retry_t &	set_retry_event(const ntudp_event_t::type &event_type)		throw();
	ntudp_itor_retry_t &	set_local_addr(const ntudp_addr_t &local_addr)			throw();
	ntudp_itor_retry_t &	set_timeout(const delay_t &expire_delay = EXPIRE_DELAY_DFL)	throw();
	ntudp_err_t		set_remote_addr(ntudp_peer_t *ntudp_peer
							, const ntudp_addr_t &remote_addr)	throw();
	ntudp_err_t		start()								throw();
	ntudp_err_t		start(ntudp_peer_t *ntudp_peer, const ntudp_addr_t &remote_addr
					, ntudp_itor_retry_cb_t *callback, void *userptr)	throw();
	
	/*************** Query function	***************************************/
	bool			is_null()	const throw()	{ return callback == NULL;		}
	const ntudp_addr_t &	local_addr()	const throw()	{ return ntudp_itor->local_addr();	}
	const ntudp_addr_t &	remote_addr()	const throw()	{ return ntudp_itor->remote_addr();	}

	/*************** display function	*******************************/
	std::string	to_string()	const throw()	{ return ntudp_itor->to_string();	}
	friend	std::ostream & operator << (std::ostream & os, const ntudp_itor_retry_t &itor_retry ) throw()
		{ return os << itor_retry.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_ITOR_RETRY_HPP__  */



