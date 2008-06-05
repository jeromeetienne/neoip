/*! \file
    \brief Definition of the \ref ippkt_util_t class

*/

/* system include */
#include <netinet/ip.h>
#include <netinet/ip_icmp.h>
/* local include */
#include "neoip_ippkt_util.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        Query ip header field
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief return the version of the ip packet passed in parameter
 * 
 * @return the version number (4 or 6), or 0 if an error occurs
 */
int	ippkt_util_t::get_ip_version(const pkt_t &ip_pkt)			throw()
{
	// if the packet is not long enought, to contain an ip version, return 0
	if( ip_pkt.get_len() == 0 )	return 0;
	// get a pointer on the version
	uint8_t	*	ptr	= (uint8_t *)ip_pkt.get_data();
	// extract the version number
	int		version	= ((*ptr) >> 4) & 0xF;
	// if the version is NOT 4 or 6, return 0
	if( version != 4 && version != 6 )	return 0;
	// else return the version number
	return version;
}

/** \brief return the source ip address of the ip packet passed in parameter
 * 
 * @return the source ip address or .is_null() if an error occured
 */
ip_addr_t	ippkt_util_t::get_src_addr(const pkt_t &ip_pkt)			throw()
{
	// set the ip header pointer
	struct iphdr *	iph	= (struct iphdr *)ip_pkt.get_data();
	// if the packet is not large enougth to contain a ip header, return a null ip_addr_t
	if( ip_pkt.get_len() < sizeof(*iph) )		return ip_addr_t();
	// if the packet is not a IPv4 packet, return a null ip_addr_t
	if( ippkt_util_t::get_ip_version(ip_pkt) != 4)	return ip_addr_t();
	// return the source ip_addr_t
	return ip_addr_t( ntohl(iph->saddr) );
}

/** \brief return the destination ip address of the ip packet passed in parameter
 * 
 * @return the destination ip address or .is_null() if an error occured
 */
ip_addr_t	ippkt_util_t::get_dst_addr(const pkt_t &ip_pkt)			throw()
{
	// set the ip header pointer
	struct iphdr *	iph	= (struct iphdr *)ip_pkt.get_data();
	// if the packet is not large enougth to contain a ip header, return a null ip_addr_t
	if( ip_pkt.get_len() < sizeof(*iph) )		return ip_addr_t();
	// if the packet is not a IPv4 packet, return a null ip_addr_t
	if( ippkt_util_t::get_ip_version(ip_pkt) != 4)	return ip_addr_t();
	// return the destination ip_addr_t
	return ip_addr_t( ntohl(iph->daddr) );
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        internet checksum computation
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Update a inet checksum
 */
uint16_t ippkt_util_t::cpu_inet_csum(const datum_t &datum, uint16_t csum)	throw()
{
	int 		nb_left	= datum.get_len();
	uint16_t *	w	= (uint16_t*)datum.get_data();
	int 		sum	= csum;
	uint16_t	answer;

	/*
	 *  Our algorithm is simple, using a 32 bit accumulator (sum),
	 *  we add sequential 16 bit words to it, and at the end, fold
	 *  back all the carry bits from the top 16 bits into the lower
	 *  16 bits.
	 */
	while( nb_left > 1 ){
		sum	+= *w++;
		nb_left	-= 2;
	}

	// mop up an odd byte, if necessary
	if( nb_left == 1 )	sum += htons(*(u_char *)w << 8);

	// add back carry outs from top 16 bits to low 16 bits
	sum	 = (sum >> 16) + (sum & 0xffff);
	sum	+= sum >> 16;
	answer	 = ~sum;
	// return the result
	return answer;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       Packet Building
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Build a ICMPv4 packet
 */
pkt_t	ippkt_util_t::build_icmp4_pkt(const ip_addr_t &src_addr, const ip_addr_t &dst_addr
						, uint8_t type, uint8_t code, uint32_t aux_value
						, const datum_t &payload)		throw()
{
	char		buffer[64*1024];
	struct iphdr *	iph		= (struct iphdr *)buffer;
	size_t		ippayl_len	= payload.get_len() + sizeof(struct icmphdr) + sizeof(struct iphdr);
	struct icmphdr*	icmph;
	// sanity check - the src_addr MUST be IPv4
	DBG_ASSERT( src_addr.get_version() == 4 );
	// sanity check - the dst_addr MUST be IPv4
	DBG_ASSERT( dst_addr.get_version() == 4 );
	// sanity check - the resulting packet MUST NOT be larger than the buffer
	DBG_ASSERT( ippayl_len <= sizeof(buffer) );

	// build ip header
	iph->version	= 4;			// version
	iph->ihl	= 5;			// header lenght
	iph->tos	= 0;			// type of service
	iph->tot_len	= htons(ippayl_len);	// total length
	iph->frag_off	= 0;			// fragment offset field 
	iph->id		= 0;			// IP identificator 
	iph->ttl	= IPDEFTTL;
	iph->protocol	= IPPROTO_ICMP;
	iph->saddr	= htonl( src_addr.get_v4_addr() );
	iph->daddr	= htonl( dst_addr.get_v4_addr() );
	iph->check	= 0;
	// compute che checksum for the IP header - see rfc791.p14
	iph->check	= ippkt_util_t::cpu_inet_csum(iph, iph->ihl*4);

	// fill the icmp header for ECHO REQUEST - see rfc792.p14
	icmph		= (struct icmphdr *)(buffer + iph->ihl*4);
	icmph->type 	= type;
	icmph->code	= code;
	icmph->checksum	= 0;
	icmph->un.gateway= htonl(aux_value);

	// copy the data after the icmph
	memcpy( (char *)icmph + sizeof(*icmph), payload.get_data(), payload.get_len() );
	// compute the icmp checksum computed over the icmp header + data payload
	icmph->checksum	= ippkt_util_t::cpu_inet_csum( icmph, sizeof(*icmph) + payload.get_len() );

	// return the built packet
	return pkt_t(buffer, ippayl_len);
}

NEOIP_NAMESPACE_END


