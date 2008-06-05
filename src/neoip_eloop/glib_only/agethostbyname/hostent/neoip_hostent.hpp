/*! \file
    \brief Header of the \ref kad_http_t class

*/


#ifndef __NEOIP_HOSTENT_HPP__ 
#define __NEOIP_HOSTENT_HPP__ 
/* system include */
#include <vector>
#include <string>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
/* local include */
#include "neoip_ip_addr.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief c++ version of the struct hostent from the gethostbyname()
 */
class hostent_t {
private:
	std::string			hostname;
	std::vector<std::string>	alias_db;
	std::vector<ip_addr_t>		ip_addr_db;
public:	
	/*************** ctor/dtor	***************************************/
	hostent_t(struct hostent *hostent)	throw();
	hostent_t()	throw()	{}
	~hostent_t()	throw();

	bool	is_null()	const throw()	{ return hostname.empty();	}

	/*************** query function	***************************************/	
	const std::string &		get_hostname()	const throw()	{ return hostname;	}
	const std::vector<std::string> &get_alias_db()	const throw()	{ return alias_db;	}
	const std::vector<ip_addr_t> &	get_ip_addr_db()const throw()	{ return ip_addr_db;	}

	/*************** display function	*******************************/	
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const hostent_t &hostent ) throw()
					{ return os << hostent.to_string();	}	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HOSTENT_HPP__  */


 
