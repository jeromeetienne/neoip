/*! \file
    \brief Header of the \ref router_rident_t

*/


#ifndef __NEOIP_ROUTER_RIDENT_HPP__ 
#define __NEOIP_ROUTER_RIDENT_HPP__ 
/* system include */
/* local include */
#include "neoip_router_rident_wikidbg.hpp"
#include "neoip_router_peerid.hpp"
#include "neoip_x509_cert.hpp"
#include "neoip_router_name.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	router_profile_t;

/** \brief to handle the local identity in router_peer_t
 */
class router_rident_t : NEOIP_COPY_CTOR_ALLOW
			, private wikidbg_obj_t<router_rident_t, router_rident_wikidbg_init> {
private:
	router_peerid_t		m_peerid;
	x509_cert_t		m_cert;	
	router_name_t		m_hostonly_lview;
public:
	/*************** ctor/dtor	***************************************/
	router_rident_t()	throw()	{}
	router_rident_t(const router_peerid_t &m_peerid, const x509_cert_t &m_cert
					, const router_name_t &m_hostonly_lview)	throw();
	explicit router_rident_t(const std::string &str) throw() { *this = from_canonical_string(str);	}

	/*************** Query function	***************************************/
	bool			is_null()	const throw()	{ return peerid().is_null();	}
	bool			is_sane()	const throw();
	const router_peerid_t &	peerid()	const throw()	{ return m_peerid;		}
	const x509_cert_t &	cert()		const throw()	{ return m_cert;		}
	const router_name_t &	hostonly_lview()const throw()	{ return m_hostonly_lview;	}
	router_name_t		dnsfqname(const router_profile_t &profile)	const throw();
	
	/*************** Comparison Operator	*******************************/
	int	compare(const router_rident_t &other)	   const throw();
	bool	operator == (const router_rident_t &other) const throw() { return compare(other) == 0; }
	bool	operator != (const router_rident_t &other) const throw() { return compare(other) != 0; }
	bool	operator <  (const router_rident_t &other) const throw() { return compare(other) <  0; }
	bool	operator <= (const router_rident_t &other) const throw() { return compare(other) <= 0; }
	bool	operator >  (const router_rident_t &other) const throw() { return compare(other) >  0; }
	bool	operator >= (const router_rident_t &other) const throw() { return compare(other) >= 0; }

	/*************** String convertion	*******************************/
	static router_rident_t	from_canonical_string(const std::string &str)	throw();
	std::string		to_canonical_string()				const throw();
	
	/*************** Display function	*******************************/
	std::string		to_string()		const throw()	{ return to_canonical_string();	}
	friend	std::ostream & operator << (std::ostream & os, const router_rident_t &rident)throw()
							{ return os << rident.to_string();	}

	/*************** List of friend class	*******************************/
	friend class	router_rident_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_RIDENT_HPP__  */



