/*! \file
    \brief Header of the \ref bt_cast_pidx_t

*/


#ifndef __NEOIP_BT_CAST_PIDX_HPP__ 
#define __NEOIP_BT_CAST_PIDX_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_xmlrpc_decl.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief to store an pieceidx for a bt_cast (aka with circularidx)
 */
class bt_cast_pidx_t : NEOIP_COPY_CTOR_ALLOW {
private:
	size_t		m_modulo;
	size_t		m_index;
public:
	/*************** ctor/dtor	***************************************/
	bt_cast_pidx_t()		throw();
	
	/*************** query function	***************************************/
	bool		is_null()	const throw();
	bool		is_sane()	const throw();
	size_t		modulo()	const throw()	{ return m_modulo;	}
	size_t		index()		const throw()	{ return m_index;	}
	bool		is_in(size_t range_beg, size_t range_end)		const throw();
	size_t		is_nb_piece_before(size_t pieceidx_fwd)	const throw();
	size_t		is_nb_piece_after(size_t pieceidx_bwd)	const throw();

	/************** action function	***************************************/
	bt_cast_pidx_t&	modulo(size_t new_val)	throw();
	bt_cast_pidx_t&	index(size_t new_val)	throw();

	/************** Arithmetic operator	*******************************/
	bt_cast_pidx_t&	operator += (size_t &other)	throw();
	bt_cast_pidx_t&	operator -= (size_t &other)	throw();
	bt_cast_pidx_t&	operator ++ ()			throw();
	bt_cast_pidx_t&	operator -- ()			throw();
	bt_cast_pidx_t 	operator ++ (int dummy)		throw()	{ bt_cast_pidx_t tmp(*this); tmp ++; return tmp;	}
	bt_cast_pidx_t 	operator -- (int dummy)		throw()	{ bt_cast_pidx_t tmp(*this); tmp --; return tmp;	}

	/************** display function	*******************************/
	std::string	to_string()	const throw();
	friend	std::ostream & operator << (std::ostream & oss, const bt_cast_pidx_t &cast_pidx) throw()
					{ return oss << cast_pidx.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const bt_cast_pidx_t &cast_pidx)	throw();
	friend	serial_t& operator >> (serial_t & serial, bt_cast_pidx_t &cast_pidx) 		throw(serial_except_t);			

	/*************** xmlrpc function	*******************************/
	friend	xmlrpc_build_t &operator << (xmlrpc_build_t& xmlrpc_build, const bt_cast_pidx_t &cast_pidx)	throw();
	friend	xmlrpc_parse_t &operator >> (xmlrpc_parse_t& xmlrpc_parse, bt_cast_pidx_t &cast_pidx)		throw(xml_except_t);
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_CAST_PIDX_HPP__  */



