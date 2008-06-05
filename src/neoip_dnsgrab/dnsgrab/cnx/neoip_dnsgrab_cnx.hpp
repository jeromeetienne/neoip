/*! \file
    \brief Header of the \ref dnsgrab_t class
    
*/


#ifndef __NEOIP_DNSGRAB_CNX_HPP__ 
#define __NEOIP_DNSGRAB_CNX_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_tcp_full_cb.hpp"
#include "neoip_dnsgrab_cnx_wikidbg.hpp"
#include "neoip_dnsgrab.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	pkt_t;

/** \brief handle a given connection received by the socket_utest_resp_t
 */
class dnsgrab_cnx_t : NEOIP_COPY_CTOR_DENY, private tcp_full_cb_t
				, private wikidbg_obj_t<dnsgrab_cnx_t, dnsgrab_cnx_wikidbg_init> {
private:
	dnsgrab_t *	dnsgrab;	//!< backpointer to dnsgrab_t
	slot_id_t	slot_id;	//!< the slot_id of this cnx
	std::stringstream recved_data;	//!< contains the data read from the connection
					//!< until the http request if fully received
	
	/*************** tcp_full_t	***************************************/
	tcp_full_t *	tcp_full;
	bool		neoip_tcp_full_event_cb(void *userptr, tcp_full_t &cb_tcp_full
					, const tcp_event_t &tcp_event)		throw();

	/*************** data parsing function	*******************************/
	bool		handle_recved_data(const pkt_t &pkt)					throw();
	bool		contain_request(const std::stringstream &ss) 				throw();						
	bool		parse_request(const std::string &str, dnsgrab_request_t &request_out)	throw();
public:
	/*************** ctor/dtor	***************************************/
	dnsgrab_cnx_t(dnsgrab_t *dnsgrab, tcp_full_t *tcp_full)	throw();
	~dnsgrab_cnx_t()					throw();
	
	/*************** Query function	***************************************/
	slot_id_t	get_slot_id()	const throw()	{ return slot_id;	}
	
	/*************** action function	*******************************/
	void		notify_reply(const dnsgrab_request_t &dnsgrab_request)	throw();

	/*************** List of friend class	*******************************/
	friend class	dnsgrab_cnx_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DNSGRAB_CNX_HPP__  */



