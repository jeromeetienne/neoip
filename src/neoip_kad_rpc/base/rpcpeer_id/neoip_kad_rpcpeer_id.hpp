/*! \file
    \brief Header of the \ref kad_rpcpeer_id_t

*/


#ifndef __NEOIP_KAD_RPCPEER_ID_HPP__ 
#define __NEOIP_KAD_RPCPEER_ID_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_kad_rpclistener_id.hpp"
#include "neoip_kad_id.hpp"
#include "neoip_xmlrpc_decl.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_rpcpeer_t;

/** \brief to store the parameters replied by a tracker
 */
class kad_rpcpeer_id_t : NEOIP_COPY_CTOR_ALLOW {
private:
public:
	/*************** query function	***************************************/
	bool		is_null()						const throw();

	/*************** from/to bt_ezswarm_t	*******************************/
	static kad_rpcpeer_id_t	from_rpcpeer(const kad_rpcpeer_t *kad_rpcpeer)	throw();

	/*************** comparison operator	*******************************/
	int	compare(const kad_rpcpeer_id_t &other)	const throw();
	bool 	operator == (const kad_rpcpeer_id_t & other)const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const kad_rpcpeer_id_t & other)const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const kad_rpcpeer_id_t & other)const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const kad_rpcpeer_id_t & other)const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const kad_rpcpeer_id_t & other)const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const kad_rpcpeer_id_t & other)const throw()	{ return compare(other) >= 0;	}
	
	/*************** #define to ease the declaration	***************/
#	define RES_VAR_DIRECT(var_type, var_name)							\
	private:var_type	var_name##_val;								\
	public:	const var_type &var_name()	const throw()	{ return var_name ## _val; }		\
	public:	kad_rpcpeer_id_t &var_name(const var_type &var_name)	throw()				\
						{ var_name ## _val = var_name; return *this; }
#	define RES_VAR_STRUCT(var_type, var_name)							\
	public:	var_type &	var_name()	throw()		{ return var_name ## _val; }		\
	RES_VAR_DIRECT(var_type, var_name);

	/*************** declaration of res fields	***********************/
	RES_VAR_DIRECT( kad_rpclistener_id_t	, rpclistener_id);
	RES_VAR_DIRECT( kad_peerid_t		, kad_peerid);
	
	/*************** #undef to ease the declaration	***********************/
#	undef RES_VAR_DIRECT
#	undef RES_VAR_STRUCT

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const kad_rpcpeer_id_t &kad_rpcpeer_id) throw()
						{ return oss << kad_rpcpeer_id.to_string();	}

	/*************** xmlrpc function	*******************************/
	friend xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const kad_rpcpeer_id_t &kad_rpcpeer_id)	throw();
	friend xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, kad_rpcpeer_id_t &kad_rpcpeer_id)		throw(xml_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_RPCPEER_ID_HPP__  */



