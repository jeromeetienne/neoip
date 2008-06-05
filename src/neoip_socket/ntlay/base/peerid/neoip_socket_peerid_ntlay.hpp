/*! \file
    \brief Declaration of the neoip_socket_ntudp_daddr
*/


#ifndef __NEOIP_SOCKET_PEERID_NTLAY_HPP__
#define __NEOIP_SOCKET_PEERID_NTLAY_HPP__
/* system include */
/* local include */
#include "neoip_socket_peerid_vapi.hpp"
#include "neoip_ntudp_peerid.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief socket_domain_t::NTLAY implementation of the socket_peerid_vapi_t
 */
class socket_peerid_ntlay_t : public socket_peerid_vapi_t {
private:
	ntudp_peerid_t	ntudp_peerid;		//!< represent the actual peerid for this domain
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
	FACTORY_PRODUCT_DECLARATION(socket_peerid_vapi_t, socket_peerid_ntlay_t);

	/************** List of friend class	*******************************/
	friend class	socket_resp_ntlay_t;
	friend class	socket_itor_ntlay_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_PEERID_NTLAY_HPP__ */



