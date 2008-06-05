/*! \file
    \brief Header of the \ref rdgram_t

- see \ref neoip_rdgram_t.cpp
*/


#ifndef __NEOIP_ORDGRAM_CB_HPP__ 
#define __NEOIP_ORDGRAM_CB_HPP__ 
/* system include */
#include <map>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	ordgram_t;
class	ordgram_event_t;

/** \brief the callback class for \ref rdgram_t
 */
class ordgram_cb_t {
public:
	/** \brief callback notified when \ref rdgram_t send an event.
	 * 
	 * @return true if the ordgram_t is still valid after the callback, false otherwise
	 */
	virtual bool neoip_ordgram_event_cb(void *cb_userptr, ordgram_t &cb_rdgram
					, const ordgram_event_t &ordgram_event )	throw() = 0;
	virtual ~ordgram_cb_t() {};
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ORDGRAM_CB_HPP__  */



