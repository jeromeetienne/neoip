/*! \file
    \brief Header of the \ref kad_caddr_t
    
*/


#ifndef __NEOIP_KAD_CADDR_HPP__ 
#define __NEOIP_KAD_CADDR_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_id.hpp"
#include "neoip_kad_addr.hpp"
#include "neoip_cookie.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_serial.hpp"
#include "neoip_xmlrpc_decl.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_addr_t;

/** \brief class definition for remote node with cookie
 * 
 * - TODO this class should likely inherit from the kad_addr_t
 */
class kad_caddr_t {
private:
	ipport_addr_t	addr;
	kad_peerid_t	peerid;
	cookie_id_t	cookie_id;
public:
	/*************** ctor/dtor	***************************************/
	kad_caddr_t()				throw()	{}
	kad_caddr_t(const ipport_addr_t &addr, const kad_peerid_t &peerid, const cookie_id_t &cookie_id)throw();
	kad_caddr_t(const kad_addr_t &kad_addr, const cookie_id_t &cookie_id)			throw();	
	
	/*************** Query function	***************************************/
	bool		is_null()	const throw();

	/*************** Comparison operators	*******************************/
	int	compare(const kad_caddr_t &other)	const throw();
	bool 	operator == (const kad_caddr_t & other)	const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const kad_caddr_t & other)	const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const kad_caddr_t & other)	const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const kad_caddr_t & other)	const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const kad_caddr_t & other)	const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const kad_caddr_t & other)	const throw()	{ return compare(other) >= 0;	}
	
	
	/*************** Query Functions	*******************************/
	const ipport_addr_t &	get_oaddr()	const throw() { return addr;				}
	const kad_peerid_t &	get_peerid()	const throw() { return peerid;				}
	const cookie_id_t &	get_cookie_id()	const throw() { return cookie_id;			}
	kad_addr_t 		get_kad_addr()	const throw() { return kad_addr_t(addr, peerid);	}
	
	/*************** display function	*******************************/
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const kad_caddr_t &kad_caddr)	throw()
						{ return os << kad_caddr.to_string();	}

	/*************** xmlrpc function	*******************************/
	friend	xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const kad_caddr_t &kad_caddr)throw();
	friend	xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, kad_caddr_t &kad_caddr)throw(xml_except_t);
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_CADDR_HPP__  */










