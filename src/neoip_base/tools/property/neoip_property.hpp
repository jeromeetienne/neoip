/*! \file
    \brief Declaration of the property_t

*/


#ifndef __NEOIP_PROPERTY_HPP__ 
#define __NEOIP_PROPERTY_HPP__ 
/* system include */
#include <string>
#include <map>
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief Handle a pool of property variable
 */
class property_t : NEOIP_COPY_CTOR_ALLOW {
private:
	class				var_t;		//!< nested type for the variable
	std::map<std::string, var_t>	var_db;		//!< the database of all variable

	/*************** function to handle the database	***************/
	const var_t *	find_var(const std::string &name)				const throw();
	bool		insert_var(const std::string &name, const std::string &value)	throw();

public:
	bool	load_file(const std::string &filename)	throw();

	/*************** general variable function	***********************/
	bool	exist(const std::string &name)		const throw();
	void	remove(const std::string &name)		throw();
	bool	insert(const property_t &other)		throw();

	/*************** string function	*******************************/
	bool	insert_string(const std::string &name, const std::string &value)	throw();
	const std::string &find_string(const std::string &name, const std::string &default_value="")
										const throw();
	const std::string &find_string_pattern(const std::string &name
					, const std::string &default_value="")	const throw();
					
	/*************** integer function	*******************************/
	bool	insert_integer(const std::string &name, int value)		throw();
	int	find_integer(const std::string &name, int default_value = -1)	const throw();
	/*************** display function	*******************************/
	void		dump_dbg()			const throw();
	std::string	to_string()			const throw();
	friend	std::ostream &	operator << (std::ostream & os, const property_t &property)	throw()
		{ return os << property.to_string();	}	
};

/** \brief store a variable for property_t
 */
class property_t::var_t : NEOIP_COPY_CTOR_ALLOW {
private:
	std::string	name;	//!< the variable name
	std::string	value;	//!< the variable value
public:
	var_t(const std::string &name, const std::string &value): name(name), value(value) {}
friend	class property_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_PROPERTY_HPP__  */



