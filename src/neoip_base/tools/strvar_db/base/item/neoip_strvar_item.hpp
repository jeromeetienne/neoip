/*! \file
    \brief Declaration of the strvar_item_t

*/


#ifndef __NEOIP_STRVAR_ITEM_HPP__ 
#define __NEOIP_STRVAR_ITEM_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief Handle a pool of strvar_item variable
 */
class strvar_item_t : NEOIP_COPY_CTOR_ALLOW {
private:
	std::string	key_var;	//!< the key of this variable
	std::string	val_var;	//!< the value of this variable
public:
	/*************** ctor/dtor	***************************************/
	strvar_item_t(const std::string &key_var, const std::string &val_var)		throw();

	/*************** Query function	***************************************/
	bool			is_null()	const throw()	{ return key_var.empty();	}
	const std::string &	key()		const throw()	{ return key_var;		}
	const std::string &	val()		const throw()	{ return val_var;		}

	/*************** comparison operator	*******************************/
	int	compare(const strvar_item_t & other)	  const throw();
	bool	operator ==(const strvar_item_t & other)  const throw(){ return compare(other) == 0; }
	bool	operator !=(const strvar_item_t & other)  const throw(){ return compare(other) != 0; }
	bool	operator < (const strvar_item_t & other)  const throw(){ return compare(other) <  0; }
	bool	operator <=(const strvar_item_t & other)  const throw(){ return compare(other) <= 0; }
	bool	operator > (const strvar_item_t & other)  const throw(){ return compare(other) >  0; }
	bool	operator >=(const strvar_item_t & other)  const throw(){ return compare(other) >= 0; }

	/*************** action function	*******************************/
	strvar_item_t &		key(const std::string &new_val)	throw()		{ key_var = new_val;
										  return *this;		}
	strvar_item_t &		val(const std::string &new_val)	throw()		{ val_var = new_val;
										  return *this;		}

	/*************** display function	*******************************/
	std::string	to_string()			const throw();
	friend	std::ostream &	operator << (std::ostream & os, const strvar_item_t &strvar_item)	throw()
		{ return os << strvar_item.to_string();	}	
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_STRVAR_ITEM_HPP__  */



