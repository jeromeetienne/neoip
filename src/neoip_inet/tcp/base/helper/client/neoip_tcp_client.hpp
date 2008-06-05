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
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \ref class to store the tcp client
 * 
 * - it is a helper class which merge the use of tcp_itor_t and tcp_full_t
 *   which are typical for tcp clients.
 */
class tcp_client_t : NEOIP_COPY_CTOR_DENY, private tcp_itor_cb_t, private tcp_full_cb_t
			, private socket_stream_vapi_t, private socket_rate_vapi_t {
public:
	//! a constant for unlimited value - used in maysend_tshold() and sendbug_maxlen()
	static const size_t	UNLIMITED;
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
	tcp_client_t() 		throw();
	~tcp_client_t() 	throw();

	/*************** setup function	***************************************/
	tcp_client_t &	profile(const tcp_profile_t &tcp_profile)		throw();
	tcp_client_t &	local_addr(const ipport_addr_t &local_addr)		throw();
	inet_err_t	start(const ipport_addr_t &remote_addr, tcp_client_cb_t *callback
						, void * userptr)		throw();
	inet_err_t	start(const tcp_itor_arg_t &itor_arg, tcp_client_cb_t *callback
						, void * userptr)		throw();

	//! return if tcp_client_t is in itor phase, false otherwise
	bool		is_itor()	const throw()	{ return tcp_itor ? true : false;	}
	tcp_full_t *	steal_full()	throw();

	/*************** Get local/remote addresses - itor or full	*******/
	const ipport_addr_t &	local_addr()	const throw()
							{ if(tcp_itor)	return tcp_itor->local_addr();
							  else		return tcp_full->local_addr();	}
	const ipport_addr_t &	remote_addr()	const throw()
							{ if(tcp_itor)	return tcp_itor->remote_addr();
							  else		return tcp_full->remote_addr();}

	/*************** socket_stream_vapi_t - full only	***************/
	void		rcvdata_maxlen(size_t rcvdata_maxlen)		throw()
				{ DBG_ASSERT(tcp_full); tcp_full->rcvdata_maxlen(rcvdata_maxlen);	}
	size_t		rcvdata_maxlen()				const throw()
				{ DBG_ASSERT(tcp_full); return tcp_full->rcvdata_maxlen();		}
	void		maysend_tshold(size_t new_threshold)		throw()
				{ DBG_ASSERT(tcp_full); tcp_full->maysend_tshold(new_threshold);	}
	size_t		maysend_tshold()				const throw()
				{ DBG_ASSERT(tcp_full); return tcp_full->maysend_tshold();		}
	void		xmitbuf_maxlen(size_t xmitbuf_maxlen)		throw()
				{ DBG_ASSERT(tcp_full); tcp_full->xmitbuf_maxlen(xmitbuf_maxlen);	}
	size_t		xmitbuf_maxlen()				const throw()
				{ DBG_ASSERT(tcp_full); return tcp_full->xmitbuf_maxlen();		}
	size_t		xmitbuf_usedlen()				const throw()
				{ DBG_ASSERT(tcp_full); return tcp_full->xmitbuf_usedlen();		}
	size_t		xmitbuf_freelen()				const throw()
				{ DBG_ASSERT(tcp_full); return tcp_full->xmitbuf_freelen();		}
	size_t		send(const void *data_ptr, size_t data_len) 	throw()
				{ DBG_ASSERT(tcp_full); return tcp_full->send(data_ptr, data_len);	}

	// helper functions - tcp_full_t only
	size_t		send(const pkt_t &pkt)	throw()	{ DBG_ASSERT(tcp_full);
							  return tcp_full->send(pkt.get_data(), pkt.get_len());	}
	size_t		send(const datum_t &datum)throw(){DBG_ASSERT(tcp_full);
							  return tcp_full->send(datum.get_data(), datum.get_len());}

	
	/*************** socket_rate_vapi_t	*******************************/
	void		xmit_limit(const rate_limit_arg_t &limit_arg)	throw()
					{ DBG_ASSERT(tcp_full); tcp_full->xmit_limit(limit_arg);	}
	rate_limit_t &	xmit_limit()					throw()
					{ DBG_ASSERT(tcp_full); return tcp_full->xmit_limit();		}
	const rate_limit_t &xmit_limit()				const throw()
					{ DBG_ASSERT(tcp_full); return tcp_full->xmit_limit();		}
	bool		xmit_limit_is_set()				const throw()
					{ DBG_ASSERT(tcp_full); return tcp_full->xmit_limit_is_set();	}
	void		recv_limit(const rate_limit_arg_t &limit_arg)	throw()
					{ DBG_ASSERT(tcp_full); tcp_full->recv_limit(limit_arg);	}
	rate_limit_t &	recv_limit()					throw()
					{ DBG_ASSERT(tcp_full); return tcp_full->recv_limit();		}
	const rate_limit_t &recv_limit()				const throw()
					{ DBG_ASSERT(tcp_full); return tcp_full->recv_limit();		}
	bool		recv_limit_is_set()				const throw()
					{ DBG_ASSERT(tcp_full); return tcp_full->recv_limit_is_set();	}

	/*************** compatibility layer	*******************************/
	tcp_client_t &		set_local_addr(const ipport_addr_t &p_local_addr)throw()
						{ return local_addr(p_local_addr);			}
	const ipport_addr_t &	get_local_addr()	const throw()	{ return local_addr();	}
	const ipport_addr_t &	get_remote_addr()	const throw()	{ return remote_addr();	}
	inet_err_t		recv_max_len_set(ssize_t recv_max_len)		throw()
						{ rcvdata_maxlen(recv_max_len);	return inet_err_t::OK;	}
	ssize_t			recv_max_len_get()				const throw()
						{ return rcvdata_maxlen();		}
	inet_err_t		maysend_set_threshold(ssize_t maysend_threshold)throw()
						{ maysend_tshold(maysend_threshold); return inet_err_t::OK;}
	ssize_t			maysend_get_threshold()				const throw()
						{ return maysend_tshold();		}
	bool			maysend_is_set()				const throw()
						{ return maysend_tshold() != UNLIMITED;	}
	inet_err_t		sendbuf_set_max_len(ssize_t sendbuf_max_len)	throw()
						{ xmitbuf_maxlen(sendbuf_max_len); return inet_err_t::OK;}
	ssize_t			sendbuf_get_max_len()				const throw()
						{ return xmitbuf_maxlen();		}
	ssize_t			sendbuf_get_used_len()				const throw()
						{ return xmitbuf_usedlen();		}
	ssize_t			sendbuf_get_free_len()				const throw()
						{ return xmitbuf_freelen();		}
	bool			sendbuf_is_limited()				const throw()
						{ return xmitbuf_maxlen() != UNLIMITED;	}

	/*************** display function	*******************************/
	std::string	to_string()		const throw();	
	friend	std::ostream & operator << (std::ostream & os, const tcp_client_t &tcp_client)	throw()
						{ return os << tcp_client.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_CLIENT_HPP__  */



