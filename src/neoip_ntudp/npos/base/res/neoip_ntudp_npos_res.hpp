/*! \file
    \brief Header of the \ref ntudp_npos_res_t

- TODO put a natkeepalive_delay
  - no idea how to diagnose it
    - and if i do it will be very long to diagnose
    => workaround = put a default value in source and configuration file

*/


#ifndef __NEOIP_NTUDP_NPOS_RES_HPP__ 
#define __NEOIP_NTUDP_NPOS_RES_HPP__ 
/* system include */
#include <iostream>
/* local include */
#include "neoip_ntudp_npos_res_wikidbg.hpp"
#include "neoip_ip_addr.hpp"
#include "neoip_ntudp_cnxtype.hpp"
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	ipport_aview_t;

/** \brief to store the result of the ntudp_npos_eval_t
 */
class ntudp_npos_res_t : NEOIP_COPY_CTOR_ALLOW
				, private wikidbg_obj_t<ntudp_npos_res_t, ntudp_npos_res_wikidbg_init>
				{
private:
#define	DEF_NTUDP_NPOS_RES_FIELD(item_type, item_name, class_name)									\
	private:	item_type	item_name##_val;						\
			bool		item_name##_been_set;						\
	public:												\
		class_name &item_name(const item_type &val)	throw()					\
			{ item_name##_val = val; item_name##_been_set = true; return *this; }		\
		const item_type &item_name()const throw()	{ DBG_ASSERT(item_name##_present());	\
								return item_name##_val; }		\
		bool  item_name##_present()	const throw()	{ return item_name##_been_set;}
	DEF_NTUDP_NPOS_RES_FIELD(bool, natted			, ntudp_npos_res_t);
	DEF_NTUDP_NPOS_RES_FIELD(bool, natlback			, ntudp_npos_res_t);
	DEF_NTUDP_NPOS_RES_FIELD(bool, natsym			, ntudp_npos_res_t);
	DEF_NTUDP_NPOS_RES_FIELD(bool, inetreach		, ntudp_npos_res_t);
	DEF_NTUDP_NPOS_RES_FIELD(ip_addr_t, local_ipaddr_lview	, ntudp_npos_res_t);
	DEF_NTUDP_NPOS_RES_FIELD(ip_addr_t, local_ipaddr_pview	, ntudp_npos_res_t);
#undef DEF_NTUDP_NPOS_RES_FIELD
public:
	/*************** ctor/dtor	***************************************/
	ntudp_npos_res_t()	throw();
	~ntudp_npos_res_t()	throw();

	static ntudp_npos_res_t	from_ipport_aview(const ipport_aview_t &ipport_aview)	throw();
	
	/*************** Basic query function	*******************************/
	bool				is_null()	const throw()	{ return !completed();	}
	bool				completed()	const throw();
	static	ntudp_npos_res_t	get_default()	throw();
	ntudp_cnxtype_t			cpu_cnxtype(const ntudp_npos_res_t &resp_res)	const throw();
	bool				need_relay_for_resp()				const throw();

	/*************** Comparison function	*******************************/
	int	compare(const ntudp_npos_res_t &other)		const throw();
	bool 	operator == (const ntudp_npos_res_t & other)	const throw()	{ return compare(other) == 0;	}
	bool 	operator != (const ntudp_npos_res_t & other)	const throw()	{ return compare(other) != 0;	}
	bool 	operator <  (const ntudp_npos_res_t & other)	const throw()	{ return compare(other) <  0;	}
	bool 	operator <= (const ntudp_npos_res_t & other)	const throw()	{ return compare(other) <= 0;	}
	bool 	operator >  (const ntudp_npos_res_t & other)	const throw()	{ return compare(other) >  0;	}
	bool 	operator >= (const ntudp_npos_res_t & other)	const throw()	{ return compare(other) >= 0;	}
	
	/*************** display function	*******************************/
	std::string		to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & os, const ntudp_npos_res_t &npos_res)
							{ return os << npos_res.to_string();	}

	/*************** serial_t function	*******************************/
	friend serial_t& operator << (serial_t& serial, const ntudp_npos_res_t &ntudp_npos_res)	throw();
	friend serial_t& operator >> (serial_t & serial, ntudp_npos_res_t &ntudp_npos_res)	throw(serial_except_t);	

	/*************** List of friend classes	*******************************/
	friend class	ntudp_npos_res_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_NPOS_RES_HPP__  */



