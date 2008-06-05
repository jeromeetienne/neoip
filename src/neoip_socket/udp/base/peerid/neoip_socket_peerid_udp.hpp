/*! \file
    \brief Declaration of the neoip_socket_udp_daddr
*/


#ifndef __NEOIP_SOCKET_PEERID_UDP_HPP__
#define __NEOIP_SOCKET_PEERID_UDP_HPP__
/* system include */
/* local include */
#include "neoip_socket_peerid_vapi.hpp"
#include "neoip_ip_addr.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief socket_domain_t::UDP implementation of the socket_peerid_vapi_t
 */
class socket_peerid_udp_t : public socket_peerid_vapi_t {
private:
	ip_addr_t	ipaddr;		//!< represent the actual peerid for this domain
public:
	/*************** ctor/dtor	***************************************/
	socket_err_t	ctor_from_str(const std::string &addr_adr)		throw();
	
	/*************** query function`***************************************/
	socket_domain_t	get_domain()						const throw();
	bool		is_localhost()						const throw();
	bool		is_linklocal()						const throw();
	bool		is_private()						const throw();
	bool		is_public()						const throw();
	bool		is_any()						const throw();
	
	/*************** comparison operator	*******************************/
	int		compare(const socket_peerid_vapi_t &other)		const throw();

	/*************** display function	*******************************/	
	std::string	to_string()						const throw();
	
	/*************** serialization	***************************************/
	void		serialize(serial_t &serial)				const throw();
	void		unserialize(serial_t &serial)				throw(serial_except_t);

	// definition of the factory creation
	FACTORY_PRODUCT_DECLARATION(socket_peerid_vapi_t, socket_peerid_udp_t);

	/************** List of friend class	*******************************/
	friend class	socket_helper_udp_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_PEERID_UDP_HPP__ */



