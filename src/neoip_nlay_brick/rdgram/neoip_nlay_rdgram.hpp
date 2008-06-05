/*! \file
    \brief Header of the nlay_rdgram_t

*/


#ifndef __NEOIP_NLAY_RDGRAM_HPP__
#define __NEOIP_NLAY_RDGRAM_HPP__

/* system include */
#include <list>
/* local include */
#include "neoip_rdgram.hpp"
#include "neoip_nlay_nego.hpp"
#include "neoip_nlay_full_api.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

class nlay_rdgram_t : private rdgram_cb_t, public nlay_full_api_t {
private:
	rdgram_t	rdgram;
	
	// rdgram callbacks
	bool neoip_rdgram_event_cb(void *cb_userptr, rdgram_t &cb_rdgram
					, const rdgram_event_t &rdgram_event )		throw();
	bool neoip_rdgram_should_acked_pkt_cb(void *cb_userptr, rdgram_t &cb_rdgram
					, const pkt_t &pkt, const wai32_t &seqnb)	throw();	
public:
	// ctor/dtor
	nlay_rdgram_t()	throw();
	~nlay_rdgram_t()	throw();
	
	// reliability function	
	nlay_err_t	maysend_set_threshold(size_t threshold)		throw();
	size_t		maysend_get_threshold()				const throw();
	bool		maysend_is_set()			const throw();
	nlay_err_t	sendbuf_set_max_len(size_t sendbuf_max_len)	throw();
	size_t		sendbuf_get_max_len()				const throw();
	size_t		sendbuf_get_used_len()				const throw();
	size_t		sendbuf_get_free_len()				const throw();
	bool		sendbuf_is_limited()				const throw();	
	
	// function inherited from nlay_full_api_t
	void		register_handler(nlay_regpkt_t *regpkt_lower)	throw();
	nlay_err_t	start()						throw();
	void		start_closure()					throw();	
	nlay_err_t	pkt_from_upper(pkt_t &pkt)			throw();
	nlay_err_t	pkt_from_lower(pkt_t &pkt)			throw();
	size_t		get_mtu_overhead()				const throw();
	NEOIP_NLAY_FULL_API_CTORDTOR(nlay_rdgram_t);
};

NEOIP_NAMESPACE_END


#endif // __NEOIP_NLAY_RDGRAM_HPP__ 


