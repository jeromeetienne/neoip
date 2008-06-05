/*! \file
    \brief Header of the \ref router_rootca_t

*/


#ifndef __NEOIP_ROUTER_ROOTCA_HPP__ 
#define __NEOIP_ROUTER_ROOTCA_HPP__ 
/* system include */
/* local include */
#include "neoip_router_rootca_wikidbg.hpp"
#include "neoip_x509_cert.hpp"
#include "neoip_item_arr.hpp"
#include "neoip_router_name.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to handle the local identity in router_peer_t
 */
class router_rootca_t : NEOIP_COPY_CTOR_ALLOW
			, private wikidbg_obj_t<router_rootca_t, router_rootca_wikidbg_init> {
private:
	x509_cert_t		m_cert;	
	item_arr_t<router_name_t>	m_domain_db;
public:
	/*************** ctor/dtor	***************************************/
	router_rootca_t()	throw()	{}
	router_rootca_t(const x509_cert_t &m_cert, const item_arr_t<router_name_t> &m_domain_db)	throw();
	explicit router_rootca_t(const std::string &str) throw() { *this = from_canonical_string(str);	}

	/*************** Query function	***************************************/
	bool				is_null()	const throw()	{ return cert().is_null();	}
	const x509_cert_t &		cert()		const throw()	{ return m_cert;		}
	const item_arr_t<router_name_t> &	domain_db()	const throw()	{ return m_domain_db;		}
	bool				match_dnsname(const router_name_t &dnsname)	const throw();

	/*************** Comparison Operator	*******************************/
	int	compare(const router_rootca_t &other)	   const throw();
	bool	operator == (const router_rootca_t &other) const throw() { return compare(other) == 0; }
	bool	operator != (const router_rootca_t &other) const throw() { return compare(other) != 0; }
	bool	operator <  (const router_rootca_t &other) const throw() { return compare(other) <  0; }
	bool	operator <= (const router_rootca_t &other) const throw() { return compare(other) <= 0; }
	bool	operator >  (const router_rootca_t &other) const throw() { return compare(other) >  0; }
	bool	operator >= (const router_rootca_t &other) const throw() { return compare(other) >= 0; }

	/*************** String convertion	*******************************/
	static router_rootca_t	from_canonical_string(const std::string &str)	throw();
	std::string		to_canonical_string()				const throw();
	
	/*************** Display function	*******************************/
	std::string		to_string()		const throw()	{ return to_canonical_string();	}
	friend	std::ostream & operator << (std::ostream & os, const router_rootca_t &rootca)throw()
							{ return os << rootca.to_string();	}

	/*************** List of friend class	*******************************/
	friend class	router_rootca_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_ROOTCA_HPP__  */



