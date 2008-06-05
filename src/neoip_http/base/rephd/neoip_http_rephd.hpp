/*! \file
    \brief Header of the \ref http_rephd_t
    
*/


#ifndef __NEOIP_HTTP_REPHD_HPP__ 
#define __NEOIP_HTTP_REPHD_HPP__ 
/* system include */
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

// list of include for the internal fields
#include "neoip_http_method.hpp"
#include "neoip_http_uri.hpp"
#include "neoip_http_version.hpp"
#include "neoip_http_status.hpp"
#include "neoip_file_size.hpp"
#include "neoip_strvar_db.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	file_range_t;

/** \brief class definition for http_rephd_t - see rfc2616.6
 */
class http_rephd_t : NEOIP_COPY_CTOR_ALLOW {
private:
	/*************** Internal function	*******************************/	
	static std::string	consume_line(std::string &base_str)		throw();	
public:
	/*************** ctor/dtor	***************************************/
	http_rephd_t()					throw();

	/*************** convertion function	*******************************/
	std::string		to_http()					const throw();
	static http_rephd_t	from_http(const std::string &rephd_str)		throw();

	/*************** query function		*******************************/	
	bool			is_null()	const throw() { return status_code() == 0;	}
	bool			has_header(const std::string &header_key)	const throw();
	const std::string &	get_header_value(const std::string &header_key)	const throw();

	/*************** header helper	***************************************/
	file_range_t		content_range(file_size_t *instance_len_out)	const throw();
	http_rephd_t &		content_range(const file_range_t &file_range
					, const file_size_t &instance_len)	throw();
	file_size_t		content_length()				const throw();
	http_rephd_t &		content_length(const file_size_t &file_size)	throw();
	bool			accept_ranges()					const throw();
	http_rephd_t &		accept_ranges(bool doaccept)			throw();
	std::string 		transfer_encoding()				const throw();
	http_rephd_t &		transfer_encoding(const std::string &value)	throw();
	bool			is_chunked_encoded()				const throw();
	bool			is_connection_close()				const throw();
	
	/*************** #define to ease the declaration	***************/
#	define NEOIP_DEFINE_VAR(var_type, var_name)						\
	private:var_type	var_name##_val;							\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }	\
		var_type &	var_name()	throw()		{ return var_name ## _val; }	\
		http_rephd_t &	var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}

	NEOIP_DEFINE_VAR(http_version_t	, version);
	NEOIP_DEFINE_VAR(size_t		, status_code);
	NEOIP_DEFINE_VAR(std::string	, reason_phrase);
	NEOIP_DEFINE_VAR(strvar_db_t	, header_db);
	
	/*************** #undef to ease the declaration	***********************/
#	undef NEOIP_DEFINE_VAR

	/*************** comparison operator	*******************************/
	int	compare(const http_rephd_t & other)	const throw();
	bool	operator ==(const http_rephd_t & other)  const throw(){ return compare(other) == 0; }
	bool	operator !=(const http_rephd_t & other)  const throw(){ return compare(other) != 0; }
	bool	operator < (const http_rephd_t & other)  const throw(){ return compare(other) <  0; }
	bool	operator <=(const http_rephd_t & other)  const throw(){ return compare(other) <= 0; }
	bool	operator > (const http_rephd_t & other)  const throw(){ return compare(other) >  0; }
	bool	operator >=(const http_rephd_t & other)  const throw(){ return compare(other) >= 0; }

	/*************** display function	*******************************/	
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const http_rephd_t &http_rephd)	throw()
				{ return os << http_rephd.to_string();	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_REPHD_HPP__  */










