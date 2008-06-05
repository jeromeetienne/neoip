/*! \file
    \brief Header of the \ref file_path_t.cpp

*/


#ifndef __NEOIP_FILE_PATH_HPP__ 
#define __NEOIP_FILE_PATH_HPP__ 

/* system include */
#include <vector>
#include <string>
/* local include */
#include "neoip_file_err.hpp"
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class to handle file path
 * 
 */
class file_path_t : NEOIP_COPY_CTOR_ALLOW {
private://///////////////////// constant declaration ///////////////////////////
	//! a std::string containing the filesystem directory separator
	static	const std::string	FS_DIR_SEPARATOR;
	//! a std::string containing the filesystem directory placeholder
	static	const std::string	FS_DIR_PLACEHOLDER;	
	//! a std::string containing the filesystem directory parent
	static	const std::string	FS_DIR_PARENT;	
private:
	std::vector<std::string>	name_db;
	
	void	init(const std::string &pathname)	throw();
	void	normalize()				throw();
public:
	/*************** ctor/dtor	***************************************/
	file_path_t(const char  *path_ptr)			throw();
	file_path_t(const std::string &path_str = "")		throw();
	
	/*************** query function	***************************************/
	bool		is_null()				const throw();
	bool		is_absolute()				const throw();
	bool		is_relative()				const throw()	{ return !is_absolute();}
	bool		contain(const file_path_t &other)	const throw();
	file_path_t	dirname()				const throw();
	file_path_t	basename()				const throw();
	std::string	basename_ext()				const throw();
	std::string	basename_noext()			const throw();
	file_path_t	fullpath()				const throw();
	file_path_t	subpath(size_t idx)			const throw();
	
	/*************** append function	*******************************/
	file_path_t &	operator /=(const file_path_t &other)	throw();
	file_path_t	operator / (const file_path_t &other)	const throw()
			{ return file_path_t(*this) /= other;	}

	/*************** level function	*******************************/
	size_t		size()				const throw();
	file_path_t	operator[](size_t idx)		const throw();
	
	/*************** Comparison operator	*******************************/
	int	compare(const file_path_t &other)	const throw();
	bool 	operator == (const file_path_t & other)	const throw() { return compare(other) == 0;	}
	bool 	operator != (const file_path_t & other)	const throw() { return compare(other) != 0;	}
	bool 	operator <  (const file_path_t & other)	const throw() { return compare(other) <  0;	}
	bool 	operator <= (const file_path_t & other)	const throw() { return compare(other) <= 0;	}
	bool 	operator >  (const file_path_t & other)	const throw() { return compare(other) >  0;	}
	bool 	operator >= (const file_path_t & other)	const throw() { return compare(other) >= 0;	}

	/*************** display function	*******************************/
	std::string	to_string() const throw();
	friend	std::ostream & operator << (std::ostream & os, const file_path_t &file_path)
					{ return os << file_path.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const file_path_t &file_path)	throw();
	friend	serial_t& operator >> (serial_t & serial, file_path_t &file_path) 	throw(serial_except_t);			
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_PATH_HPP__  */



