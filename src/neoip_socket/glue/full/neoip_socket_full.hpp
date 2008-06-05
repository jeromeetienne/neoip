/*! \file
    \brief Header of the socket_full_t
*/


#ifndef __NEOIP_SOCKET_FULL_HPP__ 
#define __NEOIP_SOCKET_FULL_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_full_wikidbg.hpp"
#include "neoip_socket_full_vapi.hpp"
#include "neoip_socket_full_vapi_cb.hpp"
#include "neoip_socket_full_cb.hpp"
#include "neoip_socket_rate_vapi.hpp"
#include "neoip_socket_mtu_vapi.hpp"
#include "neoip_socket_stream_vapi.hpp"
#include "neoip_socket_addr.hpp"
#include "neoip_socket_type.hpp"
#include "neoip_pkt.hpp"
#include "neoip_datum.hpp"
#include "neoip_delay.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_object_slotid.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration

/** \ref class to store the socket full
 */
class socket_full_t : NEOIP_COPY_CTOR_DENY, public object_slotid_t, private socket_full_vapi_cb_t
			, public socket_rate_vapi_t, public socket_mtu_vapi_t
			, private wikidbg_obj_t<socket_full_t, socket_full_wikidbg_init> {
public:
	//! a constant for unlimited value - used in maysend_tshold() and sendbuf_maxlen()
	static const size_t	UNLIMITED_VAL	= 0xFFFFFFFF;
	static const size_t	UNLIMITED;
private:
	/*************** socket_full_vapi_t	*******************************/
	socket_full_vapi_t *	m_full_vapi;	//!< virtual api pointing on the domain implementation
	bool			neoip_socket_full_vapi_cb(void *userptr, socket_full_vapi_t &cb_full_vapi
						, const socket_event_t &socket_event)		throw();

	/*************** callback stuff	***************************************/
	socket_full_cb_t *	callback;	//!< callback to notify event
	void *			userptr;	//!< userptr associated with the event callback
	bool			notify_callback(const socket_event_t &socket_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	socket_full_t(socket_full_vapi_t *p_full_vapi)	throw();
	~socket_full_t() 				throw();

	/*************** Setup Function	***************************************/
	socket_full_t &	set_callback(socket_full_cb_t *callback, void *	userptr)throw();
	socket_err_t	start()							throw()
				{ return full_vapi()->start();				}
	socket_err_t	start(socket_full_cb_t *callback, void * userptr)	throw()
				{ return set_callback(callback, userptr).start();	}

	/*************** get local/remote addresses funtions	***************/
	const socket_domain_t &	domain()	const throw()	{ return full_vapi()->domain();		}
	const socket_type_t &	type()		const throw()	{ return full_vapi()->type();		}
	const socket_profile_t &profile()	const throw()	{ return full_vapi()->profile();	}
	const socket_addr_t &	local_addr()	const throw()	{ return full_vapi()->local_addr();	}
	const socket_addr_t &	remote_addr()	const throw()	{ return full_vapi()->remote_addr();	}
	/*************** Compatibility Layer	*******************************/
	socket_addr_t	get_local_addr()	const throw()	{ return local_addr();			}
	socket_addr_t	get_remote_addr()	const throw()	{ return remote_addr();			}
	socket_domain_t	get_domain()		const throw()	{ return domain();			}
	socket_type_t	get_type()		const throw()	{ return type();			}

	/*************** reliability function	*******************************/
	void		rcvdata_maxlen(size_t new_value)	throw()		{ full_vapi()->rcvdata_maxlen(new_value);	}
	size_t		rcvdata_maxlen()			const throw()	{ return full_vapi()->rcvdata_maxlen();		}
	// TODO put the send stuff here

	/*************** Compatibility function	*******************************/
	socket_err_t	recv_max_len_set(size_t new_value)	throw()		{ rcvdata_maxlen(new_value); return socket_err_t::OK;	}
	size_t		recv_max_len_get()			const throw()	{ return rcvdata_maxlen();			}

	// TODO fix the send bullshit... some issue as the name is the same and and the 
	// return type is not... currently emulated with old stuff
	socket_err_t	send(const void *data_ptr, size_t data_len) 	throw()
				{ size_t sentlen= full_vapi()->send(data_ptr, data_len);
				  if( sentlen != data_len ) return socket_err_t::ERROR;
				  return socket_err_t::OK;			}
	// TODO those send() helper should be in the socket_common_vapi_t ? likely
	socket_err_t	send(const pkt_t &pkt) 			throw()		{ return send(pkt.void_ptr(), pkt.length());	}
	socket_err_t	send(const datum_t &datum) 		throw()		{ return send(datum.void_ptr(), datum.length());}

	/*************** socket_stream_vapi_t accessor	***********************/
	const socket_stream_vapi_t*stream_vapi()		const throw()	{ return full_vapi()->stream_vapi();		}
	socket_stream_vapi_t *	stream_vapi()			throw()		{ return full_vapi()->stream_vapi();		}
	bool			stream_vapi_avail()		const throw()	{ return full_vapi()->stream_vapi_avail();	}
	/*************** socket_stream_vapi_t forwarder	***********************/
	void		maysend_tshold(size_t new_value)	throw()		{ stream_vapi()->maysend_tshold(new_value);	}
	size_t		maysend_tshold()			const throw()	{ return stream_vapi()->maysend_tshold();	}
	void		xmitbuf_maxlen(size_t new_value)	throw()		{ stream_vapi()->xmitbuf_maxlen(new_value);	}
	size_t		xmitbuf_maxlen()			const throw()	{ return stream_vapi()->xmitbuf_maxlen();	}
	size_t		xmitbuf_usedlen()			const throw()	{ return stream_vapi()->xmitbuf_usedlen();	}
	size_t		xmitbuf_freelen()			const throw()	{ return stream_vapi()->xmitbuf_freelen();	}

	/*************** Compatibility layer	*******************************/
	socket_err_t	maysend_set_threshold(size_t new_value)	throw()		{ maysend_tshold(new_value); return socket_err_t::OK;	}
	size_t		maysend_get_threshold()			const throw()	{ return maysend_tshold();				}
	bool		maysend_is_set()			const throw()	{ return maysend_tshold() != UNLIMITED;			}
	socket_err_t	sendbuf_set_max_len(size_t new_value)	throw()		{ xmitbuf_maxlen(new_value); return socket_err_t::OK;	}
	size_t		sendbuf_get_max_len()			const throw()	{ return xmitbuf_maxlen();				}
	size_t		sendbuf_get_used_len()			const throw()	{ return xmitbuf_usedlen();				}
	size_t		sendbuf_get_free_len()			const throw()	{ return xmitbuf_freelen();				}
	bool		sendbuf_is_limited()			const throw()	{ return xmitbuf_maxlen() != UNLIMITED;			}
	
	/*************** socket_rate_vapi_t accessor	***********************/
	const socket_rate_vapi_t*rate_vapi()				const throw()	{ return full_vapi()->rate_vapi();		}
	socket_rate_vapi_t *	rate_vapi()				throw()		{ return full_vapi()->rate_vapi();		}
	bool			rate_vapi_avail()			const throw()	{ return full_vapi()->rate_vapi_avail();	}
	/*************** socket_rate_vapi_t forwarder	***********************/
	void			xmit_limit(const rate_limit_arg_t &limit_arg) throw()	{ rate_vapi()->xmit_limit(limit_arg);		}
	rate_limit_t &		xmit_limit()				throw()		{ return rate_vapi()->xmit_limit();		}
	const rate_limit_t &	xmit_limit()				const throw()	{ return rate_vapi()->xmit_limit();		}
	bool			xmit_limit_is_set()			const throw()	{ return rate_vapi()->xmit_limit_is_set();	}
	void			recv_limit(const rate_limit_arg_t &limit_arg) throw()	{ rate_vapi()->recv_limit(limit_arg);		}
	rate_limit_t &		recv_limit()				throw()		{ return rate_vapi()->recv_limit();		}
	const rate_limit_t &	recv_limit()				const throw()	{ return rate_vapi()->recv_limit();		}
	bool			recv_limit_is_set()			const throw()	{ return rate_vapi()->recv_limit_is_set();	}

	/*************** socket_mtu_vapi_t accessor	***********************/
	const socket_mtu_vapi_t*mtu_vapi()		const throw()	{ return full_vapi()->mtu_vapi();	}
	socket_mtu_vapi_t *	mtu_vapi()		throw()		{ return full_vapi()->mtu_vapi();	}
	bool			mtu_vapi_avail()	const throw()	{ return full_vapi()->mtu_vapi_avail();	}
	/*************** socket_mtu_vapi_t	*******************************/
	void			mtu_pathdisc(bool onoff)throw()		{ mtu_vapi()->mtu_pathdisc(onoff);	}
	bool			mtu_pathdisc()		const throw()	{ return mtu_vapi()->mtu_pathdisc();	}
	size_t			mtu_overhead()		const throw()	{ return mtu_vapi()->mtu_overhead();	}
	size_t			mtu_outter()		const throw()	{ return mtu_vapi()->mtu_outter();	}
	size_t			mtu_inner()		const throw()	{ return mtu_vapi()->mtu_inner();	}

	/*************** access domain specific api	***********************/
	socket_full_vapi_t *	full_vapi()	throw()		{ DBG_ASSERT(m_full_vapi); return m_full_vapi;	}
	const socket_full_vapi_t*full_vapi()	const throw()	{ DBG_ASSERT(m_full_vapi); return m_full_vapi;	}	
	
	/*************** Display Function	*******************************/
	std::string	to_string()			const throw()	{ return full_vapi()->to_string();	}
	friend std::ostream & operator << (std::ostream & os, const socket_full_t &socket_full)
							throw()		{ return os << socket_full.to_string();	}

	/*************** List of friend function	***********************/
	friend class	socket_full_wikidbg_t;	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_FULL_HPP__  */



