/*! \file
    \brief Header of the tcp_client_t
*/


#ifndef __NEOIP_TCP_CLIENT_HPP__ 
#define __NEOIP_TCP_CLIENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_tcp_itor.hpp"
#include "neoip_tcp_full.hpp"
#include "neoip_tcp_client_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"
#include "neoip_copy_ctor_checker.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class tcp_itor_t;
class tcp_full_t;

/** \ref class to store the tcp client
 * 
 * - it is a helper class which merge the use of tcp_itor_t and tcp_full_t
 *   which are typical for tcp clients.
 */
class tcp_client_t : NEOIP_COPY_CTOR_DENY, private tcp_itor_cb_t, private tcp_full_cb_t {
private:
	/*************** tcp initiator	***************************************/
	tcp_itor_t *	tcp_itor;	//!< to initiate the connection
	bool		neoip_tcp_itor_event_cb(void *userptr, tcp_itor_t &cb_tcp_itor
						, const tcp_event_t &tcp_event)	throw();

	/*************** tcp full	***************************************/
	tcp_full_t *	tcp_full;	//!< the tcp_full_t struct
	bool		neoip_tcp_full_event_cb(void *userptr, tcp_full_t &cb_tcp_full
						, const tcp_event_t &tcp_event)	throw();
	bool		recv_cnx_established(tcp_full_t *tcp_full)		throw();
	
	/*************** callback	***************************************/
	tcp_client_cb_t *	callback;	//!< the callback to notify in case of event
	void *			userptr;	//!< the userptr associated with the event callback
	bool			notify_callback(const tcp_event_t &tcp_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	tcp_client_t() 	throw();
	~tcp_client_t() throw();

	/*************** setup function	***************************************/
	tcp_client_t &	set_local_addr(const ipport_addr_t &local_addr)		throw();
	inet_err_t	start(const ipport_addr_t &remote_addr, tcp_client_cb_t *callback
						, void * userptr)		throw();

	//! return if tcp_client_t is in itor phase, false otherwise
	bool		is_itor()	const throw()	{ return tcp_itor ? true : false;	}
	tcp_full_t *	steal_full()	throw();

	/*************** Get local/remote addresses - itor or full	*******/
	const ipport_addr_t &	get_local_addr()	const throw()
					{ if(tcp_itor)	return tcp_itor->get_local_addr();
					  else		return tcp_full->get_local_addr();	}
	const ipport_addr_t &	get_remote_addr()	const throw()
					{ if(tcp_itor)	return tcp_itor->get_remote_addr();
					  else		return tcp_full->get_remote_addr();	}

	/*************** send function - tcp_full only	***********************/
	size_t		send(const void *data_ptr, size_t data_len) 		throw()
				{ DBG_ASSERT(tcp_full); return tcp_full->send(data_ptr, data_len);	}
	inet_err_t	recv_max_len_set(size_t recv_max_len)	throw()
			{ DBG_ASSERT(tcp_full); return tcp_full->recv_max_len_set(recv_max_len);	}
	size_t		recv_max_len_get()		const throw()
			{ DBG_ASSERT(tcp_full); return tcp_full->recv_max_len_get();			}
	inet_err_t	maysend_set_threshold(ssize_t maysend_threshold)throw()
			{ DBG_ASSERT(tcp_full); return tcp_full->maysend_set_threshold(maysend_threshold);	}
	ssize_t		maysend_get_threshold()				const throw()
			{ DBG_ASSERT(tcp_full); return tcp_full->maysend_get_threshold();		}
	bool		maysend_is_set()				const throw()
			{ DBG_ASSERT(tcp_full); return tcp_full->maysend_is_set();			}

	inet_err_t	sendbuf_set_max_len(ssize_t sendbuf_max_len)	throw()
			{ DBG_ASSERT(tcp_full); return tcp_full->sendbuf_set_max_len(sendbuf_max_len);	}
	ssize_t		sendbuf_get_max_len()				const throw()
			{ DBG_ASSERT(tcp_full); return tcp_full->sendbuf_get_max_len();			}
	ssize_t		sendbuf_get_used_len()				const throw()
			{ DBG_ASSERT(tcp_full); return tcp_full->sendbuf_get_used_len();		}
	ssize_t		sendbuf_get_free_len()				const throw()
			{ DBG_ASSERT(tcp_full); return tcp_full->sendbuf_get_free_len();		}
	bool		sendbuf_is_limited()				const throw()
			{ DBG_ASSERT(tcp_full); return tcp_full->sendbuf_is_limited();			}

	/*************** display function	*******************************/
	std::string	to_string()		const throw();	
	friend	std::ostream & operator << (std::ostream & os, const tcp_client_t &tcp_client)	throw()
						{ return os << tcp_client.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_CLIENT_HPP__  */



