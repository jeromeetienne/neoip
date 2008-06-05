/*! \file
    \brief Header of the \ref dnsgrab_t class
    
*/


#ifndef __NEOIP_DNSGRAB_REQUEST_HPP__ 
#define __NEOIP_DNSGRAB_REQUEST_HPP__ 
/* system include */
#include <list>
#include <string>
/* local include */
#include "neoip_ip_addr.hpp"
#include "neoip_slotpool.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

class dnsgrab_request_t : NEOIP_COPY_CTOR_ALLOW {
private:
	slot_id_t	cnx_slot_id;		//!< the connection slot_id - used to notify the 
						//!< answer later
	std::string	request_name;		//!< the requested data (may be a hostname or an addr)
	bool		request_by_name;	//!< true if the request is a gethostbyname, false
						//!< if it is a gethostbyaddr
	std::string	addr_family;		//!< the string defining the address family AF_INET|AF_INET6

	bool			reply_present;
	std::string		reply_name;
	std::list<std::string>	reply_aliases;
	std::list<ip_addr_t>	reply_addresses;
public:	
	/*************** ctor/dtor	***************************************/ 
	dnsgrab_request_t()	throw()	{}
	dnsgrab_request_t(const slot_id_t &cnx_slot_id, const std::string & request_str)	throw();

	bool			is_null()	const throw();

	std::string		get_reply_str()		const throw();

	/*************** query function	***************************************/ 
	const std::string &	get_request_name()	const throw()	{ return request_name;		}
	const std::string &	get_addr_family()	const throw()	{ return addr_family;		}
	bool			is_request_by_name()	const throw()	{ return request_by_name;	}
	bool			is_request_by_addr()	const throw()	{ return !request_by_name;	}
	

	slot_id_t			get_cnx_slot_id()	const throw()	{ return cnx_slot_id;	}
	const bool &			get_reply_present()	const throw()	{ return reply_present;	}
	bool &				get_reply_present()	throw()		{ return reply_present;	}
	const std::string &		get_reply_name()	const throw()	{ return reply_name;	}
	std::string &			get_reply_name()	throw()		{ return reply_name;	}
	const std::list<std::string> &	get_reply_aliases()	const throw()	{ return reply_aliases;	}
	std::list<std::string> &	get_reply_aliases()	throw()		{ return reply_aliases;	}
	const std::list<ip_addr_t> &	get_reply_addresses()	const throw()	{ return reply_addresses;}
	std::list<ip_addr_t> &		get_reply_addresses()	throw()		{ return reply_addresses;}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_DNSGRAB_REQUEST_HPP__  */



