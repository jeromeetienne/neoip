/*! \file
    \brief Header of the \ref pktfrag_t

- see \ref neoip_pktfrag_t.cpp
*/


#ifndef __NEOIP_PKTFRAG_HPP__ 
#define __NEOIP_PKTFRAG_HPP__ 
/* system include */
#include <map>
/* local include */
#include "neoip_pktfrag_err.hpp"
#include "neoip_pktfrag_fraglist.hpp"
#include "neoip_pktfrag_profile.hpp"
#include "neoip_timeout.hpp"
#include "neoip_pkt.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief A layer to allow datagram fragmentation and reassembly
 * 
 * - It support limited or unlimited pool size
 * - It support to timeout or not the pending datagrams
 */
class pktfrag_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! the default MTU size for \ref pktfrag_t
	static const size_t	OUTTER_MTU_DFL;
	//! the default pool max size for \ref pktfrag_t
	static const size_t	POOL_MAX_SIZE_DFL;
	//! the default expiration time of a pending datagram for \ref pktfrag_t
	static const delay_t	PENDING_DGRAM_EXPIRE_DFL;

private:
	uint32_t	next_xmit_seqnb;	//!< the datagram seqnb for xmit
	
	// TODO store all those a profile
	size_t		outter_mtu;		//!< the maximum size for the packet of the lower layer
						//!< (the size of the fragment header is part of it)
	delay_t		pending_dgram_expire;
	size_t		pool_max_size;		//!< the maxium amount of memory that fragment can use
						//!< (0 mean infinite)


	size_t		cpu_fragment_hd_len()			const throw();
	pktfrag_err_t	recv_pkt_fragment(pkt_t &pkt)		throw();

	/*************** pending_dgram_db	*******************************/
	class						pending_dgram_t;
	typedef std::map<uint32_t, pending_dgram_t *>	pending_dgram_db_t;
	pending_dgram_db_t	pending_dgram_db;
	void 			pending_dgram_dolink(pending_dgram_t *pending_dgram) 	throw();
	void 			pending_dgram_unlink(pending_dgram_t *pending_dgram) 	throw();
	
	/*************** reasm pool	***************************************/
	void		pool_drain(size_t needed_len)	throw();
	size_t		pool_cur_size;		//!< the current size of the reasm pool (valid even
						//!< if pool_max_size == 0)
public:
	/*************** ctor/dtor	***************************************/
	pktfrag_t()	throw();
	~pktfrag_t()	throw();

	/*************** Setup function	***************************************/
	pktfrag_err_t	set_outter_mtu(size_t new_outter_mtu)			throw();
	// TODO change to the usual set_profile function
	pktfrag_err_t	set_from_profile(const pktfrag_profile_t &profile)	throw();
	
	/*************** Query function	***************************************/
	size_t		get_outter_mtu()	const throw()	{ return outter_mtu;		}
	bool		pool_size_is_limited()	const throw()	{ return pool_max_size != 0;	}
	size_t		pool_cur_size_get()	const throw()	{ return pool_cur_size;		}

	/*************** action function	*******************************/
	pktfrag_fraglist_t	pkt_from_upper(pkt_t &pkt, uint32_t seqnb)	throw();
	pktfrag_fraglist_t	pkt_from_upper(pkt_t &pkt)			throw();
	pktfrag_err_t		pkt_from_lower(pkt_t &pkt)			throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_PKTFRAG_HPP__  */



