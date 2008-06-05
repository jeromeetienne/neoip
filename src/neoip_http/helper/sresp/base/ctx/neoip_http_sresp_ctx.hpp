/*! \file
    \brief Header of the \ref http_sresp_ctx_t
    
*/


#ifndef __NEOIP_HTTP_SRESP_CTX_HPP__ 
#define __NEOIP_HTTP_SRESP_CTX_HPP__ 
/* system include */
#include <sstream>
/* local include */
#include "neoip_http_reqhd.hpp"
#include "neoip_http_rephd.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for http_sresp_ctx_t
 */
class http_sresp_ctx_t : NEOIP_COPY_CTOR_ALLOW {
private:
	/*************** internal function	*******************************/
	void copy(const http_sresp_ctx_t &other)	throw();
public:
	/*************** ctor/dtor	***************************************/
	http_sresp_ctx_t()			throw();

	/*************** copy operator	***************************************/
	http_sresp_ctx_t(const http_sresp_ctx_t &other)			throw();
	http_sresp_ctx_t &operator = (const http_sresp_ctx_t & other)	throw();
	
	/*************** query function		*******************************/	
	bool		is_null()	const throw() { return reqhd().is_null();	}
	
	/*************** Helper function	*******************************/
	void		reply_error(size_t status_code, const std::string &reason_phrase)	throw();
	
	/*************** #define to ease the declaration	***************/
#	define NEOIP_DEFINE_VAR(var_type, var_name)						\
	private:var_type	var_name##_val;							\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }	\
		var_type &	var_name()	throw()		{ return var_name ## _val; }	\
		http_sresp_ctx_t &	var_name(const var_type &value)	throw()				\
						{ var_name ## _val = value; return *this;	}

	NEOIP_DEFINE_VAR(http_reqhd_t	, reqhd);
	NEOIP_DEFINE_VAR(bytearray_t	, post_data);
	NEOIP_DEFINE_VAR(http_rephd_t	, rephd);

	// special case for response_body, because std::ostringstream is not copiable
	private: std::ostringstream	response_body_val;
	public:	const std::ostringstream &	response_body()	const throw()	{ return response_body_val; }
		std::ostringstream &		response_body()	throw()		{ return response_body_val; }
	
	/*************** #undef to ease the declaration	***********************/
#	undef NEOIP_DEFINE_VAR

	/*************** comparison operator	*******************************/
	int	compare(const http_sresp_ctx_t & other)	const throw();
	bool	operator ==(const http_sresp_ctx_t & other)  const throw(){ return compare(other) == 0; }
	bool	operator !=(const http_sresp_ctx_t & other)  const throw(){ return compare(other) != 0; }
	bool	operator < (const http_sresp_ctx_t & other)  const throw(){ return compare(other) <  0; }
	bool	operator <=(const http_sresp_ctx_t & other)  const throw(){ return compare(other) <= 0; }
	bool	operator > (const http_sresp_ctx_t & other)  const throw(){ return compare(other) >  0; }
	bool	operator >=(const http_sresp_ctx_t & other)  const throw(){ return compare(other) >= 0; }

	/*************** display function	*******************************/	
	std::string		to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream & os, const http_sresp_ctx_t &http_sresp_ctx)	throw()
				{ return os << http_sresp_ctx.to_string();	}
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_HTTP_SRESP_CTX_HPP__  */










