/*! \file
    \brief Header of the \ref http_uri2_t

*/


#ifndef __NEOIP_HTTP_URI2_HPP__
#define __NEOIP_HTTP_URI2_HPP__
/* system include */
#include <string>
/* local include */
#include "neoip_http_scheme.hpp"
#include "neoip_file_path.hpp"
#include "neoip_strvar_db.hpp"
#include "neoip_serial.hpp"
#include "neoip_xmlrpc_decl.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for http uri as in rfc2616
 */
class http_uri2_t : NEOIP_COPY_CTOR_ALLOW {
public:	/////////////////////// constant declaration ///////////////////////////
	//! unreserved charset for url from rfc2396.2.3
	static const std::string	UNRESERVED_CHARSET;
private:
	// typedef to store the header reply variables
	http_scheme_t	uri_scheme;
	std::string	uri_host;
	size_t		uri_port;
	file_path_t	uri_path;
	strvar_db_t	uri_var;
	std::string	uri_anchor;

	/*************** Internal function	*******************************/
	void		ctor_from_str(const std::string &str)	throw();
public:
	/*************** ctor/dtor	***************************************/
	http_uri2_t()				throw()	{}
	http_uri2_t(const std::string &str)	throw()	{ ctor_from_str(str);			}
	http_uri2_t(const char *str)		throw()	{ ctor_from_str(std::string(str));	}

	/*************** Query function	***************************************/
	bool			is_null()	const throw()	{ return uri_host.empty();	}
	const http_scheme_t &	scheme()	const throw()	{ return uri_scheme;		}
	const std::string &	host()		const throw()	{ return uri_host;		}
	size_t			port()		const throw()	{ return uri_port;		}
	const file_path_t &	path()		const throw()	{ return uri_path;		}
	const std::string &	anchor()	const throw()	{ return uri_anchor;		}
	const strvar_db_t &	var()		const throw()	{ return uri_var;		}
	strvar_db_t &		var()		throw()		{ return uri_var;		}

	/*************** action function	*******************************/
	http_uri2_t &		clear_hostport()	throw();
	http_uri2_t &		clear_pathquery()	throw();

	/*************** uri scramble stuff	*******************************/
	bool			is_scrambled()		const throw();
	http_uri2_t		unscramble()		const throw();

	/*************** comparison operator	*******************************/
	int	compare(const http_uri2_t & other)	const throw();
	bool	operator ==(const http_uri2_t & other)  const throw(){ return compare(other) == 0; }
	bool	operator !=(const http_uri2_t & other)  const throw(){ return compare(other) != 0; }
	bool	operator < (const http_uri2_t & other)  const throw(){ return compare(other) <  0; }
	bool	operator <=(const http_uri2_t & other)  const throw(){ return compare(other) <= 0; }
	bool	operator > (const http_uri2_t & other)  const throw(){ return compare(other) >  0; }
	bool	operator >=(const http_uri2_t & other)  const throw(){ return compare(other) >= 0; }

	/*************** display function	*******************************/
	std::string	hostport_str()	const throw();
	std::string	path_str()	const throw();
	std::string	query_str()	const throw();
	std::string	pathquery_str()	const throw();
	std::string	to_string()	const throw();
	friend std::ostream & operator << (std::ostream & os, const http_uri2_t & http_uri) throw()
					{ return os << http_uri.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const http_uri2_t &http_uri)	throw();
	friend	serial_t& operator >> (serial_t & serial, http_uri2_t &http_uri) 	throw(serial_except_t);

	/*************** xmlrpc function	*******************************/
	friend	xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const http_uri2_t &http_uri)	throw();
	friend	xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, http_uri2_t &http_uri)		throw(xml_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_URI2_HPP__  */










