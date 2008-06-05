/*! \file
    \brief Header of the \ref router_name_t.cpp

*/


#ifndef __NEOIP_ROUTER_NAME_HPP__ 
#define __NEOIP_ROUTER_NAME_HPP__ 

/* system include */
#include <string>
/* local include */
#include "neoip_item_arr.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class to handle router_name_t
 * 
 * - it handles the dns like names
 */
class router_name_t : NEOIP_COPY_CTOR_ALLOW {
private://///////////////////// constant declaration ///////////////////////////
	//! a std::string containing label separator
	static	const char *	SEPARATOR;
	//! a std::string containing GLOB_STR
	static	const char *	GLOB_STR;
private:
	std::string	m_host;		//!< the string for the host
	std::string	m_domain;	//!< the string for the domain

	/*************** Internal function	*******************************/
	void	ctor_internal(const std::string &host_str, const std::string &domain_str)throw();
public:
	/*************** ctor/dtor	***************************************/
	router_name_t()		throw()	{}
	router_name_t(const std::string &host_str, const std::string &domain_str)	throw();
	explicit router_name_t(const std::string &str)					throw();
	
	/*************** query function	***************************************/
	bool		is_null()		const throw()	{ return m_host.empty() || m_domain.empty();	}
	bool		is_domain_only()	const throw()	{ return host() == GLOB_STR;			}
	bool		is_host_only()		const throw()	{ return domain() == GLOB_STR;			}
	bool		is_fully_qualified()	const throw()	{ return !is_null() && !is_host_only() && !is_domain_only();	}
	const std::string & host()		const throw()	{ return m_host;				}
	const std::string & domain()		const throw()	{ return m_domain;				}
	bool		is_selfsigned_ok()	const throw();
	bool		is_authsigned_ok()	const throw();
	bool		is_nonesigned_ok()	const throw();
	bool		match(const router_name_t &other)	const throw();
	
	/*************** Update function	*******************************/
	router_name_t &	add_domain(const router_name_t &other)	throw();
	router_name_t &	operator /=(const router_name_t &other)	throw()		{ return this->add_domain(other);	}
	router_name_t	operator / (const router_name_t &other)	const throw()	{ return router_name_t(*this) /= other;	}
	
	/*************** Comparison operator	*******************************/
	int	compare(const router_name_t &other)		const throw();
	bool 	operator == (const router_name_t & other)	const throw() { return compare(other) == 0;	}
	bool 	operator != (const router_name_t & other)	const throw() { return compare(other) != 0;	}
	bool 	operator <  (const router_name_t & other)	const throw() { return compare(other) <  0;	}
	bool 	operator <= (const router_name_t & other)	const throw() { return compare(other) <= 0;	}
	bool 	operator >  (const router_name_t & other)	const throw() { return compare(other) >  0;	}
	bool 	operator >= (const router_name_t & other)	const throw() { return compare(other) >= 0;	}

	/*************** display function	*******************************/
	std::string	to_string() const throw();
	friend	std::ostream & operator << (std::ostream & os, const router_name_t &dnsname)
					{ return os << dnsname.to_string();	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_NAME_HPP__  */



