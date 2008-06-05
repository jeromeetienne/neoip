/*! \file
    \brief Header of the ippkt_util_t class
    
*/


#ifndef __NEOIP_IPPKT_UTIL_HPP__ 
#define __NEOIP_IPPKT_UTIL_HPP__ 
/* system include */
/* local include */
#include "neoip_pkt.hpp"
#include "neoip_datum.hpp"
#include "neoip_ip_addr.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief bunch of static function to build/parse ip packets
 */
class ippkt_util_t {
public:
	/*************** Query IP header fields	*******************************/
	static int		get_ip_version(const pkt_t &pkt)	throw();
	static ip_addr_t	get_src_addr(const pkt_t &pkt)		throw();
	static ip_addr_t	get_dst_addr(const pkt_t &pkt)		throw();
	/*************** Checksum Function	*******************************/
	static uint16_t		cpu_inet_csum(const datum_t &datum, uint16_t prev_csum = 0)	throw();
	static uint16_t		cpu_inet_csum(const void *ptr,size_t len,uint16_t prev_csum = 0)throw()
				{ return cpu_inet_csum(datum_t(ptr,len,datum_t::NOCOPY), prev_csum);	}
	/*************** Packet Building	*******************************/
	static pkt_t	build_icmp4_pkt(const ip_addr_t &src_addr, const ip_addr_t &dst_addr
						, uint8_t type, uint8_t code, uint32_t aux_value
						, const datum_t &payload)		throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_IPPKT_UTIL_HPP__  */



