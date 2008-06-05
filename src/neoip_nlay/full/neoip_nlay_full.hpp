/*! \file
    \brief Header of the \ref nlay_full_t

- see \ref neoip_nlay_full.cpp
*/


#ifndef __NEOIP_NLAY_FULL_HPP__ 
#define __NEOIP_NLAY_FULL_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_nlay_full_cb.hpp"
#include "neoip_nlay_full_upapi.hpp"
#include "neoip_nlay_profile.hpp"
#include "neoip_nlay_event.hpp"
#include "neoip_nlay_type.hpp"
#include "neoip_nlay_nego.hpp"
#include "neoip_nlay_regpkt.hpp"
#include "neoip_nlay_event.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	scnx_full_t;
class	nlay_closure_t;
class	nlay_dataize_t;
class	nlay_idletimeout_t;
class	nlay_ordgram_t;
class	nlay_pktcomp_t;
class	nlay_pktfrag_t;
class	nlay_reachpeer_t;
class	nlay_rdgram_t;
class	nlay_scnx_full_t;
class	nlay_simuwan_t;


/** \brief to handle the full in the nlay stack
 */
class nlay_full_t : NEOIP_COPY_CTOR_DENY, public nlay_full_upapi_t {
private:
	nlay_type_t	inner_type;	//!< the inner nlay_type_t of this nlay_full_t
	nlay_type_t	outter_type;	//!< the outter nlay_type_t of this nlay_full_t

	
	// internal functions
	nlay_err_t	pkt_from_upper_api(pkt_t &pkt, nlay_full_api_t *full_api_lower)		throw();
	nlay_err_t	pkt_from_lower_api(pkt_t &pkt, nlay_regpkt_t *first_regpkt)		throw();
	size_t		mtu_propagate(size_t most_outter_mtu, nlay_full_api_t *full_api)	throw();

// stack management/storage stuff
	nlay_regpkt_t *		lowest_regpkt;		//!< pointer on the lowest regpkt_t - used in 
							//!< pkt_from_lower()
	nlay_full_api_t *	uppest_full_api;	//!< pointer on the uppest nlay_full_api_t - used
							//!< int pkt_from_upper()
	// to store the nlay_full_api_t stack
	std::vector<nlay_regpkt_t>	regpkt_db;	//!< the database packet registery in no particular
							//!< order (see lowest_regpkt)
	std::list<nlay_full_api_t *>	full_api_db;	//!< the database of nlay_full_api_t in no
							//!< particular order (see uppest_full_api)

	/*************** stack construction/destruction	***********************/
	void		stack_build_outter_dgram(scnx_full_t *scnx_full)	throw();
	nlay_err_t	stack_set_parameter(const nlay_profile_t *nlay_profile
				, const nlay_nego_result_t *nego_result)	throw();
	nlay_err_t	stack_start()						throw();
	void		stack_destroy()						throw();
	void		stack_start_closure()					throw();

	// special ptr on nlay_full_api_t which require direct access (and not only the nlay_full_api_t)
	nlay_closure_t *nlay_closure;
	nlay_scnx_full_t *nlay_scnx_full;
	nlay_full_api_t*nlay_reliability;	//!< short cut on the nlay brick handling the reliability

	/************** event notification from nlay_full_api_t ***************/
	bool		notify_event(const nlay_event_t &nlay_event)				throw();
	bool		notify_pkt_to_lower(pkt_t &pkt, nlay_full_api_t *full_api_lower)	throw();
	bool		notify_pkt_to_upper(pkt_t &pkt, nlay_regpkt_t *regpkt_upper)		throw();

	/*************** callback	***************************************/
	nlay_full_cb_t *callback;	//!< the callback to notify events
	void *		userptr;	//!< the userptr associated with the event callback
	bool		notify_callback(const nlay_event_t &nlay_event)		throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	nlay_full_t()	throw();
	~nlay_full_t()	throw();	
	nlay_full_t(const nlay_profile_t *nlay_profile, const nlay_nego_result_t *nego_result
				, const nlay_type_t &inner_type, const nlay_type_t &outter_type
				, scnx_full_t *scnx_full)			throw();

	/*************** setup function	***************************************/
	nlay_err_t	start(nlay_full_cb_t *callback, void *userptr)		throw();
	
	/*************** Query function	***************************************/
	bool		is_null()		const throw()	{ return full_api_db.empty();	}
	nlay_type_t	get_inner_type()	const throw()	{ return inner_type;		}
	
	

	// reliability function	
	nlay_err_t	recv_max_len_set(size_t recv_max_len)		throw();
	size_t		recv_max_len_get()				const throw();
	nlay_err_t	maysend_set_threshold(size_t threshold)		throw();
	size_t		maysend_get_threshold()				const throw();
	bool		maysend_is_set()				const throw();
	nlay_err_t	sendbuf_set_max_len(size_t sendbuf_max_len)	throw();
	size_t		sendbuf_get_max_len()				const throw();
	size_t		sendbuf_get_used_len()				const throw();
	size_t		sendbuf_get_free_len()				const throw();
	bool		sendbuf_is_limited()				const throw();
	
	// packet processing acting as filter from caller only
	void		start_closure()			throw();	
	nlay_err_t	pkt_from_upper(pkt_t &pkt)	throw();
	nlay_err_t	pkt_from_lower(pkt_t &pkt)	throw();
	
	void		mtu_outter(size_t new_mtu)	throw();
	size_t		mtu_overhead()			const throw();
	
	
	/*************** nlay_full_upapi_t function	***********************/
	const nlay_scnx_full_api_t &	scnx()		const throw();
	nlay_scnx_full_api_t &		scnx()		throw();
	
	/*************** List of friend class	*******************************/
	// - just to allow them to use the notify_* functions, nothing MORE!
	// - thus the nlay_full_t's caller is unable to use them
	friend class nlay_dataize_t;	friend class nlay_ordgram_t;	friend class nlay_pktcomp_t;
	friend class nlay_pktfrag_t;	friend class nlay_rdgram_t;	friend class nlay_simuwan_t;
	friend class nlay_idletimeout_t;friend class nlay_reachpeer_t;	friend class nlay_scnx_full_t;
	friend class nlay_closure_t;	friend class nlay_rate_limit_t;	friend class nlay_bstream2dgram_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NLAY_FULL_HPP__  */



