/*! \file
    \brief Declaration of the ntudp_npos_server_t
    
*/


#ifndef __NEOIP_NTUDP_NPOS_SERVER_HPP__ 
#define __NEOIP_NTUDP_NPOS_SERVER_HPP__ 
/* system include */
#include <list>
#include <map>
/* local include */
#include "neoip_ntudp_npos_server_probe_cb.hpp"
#include "neoip_ntudp_npos_server_wikidbg.hpp"
#include "neoip_ntudp_nonce.hpp"
#include "neoip_ntudp_err.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_udp_vresp_cb.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief class definition for ntudp_npos_server_t
 */
class ntudp_npos_server_t : private udp_vresp_cb_t, NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<ntudp_npos_server_t, ntudp_npos_server_wikidbg_init>
			{
private:
	/*************** Handle the udp_vresp_t	*******************************/
	udp_vresp_t *	udp_vresp;	//!< the udp_vresp_t to which it is attached
	bool neoip_inet_udp_vresp_event_cb(void *cb_userptr, udp_vresp_t &cb_udp_vresp
							, const udp_event_t &udp_event)	throw();	

	/*************** List of incoming connection	***********************/
	class				cnx_t;
	std::list<cnx_t *>		cnx_list;
	void cnx_link(cnx_t *cnx)	throw()	{ cnx_list.push_back(cnx);	}
	void cnx_unlink(cnx_t *cnx)	throw()	{ cnx_list.remove(cnx);		}
	
	
	/*************** inetreach_probe callback	***********************/
	class						probe_cb_t;
	typedef std::map<ntudp_nonce_t, probe_cb_t>	probe_db_t;
	probe_db_t	probe_db;
	probe_cb_t	inetreach_probe_find(ntudp_nonce_t &nounce)		const throw();

public:
	/*************** ctor/dtor	***************************************/
	ntudp_npos_server_t()		throw();
	~ntudp_npos_server_t()		throw();

	/*************** Setup function	***************************************/
	ntudp_err_t	start(udp_vresp_t *udp_vresp)	throw();
	
	/*************** Query function	***************************************/
	ipport_addr_t	listen_addr()			const throw();

	/*************** inetreach_probe function	***********************/
	void		inetreach_probe_register(ntudp_nonce_t &nonce
				, ntudp_npos_server_probe_cb_t *callback, void * userptr)	throw();
	void		inetreach_probe_unregister(ntudp_nonce_t &nonce)		throw();


	/*************** List of friend classes	*******************************/
	friend class	ntudp_npos_server_wikidbg_t;
};

/** \brief the probe callback stored to determine the inetreach test
 */
class ntudp_npos_server_t::probe_cb_t : NEOIP_COPY_CTOR_ALLOW {
private:
	ntudp_npos_server_probe_cb_t*	callback;	//!< callback used to notify result
	void *				userptr;	//!< userptr associated with the callback
	ntudp_nonce_t			nonce;		//!< the pkttype for which this probe_cb_t
	// TODO is this nonce used ? i dont think so :) so remove it
	// - so try to remove it and see what happen
	// - in anycase this object seems like shit to me
	// - maybe a good idea to recode it in its own .cpp ?
public:
	probe_cb_t()	throw() : callback(NULL)	{}
	probe_cb_t(const ntudp_nonce_t &nonce, ntudp_npos_server_probe_cb_t *callback, void *userptr)throw() 
			: callback(callback), userptr(userptr), nonce(nonce)	{}
	bool	is_null()	const throw()	{ return callback == NULL;	}
	
	ntudp_npos_server_probe_cb_t*	get_callback()	throw()	{ return callback;	}
	void *				get_userptr()	throw()	{ return userptr;	}


	/*************** List of friend classes	*******************************/
	friend class	ntudp_npos_server_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_NPOS_SERVER_HPP__  */



