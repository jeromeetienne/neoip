/*! \file
    \brief Header of the test of tcp_client_t
*/


#ifndef __NEOIP_TCP_CHARGEN_CLIENT_HPP__ 
#define __NEOIP_TCP_CHARGEN_CLIENT_HPP__ 
/* system include */
/* local include */
#include "neoip_tcp_chargen_client_cb.hpp"
#include "neoip_tcp_client_cb.hpp"
#include "neoip_timeout.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	tcp_itor_arg_t;
class	pkt_t;

/** \brief Class which implement chargen client
 */
class tcp_chargen_client_t : NEOIP_COPY_CTOR_DENY, private tcp_client_cb_t	{
private:	
	size_t		recv_maxlen;	//!< the amount of data to receive before notifying OK
					//!< 0 means infinite
	size_t		recv_curlen;	//!< the amount of data already received
	/*************** client	***********************************************/
	tcp_client_t*tcp_client;
	bool		neoip_tcp_client_event_cb(void *userptr, tcp_client_t &cb_tcp_client
					, const tcp_event_t &tcp_event)	throw();

	/*************** packet reception	*******************************/
	bool		handle_recved_data(pkt_t &pkt)				throw();

	/*************** callback stuff	***************************************/
	tcp_chargen_client_cb_t*callback;	//!< the callback to notify
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(const inet_err_t &inet_err)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	tcp_chargen_client_t()		throw();
	~tcp_chargen_client_t()	throw();

	/*************** Setup function	***************************************/
	inet_err_t	start(const tcp_itor_arg_t &itor_arg, size_t recv_maxlen
				, tcp_chargen_client_cb_t *callback, void *userptr)	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_CHARGEN_CLIENT_HPP__  */



