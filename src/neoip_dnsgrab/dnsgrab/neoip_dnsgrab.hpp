/*! \file
    \brief Header of the \ref dnsgrab_t class
    
*/


#ifndef __NEOIP_DNSGRAB_HPP__ 
#define __NEOIP_DNSGRAB_HPP__ 
/* system include */
#include <iostream>
#include <string>
#include <list>
/* local include */
#include "neoip_dnsgrab_cb.hpp"
#include "neoip_dnsgrab_err.hpp"
#include "neoip_dnsgrab_wikidbg.hpp"
#include "neoip_dnsgrab_request.hpp"
#include "neoip_tcp_resp.hpp"
#include "neoip_delay.hpp"
#include "neoip_slotpool.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	dnsgrab_cnx_t;
class	dnsgrab_register_t;
class	dnsgrab_arg_t;

/** \brief Handle a dns grabber
 */
class dnsgrab_t : NEOIP_COPY_CTOR_DENY, private tcp_resp_cb_t
				, private wikidbg_obj_t<dnsgrab_t, dnsgrab_wikidbg_init> {
private:
	dnsgrab_register_t *	dnsgrab_register;	//!< a pointer on the grabber registration

	/*************** tcp_resp_t responder	*******************************/
	tcp_resp_t *		tcp_resp;
	bool			neoip_tcp_resp_event_cb(void *userptr, tcp_resp_t &cb_tcp_resp
					, const tcp_event_t &tcp_event)		throw();

	/*************** list of incoming connections	***********************/
	std::list<dnsgrab_cnx_t *>	cnx_db;
	void 			cnx_link(dnsgrab_cnx_t *cnx)	throw()	{ cnx_db.push_back(cnx);	}
	void 			cnx_unlink(dnsgrab_cnx_t *cnx)	throw()	{ cnx_db.remove(cnx);		}
	slotpool_t		cnx_slotpool;	//!< slotpool to allocate slot_id for each cnx
	
	/*************** callback stuff	***************************************/
	dnsgrab_cb_t *		callback;	//!< the callback to be notified when a request is received
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(dnsgrab_request_t &request)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	dnsgrab_t(dnsgrab_cb_t *callback, void *userptr)	throw();
	~dnsgrab_t()						throw();

	/*************** setup function	***************************************/
	dnsgrab_err_t	start(const ipport_addr_t &listen_addr, const std::string reg_location
					, size_t reg_priority, const delay_t &reg_request_ttl
					, const std::string &reg_tag = ""
					, const std::string &reg_nounce = "")	throw();
	dnsgrab_err_t	start(const dnsgrab_arg_t &dnsgrab_arg)			throw();

	/*************** to notify a reply	*******************************/
	dnsgrab_err_t	notify_reply(const dnsgrab_request_t &dnsgrab_request)	throw();
	
	/*************** List of friend class	*******************************/
	friend class	dnsgrab_cnx_t;
	friend class	dnsgrab_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DNSGRAB_HPP__  */



