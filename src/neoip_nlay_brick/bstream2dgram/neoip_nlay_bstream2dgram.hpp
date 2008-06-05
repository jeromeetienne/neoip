/*! \file
    \brief Header of the nlay_bstream2dgram_t

*/


#ifndef __NEOIP_NLAY_BSTREAM2DGRAM_HPP__
#define __NEOIP_NLAY_BSTREAM2DGRAM_HPP__

/* system include */
#include <list>
/* local include */
#include "neoip_nlay_nego.hpp"
#include "neoip_nlay_full_api.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

class nlay_bstream2dgram_t : public nlay_full_api_t {
private:
	size_t		outter_mtu;
	size_t		inner_mtu;
	pkt_t		recved_data;
public:
	nlay_bstream2dgram_t()	throw();
	~nlay_bstream2dgram_t()	throw();
	
	// function inherited from nlay_full_api_t
	void		register_handler(nlay_regpkt_t *regpkt_lower)		throw();
	nlay_err_t	pkt_from_upper(pkt_t &pkt)				throw();
	nlay_err_t	pkt_from_lower(pkt_t &pkt)				throw();
	nlay_err_t	set_outter_mtu(size_t outter_mtu)			throw();
	NEOIP_NLAY_FULL_API_CTORDTOR(nlay_bstream2dgram_t);
};

NEOIP_NAMESPACE_END


#endif // __NEOIP_NLAY_BSTREAM2DGRAM_HPP__ 


