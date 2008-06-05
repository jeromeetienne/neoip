/*! \file
    \brief Header of the \ref rate_sched_nunit_item_t
    
*/


#ifndef __NEOIP_RATE_SCHED_NUNIT_ITEM_HPP__ 
#define __NEOIP_RATE_SCHED_NUNIT_ITEM_HPP__ 
/* system include */
/* local include */
#include "neoip_rate_prec.hpp"
#include "neoip_rate_err.hpp"
#include "neoip_rate_limit_cb.hpp"
#include "neoip_timeout.hpp"
#include "neoip_socket_client_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	rate_sched_testclass_t;
class	rate_limit_t;

/** \brief Handle a item of rate_sched_nunit_item_t
 */
class rate_sched_nunit_item_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t, private socket_client_cb_t
						, private rate_limit_cb_t {
private:
	rate_sched_testclass_t *rate_sched_testclass;	//!< backpointer on the rate_sched_testclass_t
	delay_t			chunk_init_delay;
	size_t			chunk_stop_len;
	size_t			chunk_sent_len;

	bool			try_send_data()	throw();

	/*************** socket_client_t	*******************************/
	socket_client_t *	socket_client;
	bool			neoip_socket_client_event_cb(void *userptr, socket_client_t &cb_socket_client
							, const socket_event_t &socket_event)	throw();
	/*************** init_timeout	***************************************/
	timeout_t		init_timeout;
	bool			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	/*************** rate_limit_t	***************************************/
	rate_limit_t *		rate_limit;
	bool			neoip_rate_limit_cb(void *cb_userptr, rate_limit_t &cb_rate_limit)throw();
public:
	/*************** ctor/dtor	***************************************/
	rate_sched_nunit_item_t()		throw();
	~rate_sched_nunit_item_t()		throw();
	
	/*************** Setup function	***************************************/
	rate_err_t	start(rate_sched_testclass_t *rate_sched_testclass, const rate_prec_t &rate_prec
						, const delay_t &chunk_init_delay
						, size_t chunk_stop_len)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RATE_SCHED_NUNIT_ITEM_HPP__  */










