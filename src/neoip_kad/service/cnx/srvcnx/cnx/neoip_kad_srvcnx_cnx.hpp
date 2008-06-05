/*! \file
    \brief Declaration of the kad_srvcnx_t

*/


#ifndef __NEOIP_KAD_SRVCNX_CNX_HPP__ 
#define __NEOIP_KAD_SRVCNX_CNX_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_srvcnx.hpp"
#include "neoip_udp_full_cb.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief handle a given connection received by the kad_srvcnx_t
 */
class kad_srvcnx_t::cnx_t : private udp_full_cb_t {
private:
	kad_srvcnx_t *	kad_srvcnx;	//!< backpointer to the kad_srvcnx_t
	datum_t		build_pathid()	const throw();
		
	/*************** udp_full_t	***************************************/
	udp_full_t *	udp_full;
	bool		neoip_inet_udp_full_event_cb(void *userptr, udp_full_t &cb_udp_full
						, const udp_event_t &udp_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	cnx_t(kad_srvcnx_t *kad_srvcnx, udp_full_t *udp_full)	throw();
	~cnx_t()						throw();

	/*************** start() function	*******************************/
	kad_err_t	start()					throw();
};


NEOIP_NAMESPACE_END

#endif // __NEOIP_KAD_SRVCNX_CNX_HPP__ 



