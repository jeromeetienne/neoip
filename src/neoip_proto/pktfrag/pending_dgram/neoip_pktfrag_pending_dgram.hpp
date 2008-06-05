/*! \file
    \brief Header of the \ref pktfrag_t

- see \ref neoip_pktfrag.cpp
*/


#ifndef __NEOIP_PKTFRAG_PENDING_DGRAM_HPP__ 
#define __NEOIP_PKTFRAG_PENDING_DGRAM_HPP__ 
/* system include */
#include <map>
/* local include */
#include "neoip_pktfrag.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief Handle a pending datagram for \ref pktfrag_t
 */
class	pktfrag_t::pending_dgram_t : NEOIP_COPY_CTOR_ALLOW, public timeout_cb_t {
private:
	pktfrag_t *	pktfrag;	//!< backpointer to the \ref pktfrag_t
	uint32_t	seqnb;		//!< the sequence number of this pending datagram
	size_t		full_dgram_len;	//!< the length of the complete datagram (==0 means it isnt
					//!< yet known aka the last fragment has yet been received)

	bool		is_complete()		const throw();
	

	/*************** expire_timeout	***************************************/
	timeout_t	expire_timeout;	//!< the expiration timeout
	bool		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	/*************** fragment_db	***************************************/
	typedef	std::map<uint32_t, pkt_t>	fragment_db_t;
	fragment_db_t	fragment_db;	//!< contain all the fragment for this dgram 
public:
	/*************** ctor/dtor	***************************************/
	pending_dgram_t()						throw() : pktfrag(NULL)	{}
	pending_dgram_t(pktfrag_t *pktfrag, uint32_t seqnb)		throw();
	~pending_dgram_t()						throw();

	/*************** Query function	***************************************/
	uint32_t	get_seqnb()			const throw()	{ return seqnb;	}
	pkt_t		get_complete_dgram()		const throw();	
	
	/*************** action function	******************************/
	bool		add_fragment(uint32_t offset, bool more_bit, const pkt_t &fragment)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_PKTFRAG_PENDING_DGRAM_HPP__  */



