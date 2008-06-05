/*! \file
    \brief Header of the \ref rdgram_t callback

*/


#ifndef __NEOIP_RDGRAM_CB_HPP__ 
#define __NEOIP_RDGRAM_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_wai.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	rdgram_t;
class	rdgram_event_t;
class	pkt_t;

/** \brief the callback class for \ref rdgram_t
 */
class rdgram_cb_t {
public:
	/** \brief callback notified when \ref rdgram_t send an event.
	 * 
	 * @return true if the rdgram_t is still valid after the callback, false otherwise
	 */
	virtual bool neoip_rdgram_event_cb(void *cb_userptr, rdgram_t &cb_rdgram
						, const rdgram_event_t &rdgram_event )	throw() = 0;

	/** \brief callback notified when \ref rdgram_t just receive a packet and wish to know
	 *         if this packet must be acknowledged or not.
	 * 
	 * - it isnt allowed to delete rdgram_t during this callback
	 * 
	 * @return true if the packet must be acked, false otherwise
	 */	
	virtual bool neoip_rdgram_should_acked_pkt_cb(void *cb_userptr, rdgram_t &cb_rdgram
					, const pkt_t &pkt, const wai32_t &seqnb)	throw() = 0;
	virtual ~rdgram_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_RDGRAM_CB_HPP__  */



