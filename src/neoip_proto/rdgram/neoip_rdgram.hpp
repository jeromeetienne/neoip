/*! \file
    \brief Header of the \ref rdgram_t

- see \ref neoip_rdgram_t.cpp
*/


#ifndef __NEOIP_RDGRAM_HPP__ 
#define __NEOIP_RDGRAM_HPP__ 
/* system include */
#include <map>
#include <list>
/* local include */
#include "neoip_rdgram_cb.hpp"
#include "neoip_rdgram_err.hpp"
#include "neoip_rdgram_event.hpp"
#include "neoip_wai.hpp"
#include "neoip_pkt.hpp"
#include "neoip_timeout.hpp"
#include "neoip_timer_expboff.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief A layer to allow datagram reliability
 * 
 * - This doesn't remove duplicate in reception.
 *   - so if the remote peer retransmit a packet several times, it will be delivered several times
 *   - TODO put a slidwin to determine which packet has been already delivered
 *   - with a sanity check to be sure all the packets has been received, thus detect if the slidwin
 *     is too short
 *   - the slidewin size will be expressed in nb of datagram and not in number of byte
 *     - so the tuning of this size MUST be large enougth
 *       - if it isnt, the result will still be correct but the performance will be reduced
 *     - the slidwin MUST NOT advance until all packets has been received
 *     - if the slidewin cant advance don't ack the packet
 *   - make its size or presence tunable
 */
class rdgram_t : public zerotimer_cb_t, NEOIP_COPY_CTOR_DENY {
private:
	// declaration internal constant
	static const wai32_t	FIRST_SEQNB;

	// define the sendbuf packet and storage
	class		sendbuf_pkt_t;
	std::map<wai32_t, sendbuf_pkt_t *>	sendbuf_pkt_db;
	
	wai32_t		next_xmit_seqnb;	//!< the next sequence number to xmit

	size_t		sendbuf_used_len;	//!< the used length of the sendbuf
	size_t		sendbuf_max_len;	//!< the max length of the sendbuf (0 == unlimited)
	size_t		maysend_threshold;

	rdgram_cb_t *	callback;		//!< callback used to notify event
	void *		userptr;		//!< the userptr associated with the callback

	// seqnb2ack stuff
	// - it is sent thru zerotimer_t to avoid reccursive callback notification
	std::list<wai32_t>	seqnb2ack_list;
	zerotimer_t	zerotimer_seqnb2ack;
	bool		zerotimer_seqnb2ack_process()	throw();

	// maysend notification stuff
	// - it is sent thru zerotimer_t to avoid reccursive callback notification
	zerotimer_t	zerotimer_maysend;
	bool		zerotimer_maysend_process()	throw();
	
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();

	// maintain the sendbuf
	void		sendbuf_add_pkt( size_t pkt_len )		throw();
	void		sendbuf_del_pkt( size_t pkt_len )		throw();

	// packet recv/build
	rdgram_err_t	recv_data_pkt(pkt_t &pkt, wai32_t &seqnb_out)	throw();
	rdgram_err_t	recv_ack_pkt(pkt_t &pkt)			throw();
	pkt_t		build_ack(wai32_t seqnb)			throw();
	

	sendbuf_pkt_t *	find_sendbuf_pkt(wai32_t seqnb)		throw();
public:
	// ctor/dtor
	rdgram_t()	throw();
	~rdgram_t()	throw();

	// maysend_threshold parameter set/get
	rdgram_err_t	maysend_set_threshold(size_t max_len)	throw();
	size_t		maysend_get_threshold()			const throw();
	bool		maysend_is_set()		const throw();

	// sendbuf parameter set/get
	rdgram_err_t	sendbuf_set_max_len(size_t max_len)	throw();
	size_t		sendbuf_get_max_len()			const throw();
	size_t		sendbuf_get_used_len()			const throw();
	size_t		sendbuf_get_free_len()			const throw();
	bool		sendbuf_is_limited()			const throw();

	rdgram_err_t	set_callback(rdgram_cb_t * callback, void *userptr)	throw();
	rdgram_err_t	start()							throw();
	void		stop()							throw();

	// helper function for start()
	rdgram_err_t	start(rdgram_cb_t *callback, void *userptr)	throw();

	// packet processing
	rdgram_err_t	pkt_from_upper(pkt_t &pkt)			throw();
	rdgram_err_t	pkt_from_lower(pkt_t &pkt, wai32_t &seqnb_out)	throw();
	size_t		get_mtu_overhead()				const throw();
	
friend	class ordgram_t;
};

/** \brief handle a individual packet for rdgram_t send buffer
 */
class rdgram_t::sendbuf_pkt_t : public timeout_cb_t, NEOIP_COPY_CTOR_ALLOW {
private:
	wai32_t		seqnb;			//!< the sequence number of this packet
	pkt_t		pkt;			//!< the packet to send

	timeout_t	rxmit_timeout;		//!< retransmition timer to rxmit
	timer_expboff_t	rxmit_timer_policy;	//!< the timer policy for the retransmission
	bool		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();

	rdgram_t *	rdgram;			//!< backpointer on the main struct
public:
	sendbuf_pkt_t(const pkt_t &pkt, const wai32_t &seqnb, rdgram_t *rdgram)	throw();
	~sendbuf_pkt_t()								throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RDGRAM_HPP__  */



