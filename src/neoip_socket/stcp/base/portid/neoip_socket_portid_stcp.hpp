/*! \file
    \brief Declaration of the neoip_socket_stcp_daddr
*/


#ifndef __NEOIP_SOCKET_PORTID_STCP_HPP__
#define __NEOIP_SOCKET_PORTID_STCP_HPP__
/* system include */
/* local include */
#include "neoip_socket_portid_vapi.hpp"
#include "neoip_ip_addr.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief socket_domain_t::STCP implementation of the socket_portid_vapi_t
 */
class socket_portid_stcp_t : public socket_portid_vapi_t {
private:
	uint16_t	port;		//!< represent the actual portid for this domain
public:
	/*************** ctor/dtor	***************************************/
	socket_err_t	ctor_from_str(const std::string &addr_adr)		throw();
	
	/*************** query function`***************************************/
	socket_domain_t	get_domain()						const throw();

	/*************** comparison operator	*******************************/
	int		compare(const socket_portid_vapi_t &other)		const throw();

	/*************** display function	*******************************/	
	std::string	to_string()						const throw();
	
	/*************** serialization	***************************************/
	void		serialize(serial_t &serial)				const throw();
	void		unserialize(serial_t &serial)				throw(serial_except_t);

	// definition of the factory creation
	FACTORY_PRODUCT_DECLARATION(socket_portid_vapi_t, socket_portid_stcp_t);

	/************** List of friend class	*******************************/
	friend class	socket_helper_stcp_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_PORTID_STCP_HPP__ */



