/*! \file
    \brief Header of the \ref kad_addr_t
    
*/


#ifndef __NEOIP_KAD_ADDR_HPP__ 
#define __NEOIP_KAD_ADDR_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_id.hpp"
#include "neoip_ipport_addr.hpp"
#include "neoip_serial.hpp"
#include "neoip_xmlrpc_decl.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for remote node
 */
class kad_addr_t : NEOIP_COPY_CTOR_ALLOW {
public:	/** \brief The type for the kad_addr_t filter callback
	 * 
	 * @return true if the kad_addr_t MUST be filtered out, false otherwise
	 */
	typedef bool (*filter_fct_t)(const kad_addr_t &kad_addr);
	static bool	filter_fct_dstpublic(const kad_addr_t &kad_addr)	throw();
	static bool	filter_fct_dstprivate(const kad_addr_t &kad_addr)	throw();
	static bool	filter_fct_dstlinklocal(const kad_addr_t &kad_addr)	throw();
	static bool	filter_fct_dstlocalhost(const kad_addr_t &kad_addr)	throw();
private:
	ipport_addr_t	m_oaddr;		//!< the ip:port of the kad peer
	kad_peerid_t	m_peerid;		//!< the peerid of the kad peer
public:
	/*************** ctor/dtor	***************************************/
	kad_addr_t()								throw() {}
	kad_addr_t(const ipport_addr_t &m_oaddr, const kad_peerid_t &m_peerid)	throw();
	
	/*************** comparison operator	*******************************/
	int	compare(const kad_addr_t &other)	const throw();
	bool 	operator == (const kad_addr_t & other)	const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const kad_addr_t & other)	const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const kad_addr_t & other)	const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const kad_addr_t & other)	const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const kad_addr_t & other)	const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const kad_addr_t & other)	const throw()	{ return compare(other) >= 0;	}
	
	/*************** query function		*******************************/	
	bool			is_null()	const throw() { return oaddr().is_null() && peerid().is_null();}
	const ipport_addr_t &	oaddr()		const throw() { return m_oaddr;		}
	ipport_addr_t &		oaddr()		      throw() { return m_oaddr;		}
	const kad_peerid_t &	peerid()	const throw() { return m_peerid;	}
	bool			is_fully_qualified() 	const throw()
				{ return oaddr().is_fully_qualified() && peerid().is_fully_qualified();	}

	/*************** Compatibility layer	*******************************/
	const ipport_addr_t &	get_oaddr()	const throw() { return oaddr();		}
	ipport_addr_t &		get_oaddr()	      throw() { return oaddr();		}
	const kad_peerid_t &	get_peerid()	const throw() { return peerid();	}
	
	/*************** display function	*******************************/	
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const kad_addr_t &kad_addr)	throw()
				{ return os << kad_addr.to_string();	}

	/*************** serialization function	*******************************/
	friend	serial_t &operator << (serial_t& serial, const kad_addr_t &kad_addr)	throw();
	friend	serial_t &operator >> (serial_t& serial, kad_addr_t &kad_addr)	 	throw(serial_except_t);	

	/*************** xmlrpc function	*******************************/
	friend	xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const kad_addr_t &kad_addr)	throw();
	friend	xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, kad_addr_t &kad_addr)	throw(xml_except_t);
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_ADDR_HPP__  */










