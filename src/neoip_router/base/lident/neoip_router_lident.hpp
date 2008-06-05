/*! \file
    \brief Header of the \ref router_lident_t

*/


#ifndef __NEOIP_ROUTER_LIDENT_HPP__ 
#define __NEOIP_ROUTER_LIDENT_HPP__ 
/* system include */
/* local include */
#include "neoip_router_lident_wikidbg.hpp"
#include "neoip_router_peerid.hpp"
#include "neoip_x509_privkey.hpp"
#include "neoip_x509_cert.hpp"
#include "neoip_router_name.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	router_rident_t;
class	router_profile_t;

/** \brief to handle the local identity in router_peer_t
 */
class router_lident_t : NEOIP_COPY_CTOR_ALLOW
			, private wikidbg_obj_t<router_lident_t, router_lident_wikidbg_init> {
private:
	router_peerid_t		m_peerid;
	x509_privkey_t		m_privkey;
	x509_cert_t		m_cert;
	router_name_t		m_dnsname;	
public:
	/*************** ctor/dtor	***************************************/
	router_lident_t()	throw()	{}
	router_lident_t(const router_peerid_t &m_peerid, const x509_privkey_t &m_privkey
				, const x509_cert_t &m_cert, const router_name_t &m_dnsname)	throw();
	explicit router_lident_t(const std::string &str) throw() { *this = from_canonical_string(str);	}

	/*************** Build function	***************************************/
	static router_lident_t	generate(const router_peerid_t &peerid ,const router_name_t &m_dnsname
					, int key_len, const x509_cert_t &ca_cert
					, const x509_privkey_t &ca_privkey)	throw();
	
	/*************** Query function	***************************************/
	bool			is_null()	const throw()	{ return m_peerid.is_null();		}
	bool			is_sane()	const throw();
	const router_peerid_t &	peerid()	const throw()	{ return m_peerid;			}
	const x509_privkey_t &	privkey()	const throw()	{ return m_privkey;			}
	const x509_cert_t &	cert()		const throw()	{ return m_cert;			}
	const router_name_t &	dnsname()	const throw()	{ return m_dnsname;			}
	bool			is_selfsigned()	const throw()	{ return dnsname().is_selfsigned_ok();	}
	bool			is_authsigned()	const throw()	{ return dnsname().is_authsigned_ok();	}
	bool			is_nonesigned()	const throw()	{ return dnsname().is_nonesigned_ok();	}	
	router_name_t		dnsfqname(const router_profile_t &profile)	const throw();

	/*************** convertion function	*******************************/
	router_rident_t		to_rident()	const throw();

	/*************** Comparison Operator	*******************************/
	int	compare(const router_lident_t &other)	   const throw();
	bool	operator == (const router_lident_t &other) const throw() { return compare(other) == 0; }
	bool	operator != (const router_lident_t &other) const throw() { return compare(other) != 0; }
	bool	operator <  (const router_lident_t &other) const throw() { return compare(other) <  0; }
	bool	operator <= (const router_lident_t &other) const throw() { return compare(other) <= 0; }
	bool	operator >  (const router_lident_t &other) const throw() { return compare(other) >  0; }
	bool	operator >= (const router_lident_t &other) const throw() { return compare(other) >= 0; }

	/*************** String convertion	*******************************/
	static router_lident_t	from_canonical_string(const std::string &str)	throw();
	std::string		to_canonical_string()				const throw();
	
	/*************** Display function	*******************************/
	std::string		to_string()		const throw()	{ return to_canonical_string();	}
	friend	std::ostream & operator << (std::ostream & os, const router_lident_t &lident)throw()
							{ return os << lident.to_string();	}
	/*************** List of friend class	*******************************/
	friend class	router_lident_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ROUTER_LIDENT_HPP__  */



