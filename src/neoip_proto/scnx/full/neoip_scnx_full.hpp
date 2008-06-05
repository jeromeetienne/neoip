/*! \file
    \brief Header of the \ref scnx_full_t

- see \ref neoip_scnx_full_t.cpp
*/


#ifndef __NEOIP_SCNX_FULL_HPP__ 
#define __NEOIP_SCNX_FULL_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_scnx_full_cb.hpp"
#include "neoip_scnx_full_xmit.hpp"
#include "neoip_scnx_full_recv.hpp"
#include "neoip_scnx_full_rkey.hpp"
#include "neoip_scnx_event.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief A layer to allow packet scrambling (e.g. packet drop/delay)
 */
class scnx_full_t : NEOIP_COPY_CTOR_DENY, public zerotimer_cb_t {
public:	////////////////// declaration internal constant //////////////////////
	// TODO to take from a profile
	static const uint32_t	SEQNB_ITOR_REKEY;
	static const uint32_t	SEQNB_SIG_RESERVED;
private:
	skey_ciph_type_t	ciph_type;
	skey_auth_type_t	auth_type;
	bool			reliable_outter_cnx;	//!< true if the outter connection is reliable.
							//!< false otherwise.
	datum_t			xmit_key;		//!< the key used for transmission
	datum_t			recv_key;		//!< the key used for reception

	std::string		local_idname;		//!< the local identity name
	std::string		remote_idname;		//!< the remote identity name

	scnx_full_xmit_t *	full_xmit;		//!< context to xmit packet
	scnx_full_recv_t *	full_recv;		//!< context to receive packet
	scnx_full_recv_t *	full_arcv;		//!< alternate context to receive packet
	scnx_full_rkey_t *	full_rkey;		//!< context to rekey

	/*************** estapkt_in/out	***************************************/
	pkt_t			estapkt_in;		//!< usefull to handle the last packet sent
							//!< by the responder during the connection
							//!< establishement over a unreliable connection
	pkt_t			estapkt_out;

	/*************** pkt_to_lower	***************************************/
	std::list<pkt_t>	pkt_to_lower_list;
	zerotimer_t		pkt_to_lower_zerotimer;
	bool			neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer,void *userptr)throw();
	bool			pkt_to_lower_sync_notify(pkt_t &pkt)		throw();
	void			pkt_to_lower_async_notify(pkt_t &pkt)		throw();
	
	/*************** Callback	***************************************/
	scnx_full_cb_t *callback;		//!< the event callback
	void *		userptr;		//!< userptr associated with the event callback
	bool		notify_callback(const scnx_event_t &scnx_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	scnx_full_t(const datum_t &master_key, const skey_ciph_type_t &ciph_type
				, const skey_auth_type_t &auth_type
				, bool is_itor_f, bool reliable_outter_cnx
				, const std::string &local_idname, const std::string &remote_idname
				, const pkt_t &estapkt_in, const pkt_t &estapkt_out)	throw();
	~scnx_full_t()						throw();
	
	/*************** Setup Function	***************************************/
	scnx_err_t	start(scnx_full_cb_t *callback, void *userptr)		throw();

	/*************** Query Function	***************************************/
	bool			is_reliable_outter_cnx()const throw();
	size_t			get_mtu_overhead()	throw();
	const std::string &	get_local_idname()	const throw()	{ return local_idname;	}
	const std::string &	get_remote_idname()	const throw()	{ return remote_idname;	}

	/*************** Action function	*******************************/
	scnx_err_t	pkt_from_upper(pkt_t &pkt)				throw();
	scnx_err_t	pkt_from_lower(pkt_t &pkt)				throw();

	/*************** Display function	*******************************/
	std::string	to_string()						const throw();
	friend	std::ostream & operator << (std::ostream & os, const scnx_full_t & scnx_full)	throw()
					{ return os << scnx_full.to_string();	}

	/*************** List of friend class	*******************************/
	friend class	scnx_full_rkey_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SCNX_FULL_HPP__  */



