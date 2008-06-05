/*! \file
    \brief Declaration of the tcp_full_t
    
*/


#ifndef __NEOIP_TCP_FULL_HPP__ 
#define __NEOIP_TCP_FULL_HPP__ 
/* system include */
#include <iostream>
#include <string>
/* local include */
#include "neoip_tcp_full_cb.hpp"
#include "neoip_tcp_full_profile.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_fdwatch_cb.hpp"
#include "neoip_tcp_event.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_object_slotid.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for tcp_full_t
 */
class tcp_full_t : public fdwatch_cb_t, public zerotimer_cb_t, public object_slotid_t
				, NEOIP_COPY_CTOR_DENY {
public:
	//! the sendbuf_maxlen when it is unlimited
	static const size_t	SENDBUF_UNLIMITED;
	//! the value of rcvdata_maxlen to prevent reading
	static const size_t	RCVDATA_MAXLEN_NOREAD;
	//! the maysend_threshold value when it is unset
	static const size_t	MAYSEND_TSHOLD_UNSET;
private:
	ipport_addr_t		local_addr;	//!< the local address
	ipport_addr_t		remote_addr;	//!< the remote address
	tcp_full_profile_t	profile;	//!< the profile for this tcp_full_t
	ssize_t			recv_max_len;	//!< set the maximum number of bytes notified 
						//!< by RECVED_DATA event. if == 0, any input
						//!< on this connection will be ignored.
	ssize_t			maysend_threshold;
 	ssize_t			sendbuf_max_len;
	bytearray_t		sendbuf;	//!< the send buffer

	/*************** fdwatch_t	***************************************/
	fdwatch_t *	fdwatch;	//!< the fdwatch to monitor the socket
	bool		neoip_fdwatch_cb(void *cb_userptr, const fdwatch_t &cb_fdwatch
						, const fdwatch_cond_t &cb_fdwatch_cond)	throw();
	

	/*************** zerotimer_maysend_off	*******************************/
	zerotimer_t	zerotimer_maysend_off;	//!< as maysend_off is triggered by tcp_full_t::send
						//!< and as it is not allowed to notify callback during
						//!< an external function call, this zerotimer it used
						//!< to notify maysend_off
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();

	/*************** callback stuff	***************************************/
	tcp_full_cb_t *	callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const tcp_event_t &tcp_event)		throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback

public:
	/*************** ctor/dtor	***************************************/
	tcp_full_t(const ipport_addr_t &local_addr, const ipport_addr_t &remote_addr
				, fdwatch_t *fdwatch)			throw();
	~tcp_full_t()							throw();

	/**************** Setup function	*******************************/
	tcp_full_t &	set_profile(const tcp_full_profile_t &profile)		throw();	
	tcp_full_t &	set_callback(tcp_full_cb_t *callback, void *userptr)	throw();
	inet_err_t	start()							throw();
	inet_err_t	start(tcp_full_cb_t *callback, void *userptr)		throw()
					{ return this->set_callback(callback,userptr).start();	}

	/**************** Query function	*******************************/
	bool			is_null()		const throw()	{ return local_addr.is_null();	}
	const ipport_addr_t &	get_local_addr()	const throw()	{ return local_addr;	}
	const ipport_addr_t &	get_remote_addr()	const throw()	{ return remote_addr;	}

	inet_err_t		recv_max_len_set(ssize_t recv_max_len)		throw();
	ssize_t			recv_max_len_get()				const throw();

	inet_err_t		maysend_set_threshold(ssize_t maysend_threshold)throw();
	ssize_t			maysend_get_threshold()				const throw();
	bool			maysend_is_set()				const throw();

	inet_err_t		sendbuf_set_max_len(ssize_t sendbuf_max_len)	throw();
	ssize_t			sendbuf_get_max_len()				const throw();
	ssize_t			sendbuf_get_used_len()				const throw();
	ssize_t			sendbuf_get_free_len()				const throw();
	bool			sendbuf_is_limited()				const throw();
	// send a datagram
	ssize_t			send(const void *pkt_data, size_t pkt_len) 	throw();

	/*************** Display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const tcp_full_t &tcp_full)	throw()
					{ return os << tcp_full.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_TCP_FULL_HPP__  */




