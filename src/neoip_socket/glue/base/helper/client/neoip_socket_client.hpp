/*! \file
    \brief Header of the socket_client_t
*/


#ifndef __NEOIP_SOCKET_CLIENT_HPP__ 
#define __NEOIP_SOCKET_CLIENT_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_itor.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_client_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"
#include "neoip_copy_ctor_checker.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class socket_itor_t;
class socket_full_t;

/** \ref class to store the socket client
 * 
 * - it is a helper class which merge the use of socket_itor_t and socket_full_t
 *   which are typical for socket clients.
 */
class socket_client_t : NEOIP_COPY_CTOR_DENY, private socket_itor_cb_t
					, private socket_full_cb_t {
private:
	/*************** socket_itor_t	***************************************/
	socket_itor_t *	socket_itor;	//!< to initiate the connection
	bool		neoip_socket_itor_event_cb(void *userptr, socket_itor_t &cb_socket_itor
					, const socket_event_t &socket_event)	throw();

	/*************** socket_full_t	***************************************/
	socket_full_t *	socket_full;	//!< the socket_full_t struct
	bool		neoip_socket_full_event_cb(void *userptr, socket_full_t &cb_socket_full
					, const socket_event_t &socket_event)	throw();	
	void		recv_cnx_established(socket_full_t *socket_full)	throw();
	
	/*************** callback stuff	***************************************/
	socket_client_cb_t *	callback;	//!< the callback to notify
	void *			userptr;	//!< the userptr associated with the callback
	bool			notify_callback(const socket_event_t &socket_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	socket_client_t() 	throw();
	~socket_client_t() 	throw();

	/*************** Setup Function	***************************************/
	socket_client_t&set_profile(const socket_profile_t &socket_profile)	throw();
	socket_err_t	setup(const socket_domain_t &socket_domain, const socket_type_t &socket_type
					, const socket_addr_t &local_addr
					, const socket_addr_t &remote_addr
					, socket_client_cb_t *callback, void *userptr)	throw(); 
	socket_err_t	start()	throw()	{ DBG_ASSERT(socket_itor); return socket_itor->start();	}

	/*************** Setup Helper Function	*******************************/
	socket_err_t	start(const socket_domain_t &socket_domain, const socket_type_t &socket_type
					, const socket_addr_t &local_addr
					, const socket_addr_t &remote_addr
					, socket_client_cb_t *callback, void *userptr)	throw();
	socket_err_t	start(const socket_itor_arg_t &arg
					, socket_client_cb_t *callback, void *userptr)	throw();
	socket_err_t	setup(const socket_itor_arg_t &arg
					, socket_client_cb_t *callback, void *userptr)	throw();
				
	/*************** Query function	***************************************/
	bool		is_null()	const throw()	{ return !socket_itor && !socket_full;	}
	bool		is_itor()	const throw()	{ return socket_itor != NULL;		}
	bool		is_full()	const throw()	{ return !is_itor();			}
	socket_full_t *	steal_full()	throw();

	/*************** Get local/remote addresses - itor or full	*******/
	const socket_domain_t &	domain()	const throw()	{ if(socket_itor)	return socket_itor->domain();
								  else			return socket_full->domain(); 		}
	const socket_type_t &	type()		const throw()	{ if(socket_itor)	return socket_itor->type();
								  else			return socket_full->type(); 		}
	const socket_addr_t &	local_addr()	const throw()	{ if(socket_itor)	return socket_itor->local_addr();
								  else			return socket_full->local_addr();	}
	const socket_addr_t &	remote_addr()	const throw()	{ if(socket_itor)	return socket_itor->remote_addr();
								  else			return socket_full->remote_addr();	}
	/*************** Compatibility layer	*******************************/
	socket_domain_t	get_domain()		const throw()	{ return domain();	}
	socket_type_t	get_type()		const throw()	{ return type();	}
	socket_addr_t	get_local_addr()	const throw()	{ return local_addr();	}
	socket_addr_t	get_remote_addr()	const throw()	{ return remote_addr();	}


	void		rcvdata_maxlen(size_t new_value)	throw()		{ socket_full->rcvdata_maxlen(new_value);	}
	size_t		rcvdata_maxlen()			const throw()	{ return socket_full->rcvdata_maxlen();		}
	socket_err_t	send(const void *data_ptr, size_t data_len) throw()	{ return socket_full->send(data_ptr, data_len);	}
			
	// helper functions - socket_full_t only
	socket_err_t	send(const pkt_t &pkt) 					throw()
			{ DBG_ASSERT(socket_full); return socket_full->send(pkt.get_data(), pkt.get_len());}
	socket_err_t	send(const datum_t &datum) 				throw()
			{ DBG_ASSERT(socket_full); return socket_full->send(datum.get_data(), datum.get_len());	}
	/*************** Compatibility function	*******************************/
	socket_err_t	recv_max_len_set(size_t new_value)	throw()		{ rcvdata_maxlen(new_value); return socket_err_t::OK;	}
	size_t		recv_max_len_get()			const throw()	{ return rcvdata_maxlen();			}

	/*************** socket_stream_vapi_t forwarder	***********************/
	void		maysend_tshold(size_t new_value)	throw()		{ socket_full->maysend_tshold(new_value);	}
	size_t		maysend_tshold()			const throw()	{ return socket_full->maysend_tshold();		}
	void		xmitbuf_maxlen(size_t new_value)	throw()		{ socket_full->xmitbuf_maxlen(new_value);	}
	size_t		xmitbuf_maxlen()			const throw()	{ return socket_full->xmitbuf_maxlen();		}
	size_t		xmitbuf_usedlen()			const throw()	{ return socket_full->xmitbuf_usedlen();	}
	size_t		xmitbuf_freelen()			const throw()	{ return socket_full->xmitbuf_freelen();	}

	/*************** Compatibility layer	*******************************/
	socket_err_t	maysend_set_threshold(size_t new_value)	throw()		{ maysend_tshold(new_value); return socket_err_t::OK;	}
	size_t		maysend_get_threshold()			const throw()	{ return maysend_tshold();				}
	bool		maysend_is_set()			const throw()	{ return maysend_tshold() != socket_full_t::UNLIMITED;	}
	socket_err_t	sendbuf_set_max_len(size_t new_value)	throw()		{ xmitbuf_maxlen(new_value); return socket_err_t::OK;	}
	size_t		sendbuf_get_max_len()			const throw()	{ return xmitbuf_maxlen();				}
	size_t		sendbuf_get_used_len()			const throw()	{ return xmitbuf_usedlen();				}
	size_t		sendbuf_get_free_len()			const throw()	{ return xmitbuf_freelen();				}
	bool		sendbuf_is_limited()			const throw()	{ return xmitbuf_maxlen() != socket_full_t::UNLIMITED;	}

	/*************** Display Function	*******************************/
	std::string	to_string()				const throw();
	friend	std::ostream & operator << (std::ostream & os,const socket_client_t &socket_client)
								throw()		{ return os << socket_client.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_CLIENT_HPP__  */



