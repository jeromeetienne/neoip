/*! \file
    \brief Header of the \ref rdgram_t

- see \ref neoip_rdgram_t.cpp
*/


#ifndef __NEOIP_ORDGRAM_HPP__ 
#define __NEOIP_ORDGRAM_HPP__ 
/* system include */
#include <map>
/* local include */
#include "neoip_ordgram_cb.hpp"
#include "neoip_ordgram_event.hpp"
#include "neoip_rdgram.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief A layer to allow datagram reliability with ordered delivery
 */
class ordgram_t : public zerotimer_cb_t, public rdgram_cb_t, NEOIP_COPY_CTOR_DENY {
private:
	// declaration internal constant
	static const size_t	RECVBUF_MAX_LEN_DFL;
	
	rdgram_t	rdgram;

	// recvbuf stuff
	class		recvbuf_pkt_t;
	std::map<wai32_t, recvbuf_pkt_t>	recvbuf_pkt_db;
	size_t		recvbuf_max_len;
	size_t		recvbuf_used_len;
	
	wai32_t		next_seqnb_to_deliver;	//!< the sequence number of the next packet to deliver
	
	ordgram_cb_t *	callback;		//!< callback used to notify event
	void *		userptr;		//!< the userptr associated with the callback

	// zerotimer to deliver packet which have been received outoforder
	zerotimer_t	zerotimer_deliver;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();	
	// rdgram_t callback
	bool		neoip_rdgram_event_cb(void *cb_userptr, rdgram_t &cb_rdgram
						, const rdgram_event_t &rdgram_event )	throw();
	bool		neoip_rdgram_should_acked_pkt_cb(void *cb_userptr, rdgram_t &cb_rdgram
					, const pkt_t &pkt, const wai32_t &seqnb)	throw();

	// special handling for packet from lower when it is known to be outoforder
	void		pkt_from_lower_outoforder(pkt_t &pkt, wai32_t seqnb)	throw();
public:
	// ctor/dtor
	ordgram_t()	throw();
	~ordgram_t()	throw();
	
	// maysend_threshold parameter set/get
	rdgram_err_t	maysend_set_threshold(size_t max_len)	throw()
			{ return rdgram.maysend_set_threshold(max_len);	}
	size_t		maysend_get_threshold()			const throw()
			{ return rdgram.maysend_get_threshold();	}
	bool		maysend_is_set()		const throw()
			{ return rdgram.maysend_is_set();	}
	// sendbuf parameter set/get
	rdgram_err_t	sendbuf_set_max_len(size_t max_len)	throw()
			{ return rdgram.sendbuf_set_max_len(max_len);	}
	size_t		sendbuf_get_max_len()			const throw()
			{ return rdgram.sendbuf_get_max_len();		}
	size_t		sendbuf_get_used_len()			const throw()
			{ return rdgram.sendbuf_get_used_len();		}
	size_t		sendbuf_get_free_len()			const throw()
			{ return rdgram.sendbuf_get_free_len();		}
	bool		sendbuf_is_limited()			const throw()	
			{ return rdgram.sendbuf_is_limited();		}
	
	
	rdgram_err_t	set_callback(ordgram_cb_t * callback, void *userptr)	throw();
	rdgram_err_t	start()							throw();
	void		stop()							throw()
			{ return rdgram.stop();	}

	// helper function for start()
	rdgram_err_t	start(ordgram_cb_t *callback, void *userptr)		throw();


	// packet processing
	rdgram_err_t	pkt_from_upper(pkt_t &pkt)		throw();
	rdgram_err_t	pkt_from_lower(pkt_t &pkt)		throw();
	size_t		get_mtu_overhead()	const throw()	{ return rdgram.get_mtu_overhead(); }
};

/** \brief handle a individual packet recieved by \ref ordgram_t
 * 
 * - a packet is stored in such structure iif it is received outoforder
 */
class ordgram_t::recvbuf_pkt_t : NEOIP_COPY_CTOR_ALLOW {
private:
	wai32_t			seqnb;			//!< the sequence number of this packet
	pkt_t			pkt;			//!< the packet itself
public:
	recvbuf_pkt_t(const wai32_t &seqnb, const pkt_t &pkt)	throw()
	{
		this->seqnb	= seqnb;
		this->pkt	= pkt;
	}

	pkt_t &	get_pkt()	throw()		{ return pkt;	}
	wai32_t	get_seqnb()	const throw()	{ return seqnb;	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ORDGRAM_HPP__  */



