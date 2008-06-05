/*! \file
    \brief Header of the nlay_reachpeer_t

*/


#ifndef __NEOIP_NLAY_REACHPEER_HPP__
#define __NEOIP_NLAY_REACHPEER_HPP__

/* system include */
/* local include */
#include "neoip_reachpeer.hpp"
#include "neoip_nlay_nego.hpp"
#include "neoip_nlay_full_api.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief nlay glue for \ref reachpeer_t
 */
class nlay_reachpeer_t : private reachpeer_cb_t, public nlay_full_api_t {
private:
	reachpeer_t	reachpeer;
	
	bool		neoip_reachpeer_event_cb(void *cb_userptr, reachpeer_t &cb_reachpeer
					, const reachpeer_event_t &reachpeer_event ) throw();
public:
	nlay_reachpeer_t()	throw();
	~nlay_reachpeer_t()	throw();
	
	nlay_err_t	set_reliable_outter_cnx(bool value = true)	throw();
	
	// function inherited from nlay_full_api_t
	void	register_handler(nlay_regpkt_t *regpkt_lower)throw();
	nlay_err_t	set_parameter(const nlay_profile_t *nlay_profile
				, const nlay_nego_result_t *nego_result)	throw();	
	nlay_err_t	start()							throw();
	void		start_closure()						throw();
	nlay_err_t	pkt_from_lower(pkt_t &pkt)				throw();
	NEOIP_NLAY_FULL_API_CTORDTOR(nlay_reachpeer_t);
};

NEOIP_NAMESPACE_END


#endif // __NEOIP_NLAY_REACHPEER_HPP__ 


