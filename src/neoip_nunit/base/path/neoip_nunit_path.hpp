/*! \file
    \brief Header of the \ref nunit_path_t.cpp

*/


#ifndef __NEOIP_NUNIT_PATH_HPP__ 
#define __NEOIP_NUNIT_PATH_HPP__ 

/* system include */
#include <vector>
#include <string>
/* local include */
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class to read in files in a syncronous mode
 * 
 */
class nunit_path_t : NEOIP_COPY_CTOR_ALLOW {
private://///////////////////// constant declaration ///////////////////////////
	//! a std::string containing the name separator
	static	const std::string	NAME_SEPARATOR;
private:
	std::vector<std::string>	name_db;	//!< contain all the name of the path
	
	void	init(const std::string &pathname)	throw();
public:
	/*************** ctor/dtor	***************************************/
	nunit_path_t(const char  *path_ptr)			throw();
	nunit_path_t(const std::string &path_str = "")		throw();
	
	/*************** query function	***************************************/
	bool	is_null()	const throw();
	
	/*************** append function	*******************************/
	nunit_path_t &	operator /=(const nunit_path_t &other)	throw();
	nunit_path_t	operator / (const nunit_path_t &other)	const throw()
			{ return nunit_path_t(*this) /= other;	}
	
	/*************** array function		*******************************/
	const std::string &	operator[](size_t idx)		const throw();
	size_t			size()			const throw()	{ return name_db.size();	}
	
	nunit_path_t		get_subpath()		const throw();
	
	/*************** Comparison operator	*******************************/
	int	compare(const nunit_path_t &other)	const throw();
	bool 	operator == (const nunit_path_t & other)	const throw() { return compare(other) == 0;	}
	bool 	operator != (const nunit_path_t & other)	const throw() { return compare(other) != 0;	}
	bool 	operator <  (const nunit_path_t & other)	const throw() { return compare(other) <  0;	}
	bool 	operator <= (const nunit_path_t & other)	const throw() { return compare(other) <= 0;	}
	bool 	operator >  (const nunit_path_t & other)	const throw() { return compare(other) >  0;	}
	bool 	operator >= (const nunit_path_t & other)	const throw() { return compare(other) >= 0;	}

	/*************** display function	*******************************/
	std::string	to_string() const throw();
	friend	std::ostream & operator << (std::ostream & os, const nunit_path_t &nunit_path)
			{ return os << nunit_path.to_string();	}		
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NUNIT_PATH_HPP__  */



