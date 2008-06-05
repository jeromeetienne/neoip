/*! \file
    \brief Declaration of the tcp_full_t
    
*/


#ifndef __NEOIP_TCP_FULL_HPP__ 
#define __NEOIP_TCP_FULL_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_tcp_full_wikidbg.hpp"
#include "neoip_tcp_full_cb.hpp"
#include "neoip_tcp_full_profile.hpp"
#include "neoip_socket_stream_vapi.hpp"
#include "neoip_socket_rate_vapi.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_fdwatch_cb.hpp"
#include "neoip_tcp_event.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_rate_limit_cb.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_pkt.hpp"
#include "neoip_object_slotid.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	tcp_full_arg_t;

/** \brief class definition for tcp_full_t
 */
class tcp_full_t : NEOIP_COPY_CTOR_DENY, public socket_stream_vapi_t, public socket_rate_vapi_t
			, public fdwatch_cb_t, public object_slotid_t, public rate_limit_cb_t
			, private wikidbg_obj_t<tcp_full_t, tcp_full_wikidbg_init>
			{
public:
	//! a constant for unlimited value - used in maysend_tshold() and sendbuf_maxlen()
	static const size_t	UNLIMITED_VAL	= 0xFFFFFFFF;
	static const size_t	UNLIMITED;
private:
	tcp_full_profile_t m_profile;		//!< the profile for this tcp_full_t
	ipport_addr_t	m_local_addr;		//!< the local address
	ipport_addr_t	m_remote_addr;		//!< the remote address
	size_t		m_rcvdata_maxlen;	//!< set the maximum number of bytes notified 
						//!< by RECVED_DATA event. if == 0, any input
						//!< on this connection will be ignored.
	size_t		m_maysend_tshold;
 	size_t		m_xmitbuf_maxlen;
	bytearray_t	xmitbuf;		//!< the send buffer

	/*************** rate_limit_t callback	*******************************/
	bool		neoip_rate_limit_cb(void *userptr, rate_limit_t &cb_rate_limit)	throw();

	/*************** recv_limit	***************************************/
	rate_limit_t *	m_recv_limit;
	bool		recv_limit_cb(void *userptr, rate_limit_t &cb_rate_limit)	throw();

	/*************** xmit_limit	***************************************/
	rate_limit_t *	m_xmit_limit;
	bool		xmit_limit_cb(void *userptr, rate_limit_t &cb_rate_limit)	throw();

	/*************** fdwatch_t	***************************************/
	fdwatch_t *	fdwatch;	//!< the fdwatch to monitor the socket
	bool		neoip_fdwatch_cb(void *cb_userptr, const fdwatch_t &cb_fdwatch
					, const fdwatch_cond_t &cb_fdwatch_cond)	throw();
	bool		try_to_recv()							throw();
	bool		try_to_xmit()							throw();
	bool		handle_error()							throw();
	/*************** callback stuff	***************************************/
	tcp_full_cb_t*	callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const tcp_event_t &tcp_event)		throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback

public:
	/*************** ctor/dtor	***************************************/
	tcp_full_t(const ipport_addr_t &p_local_addr, const ipport_addr_t &p_remote_addr
							, fdwatch_t *fdwatch)	throw();
	~tcp_full_t()								throw();

	/**************** Setup function	*******************************/
	tcp_full_t &	profile(const tcp_full_profile_t &profile)		throw();	
	tcp_full_t &	set_callback(tcp_full_cb_t *callback, void *userptr)	throw();
	inet_err_t	start()							throw();
	inet_err_t	start(tcp_full_cb_t *callback, void *userptr)		throw()
					{ return this->set_callback(callback,userptr).start();	}

	/**************** Query function	*******************************/
	bool			is_null()		const throw()	{ return local_addr().is_null();}
	const tcp_full_profile_t&profile()		const throw()	{ return m_profile;		}
	bool			is_started()		const throw();
	
	/*************** socket_common_vapi_t	*******************************/
	const ipport_addr_t &	local_addr()		const throw()	{ return m_local_addr;		}
	const ipport_addr_t &	remote_addr()		const throw()	{ return m_remote_addr;		}
	void			rcvdata_maxlen(size_t new_rcvdata_maxlen)	throw();
	size_t			rcvdata_maxlen()	const throw()	{ return m_rcvdata_maxlen;	}
	size_t			send(const void *data_ptr, size_t data_len) 	throw();
	// helper functions - tcp_full_t only
	size_t		send(const pkt_t &pkt)		throw()	{ return send(pkt.void_ptr(), pkt.length());	}
	size_t		send(const datum_t &datum)	throw()	{ return send(datum.void_ptr(), datum.length());}

	/*************** socket_stream_vapi_t	*******************************/
	void			maysend_tshold(size_t new_tshold)		throw();
	size_t			maysend_tshold()	const throw()	{ return m_maysend_tshold;	}
	void			xmitbuf_maxlen(size_t new_xmitbuf_maxlen)	throw();
	size_t			xmitbuf_maxlen()	const throw()	{ return m_xmitbuf_maxlen;	}
	size_t			xmitbuf_usedlen()	const throw();
	size_t			xmitbuf_freelen()	const throw();

	/*************** socket_rate_vapi_t	*******************************/
	void			xmit_limit(const rate_limit_arg_t &limit_arg)	throw();
	rate_limit_t &		xmit_limit()		throw() 	{ DBG_ASSERT(m_xmit_limit); return *m_xmit_limit;	}
	const rate_limit_t &	xmit_limit()		const throw()	{ DBG_ASSERT(m_xmit_limit); return *m_xmit_limit;	}
	bool			xmit_limit_is_set()	const throw()	{ return m_xmit_limit != 0;	}
	void			recv_limit(const rate_limit_arg_t &limit_arg)	throw();
	rate_limit_t &		recv_limit()		throw() 	{ DBG_ASSERT(m_recv_limit); return *m_recv_limit;	}
	const rate_limit_t &	recv_limit()		const throw()	{ DBG_ASSERT(m_recv_limit); return *m_recv_limit;	}
	bool			recv_limit_is_set()	const throw()	{ return m_recv_limit != 0;	}

	/*************** compatibility layer	*******************************/ // TODO to remove
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

	/*************** Display function	*******************************/
	std::string		to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const tcp_full_t &tcp_full)	throw()
					{ return os << tcp_full.to_string();	}


	/*************** List of friend class	*******************************/
	friend class	tcp_full_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_FULL_HPP__  */




