/*! \file
    \brief Header of the \ref socket_peerid_vapi_t
*/


#ifndef __NEOIP_SOCKET_PEERID_VAPI_HPP__ 
#define __NEOIP_SOCKET_PEERID_VAPI_HPP__ 

/* system include */
#include <string>
/* local include */
#include "neoip_socket_err.hpp"
#include "neoip_socket_domain.hpp"
#include "neoip_serial.hpp"
#include "neoip_obj_factory.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief Definition of the virtual API for the socket_peerid_t
 */
class socket_peerid_vapi_t {
public:
	/*************** ctor/dtor	***************************************/
	virtual socket_err_t	ctor_from_str(const std::string &str)		throw() = 0;
	
	/*************** query function`***************************************/
	virtual	socket_domain_t	get_domain()					const throw() = 0;
	virtual bool		is_localhost()					const throw() = 0;
	virtual bool		is_linklocal()					const throw() = 0;
	virtual bool		is_private()					const throw() = 0;
	virtual bool		is_public()					const throw() = 0;
	virtual bool		is_any()					const throw() = 0;

	/*************** comparison operator	*******************************/
	virtual int		compare(const socket_peerid_vapi_t &other)	const throw() = 0;

	/*************** display function	*******************************/
	virtual std::string	to_string()					const throw() = 0;

	/*************** serialization	***************************************/
	virtual	void		serialize(serial_t &serial)			const throw() = 0;
	virtual	void		unserialize(serial_t &serial)			throw(serial_except_t)= 0;

	// declaration for the factory
	FACTORY_BASE_CLASS_DECLARATION(socket_peerid_vapi_t);
	//! virtual destructor
	virtual ~socket_peerid_vapi_t() {};
};

// declaration of the factory type
FACTORY_PLANT_DECLARATION(socket_peerid_factory, socket_domain_t, socket_peerid_vapi_t);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_PEERID_VAPI_HPP__  */



