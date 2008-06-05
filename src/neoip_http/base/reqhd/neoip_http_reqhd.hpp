/*! \file
    \brief Header of the \ref http_reqhd_t
    
*/


#ifndef __NEOIP_HTTP_REQHD_HPP__ 
#define __NEOIP_HTTP_REQHD_HPP__ 
/* system include */
/* local include */
#include "neoip_http_method.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_http_version.hpp"
#include "neoip_strvar_db.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	file_range_t;
class	file_size_t;

/** \brief class definition for http_reqhd_t
 */
class http_reqhd_t : NEOIP_COPY_CTOR_ALLOW {
private:
	/*************** Internal function	*******************************/	
	static std::string	consume_line(std::string &base_str)		throw();	
public:
	/*************** ctor/dtor	***************************************/
	http_reqhd_t()					throw();

	/*************** convertion function	*******************************/
	std::string		to_http()					const throw();
	static http_reqhd_t	from_http(const std::string &reqhd_str)		throw();

	/*************** query function		*******************************/	
	bool			is_null()		const throw() { return uri().is_null();	}
	bool			has_header(const std::string &header_key)	const throw();
	const std::string &	get_header_value(const std::string &header_key)	const throw();

	/*************** header helper	***************************************/
	file_range_t		range()						const throw();
	http_reqhd_t &		range(const file_range_t &file_range)		throw();
	file_size_t		content_length()				const throw();
	http_reqhd_t &		content_length(const file_size_t &file_size)	throw();

	/*************** #define to ease the declaration	***************/
#	define NEOIP_DEFINE_VAR(var_type, var_name)						\
	private:var_type	var_name##_val;							\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }	\
		var_type &	var_name()	throw()		{ return var_name ## _val; }	\
		http_reqhd_t &	var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}

	NEOIP_DEFINE_VAR(http_method_t	, method);
	NEOIP_DEFINE_VAR(http_uri_t	, uri);
	NEOIP_DEFINE_VAR(http_version_t	, version);
	NEOIP_DEFINE_VAR(strvar_db_t	, header_db);
	
	/*************** #undef to ease the declaration	***********************/
#	undef NEOIP_DEFINE_VAR

	/*************** comparison operator	*******************************/
	int	compare(const http_reqhd_t & other)	const throw();
	bool	operator ==(const http_reqhd_t & other)  const throw(){ return compare(other) == 0; }
	bool	operator !=(const http_reqhd_t & other)  const throw(){ return compare(other) != 0; }
	bool	operator < (const http_reqhd_t & other)  const throw(){ return compare(other) <  0; }
	bool	operator <=(const http_reqhd_t & other)  const throw(){ return compare(other) <= 0; }
	bool	operator > (const http_reqhd_t & other)  const throw(){ return compare(other) >  0; }
	bool	operator >=(const http_reqhd_t & other)  const throw(){ return compare(other) >= 0; }

	/*************** display function	*******************************/	
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const http_reqhd_t &http_reqhd)	throw()
				{ return os << http_reqhd.to_string();	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_REQHD_HPP__  */










