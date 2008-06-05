/*! \file
    \brief Declaration of the neoip_socket_udp_dfull
*/


#ifndef __NEOIP_SOCKET_FULL_NTLAY_HPP__
#define __NEOIP_SOCKET_FULL_NTLAY_HPP__
/* system include */
#include <iostream>
/* local include */
#include "neoip_socket_helper_ntlay.hpp"
#include "neoip_socket_full_vapi.hpp"
#include "neoip_socket_full.hpp"
#include "neoip_socket_event.hpp"
#include "neoip_socket_profile.hpp"
#include "neoip_ntudp_full_cb.hpp"
#include "neoip_nlay_full.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	socket_full_t;
class	socket_profile_t;

/** \brief class for the \ref socket_full_t of the socket_domain_t::NTLAY
 */
class socket_full_ntlay_t : NEOIP_COPY_CTOR_DENY, public socket_full_vapi_t
				, public socket_stream_vapi_t, public socket_mtu_vapi_t
				, private ntudp_full_cb_t, private nlay_full_cb_t
				{
private:
	socket_type_t		m_socket_type;		//!< the socket_type_t for this initiator
	socket_profile_t	m_socket_profile;	//!< the socket_profile_t for this socket
	socket_addr_t		m_local_addr;		//!< the local socket_addr_t
	socket_addr_t		m_remote_addr;		//!< the remote socket_addr_t

	/*************** ntudp_full_t stuff	*******************************/
	ntudp_full_t *	ntudp_full;
	bool		neoip_ntudp_full_event_cb(void *userptr, ntudp_full_t &cb_ntudp_full
						, const ntudp_event_t &ntudp_event)	throw();

	/*************** nlay_full_t stuff	*******************************/
	nlay_full_t *	nlay_full;
	bool		neoip_nlay_full_event_cb(void *cb_userptr, nlay_full_t &cb_nlay_full
					, const nlay_event_t &nlay_event)	throw();

	
	/*************** closure stuff	***************************************/
	socket_linger_profile_t	linger_profile;	//!< a backup of the linger_profile used during closing
	socket_event_t		reported_fatal_event;	//!< used by socket_full_close_ntlay_t
							//!< to determine its behavious
	bool			notify_fatal_event(socket_event_t socket_event)	throw();

	/*************** callback stuff	***************************************/
	socket_full_vapi_cb_t *	callback;	//!< callback to notify event
	void *			userptr;	//!< userptr associated with the event callback
	bool			notify_callback(const socket_event_t &socket_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	socket_full_ntlay_t()							throw();
	~socket_full_ntlay_t()							throw();

	/*************** Setup function	***************************************/
	socket_err_t	set_ctor_param(ntudp_full_t *ntudp_full, nlay_full_t *nlay_full
					, const socket_profile_t &socket_profile)	throw();
	
	void		notify_glue_destruction()				throw();

	// used only by socket_resp_ntlay_t and socket_itor_ntlay_t
	// TODO: if so, why it isnt as friend and in private ?
	void 		start_closure()						throw();
	
	// function inherited from socket_full_vapi_t
	void		set_callback(socket_full_vapi_cb_t *callback, void *userptr)	throw();
	socket_err_t	start()								throw();


	void		rcvdata_maxlen(size_t recv_max_len)			throw();
	size_t		rcvdata_maxlen()					const throw();
	size_t		send(const void *data_ptr, size_t data_len) 		throw();

	
	/*************** various get	***************************************/
	const socket_domain_t &	domain()	const throw()	{ return socket_helper_ntlay_t::DOMAIN_VAR;}
	const socket_type_t &	type()		const throw()	{ return m_socket_type;		}		
	const socket_profile_t &profile()	const throw()	{ return m_socket_profile;	}
	const socket_addr_t &	local_addr()	const throw()	{ return m_local_addr;		}
	const socket_addr_t &	remote_addr()	const throw()	{ return m_remote_addr;		}

	/*************** socket_stream_vapi_t accessor	***********************/
	const socket_stream_vapi_t*stream_vapi()		const throw()	{ DBG_ASSERT(stream_vapi_avail()); return this;	}
	socket_stream_vapi_t *	stream_vapi()			throw()		{ DBG_ASSERT(stream_vapi_avail()); return this;	}
	bool			stream_vapi_avail()		const throw()	{ return type().is_reliable();			}
	/*************** socket_stream_vapi_t implementation	***************/
	void			maysend_tshold(size_t new_value)throw();
	size_t			maysend_tshold()		const throw();
	void			xmitbuf_maxlen(size_t new_value)throw();
	size_t			xmitbuf_maxlen()		const throw();
	size_t			xmitbuf_usedlen()		const throw();
	size_t			xmitbuf_freelen()		const throw();

	/*************** socket_mtu_vapi_t accessor	***********************/
	const socket_mtu_vapi_t*mtu_vapi()			const throw()	{ DBG_ASSERT(mtu_vapi_avail()); return this;	}
	socket_mtu_vapi_t *	mtu_vapi()			throw()		{ DBG_ASSERT(mtu_vapi_avail()); return this;	}
	bool			mtu_vapi_avail()		const throw()	{ return type().is_datagram();			}
	/*************** socket_mtu_vapi_t implementation	***************/
	void			mtu_pathdisc(bool onoff)	throw();
	bool			mtu_pathdisc()			const throw();
	size_t			mtu_overhead()			const throw();
	size_t			mtu_outter()			const throw();
	size_t			mtu_inner()			const throw();

	/*************** Display function	*******************************/
	std::string	to_string()					const throw();	
	
	/*************** helper	***********************************************/
	static socket_full_ntlay_t &		from_socket(socket_full_t &socket_full)		throw();
	static const socket_full_ntlay_t &	from_socket(const socket_full_t &socket_full)	throw();

	/*************** socket_full_ntlay_api_t function	***************/
	const nlay_full_upapi_t &	nlay()		const throw();
	nlay_full_upapi_t &		nlay()		throw();
	
	/*************** definition for the factory creation	***************/
	FACTORY_PRODUCT_DECLARATION_NOCLONE(socket_full_vapi_t, socket_full_ntlay_t);

	/*************** List of friend class	*******************************/
	friend class	socket_full_close_ntlay_t;
	friend class	socket_resp_ntlay_t;
	friend class	socket_itor_ntlay_t;	
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_FULL_NTLAY_HPP__ */



