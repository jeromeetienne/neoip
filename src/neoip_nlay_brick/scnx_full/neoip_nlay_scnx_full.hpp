/*! \file
    \brief Header of the nlay_scnx_full_t

*/


#ifndef __NEOIP_NLAY_SCNX_FULL_HPP__
#define __NEOIP_NLAY_SCNX_FULL_HPP__

/* system include */
/* local include */
#include "neoip_scnx_full_cb.hpp"
#include "neoip_nlay_scnx_full_api.hpp"
#include "neoip_nlay_full_api.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief nlay brick handling the scnx_full_t
 */
class nlay_scnx_full_t : private scnx_full_cb_t
				, public nlay_full_api_t
				, public nlay_scnx_full_api_t {
private:
	/*************** scnx_full_t	***************************************/
	scnx_full_t *	scnx_full;
	bool		neoip_scnx_full_event_cb(void *cb_userptr, scnx_full_t &cb_scnx_full
							, const scnx_event_t &scnx_event) throw();
public:
	/*************** ctor/dtor	***************************************/
	nlay_scnx_full_t()	throw();
	~nlay_scnx_full_t()	throw();
	
	// TODO to put this parameter in the ctor ?
	nlay_err_t	set_scnx_full(scnx_full_t *scnx_full)			throw();
	
	/*************** nlay_scnx_full_api_t function	***********************/
	const std::string &	get_local_idname()	const throw();
	const std::string &	get_remote_idname()	const throw();	
	
	/*************** nlay_full_api_t function	***********************/
	void		register_handler(nlay_regpkt_t *regpkt_lower)		throw();	
	nlay_err_t	start()							throw();
	nlay_err_t	pkt_from_upper(pkt_t &pkt)				throw();
	nlay_err_t	pkt_from_lower(pkt_t &pkt)				throw();
	size_t		get_mtu_overhead()					const throw();
	NEOIP_NLAY_FULL_API_CTORDTOR(nlay_scnx_full_t);
};

NEOIP_NAMESPACE_END


#endif // __NEOIP_NLAY_SCNX_FULL_HPP__ 


