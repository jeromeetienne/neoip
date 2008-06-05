/*! \file
    \brief Header of the \ref file_path2_t.cpp

*/


#ifndef __NEOIP_FILE_PATH2_HPP__ 
#define __NEOIP_FILE_PATH2_HPP__ 

/* system include */
#include <vector>
#include <string>
/* local include */
#include "neoip_file_err.hpp"
#include "neoip_serial.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class to handle file path2
 * 
 */
class file_path2_t : NEOIP_COPY_CTOR_ALLOW {
private://///////////////////// constant declaration ///////////////////////////
	//! a std::string containing the filesystem directory separator
	static	const std::string	FS_DIR_SEPARATOR;
	//! a std::string containing the filesystem directory placeholder
	static	const std::string	FS_DIR_PLACEHOLDER;	
	//! a std::string containing the filesystem directory parent
	static	const std::string	FS_DIR_PARENT;	
private:
	std::vector<std::string>	name_db;
	std::string			m_drvletter;
	
	/*************** Internal function	*******************************/
	void		ctor_from_str(const std::string &path2name)		throw();
	void		normalize()						throw();
	std::string	to_string_sepa(const std::string &dir_separator)	const throw();
	static bool	is_drvletter(const std::string &str)			throw();
public:
	/*************** ctor/dtor	***************************************/
	file_path2_t(const char  *path2_ptr)			throw();
	file_path2_t(const std::string &path2_str = "")		throw();
	
	/*************** query function	***************************************/
	bool		is_null()				const throw();
	bool		is_absolute()				const throw();
	bool		is_relative()				const throw()	{ return !is_absolute();}
	bool		contain(const file_path2_t &other)	const throw();
	
	file_path2_t	dirname()				const throw();
	file_path2_t	basename()				const throw();
	std::string	basename_ext()				const throw();
	std::string	basename_noext()			const throw();
	file_path2_t	fullpath2()				const throw();
	file_path2_t	subpath(size_t idx)			const throw();

	const std::string &	drvletter()	const throw()	{ return m_drvletter;		}
	bool			has_drvletter()	const throw()	{ return !drvletter().empty();	}
	
	/*************** append function	*******************************/
	file_path2_t &	operator /=(const file_path2_t &other)	throw();
	file_path2_t	operator / (const file_path2_t &other)	const throw()
			{ return file_path2_t(*this) /= other;	}

	/*************** level function	*******************************/
	size_t		size()				const throw();
	file_path2_t	operator[](size_t idx)		const throw();
	
	/*************** Comparison operator	*******************************/
	int	compare(const file_path2_t &other)	const throw();
	bool 	operator == (const file_path2_t & other)	const throw() { return compare(other) == 0;	}
	bool 	operator != (const file_path2_t & other)	const throw() { return compare(other) != 0;	}
	bool 	operator <  (const file_path2_t & other)	const throw() { return compare(other) <  0;	}
	bool 	operator <= (const file_path2_t & other)	const throw() { return compare(other) <= 0;	}
	bool 	operator >  (const file_path2_t & other)	const throw() { return compare(other) >  0;	}
	bool 	operator >= (const file_path2_t & other)	const throw() { return compare(other) >= 0;	}

	/*************** display function	*******************************/
	std::string	to_unxpath_string()	const throw();
	std::string	to_winpath_string()	const throw();
	std::string	to_urlpath_string()	const throw();
	std::string	to_os_path_string()	const throw();
	std::string	to_string() 		const throw()	{ return to_os_path_string();	}
	friend	std::ostream & operator << (std::ostream & os, const file_path2_t &file_path2)
					{ return os << file_path2.to_string();	}

	/*************** serial_t function	*******************************/
	friend	serial_t& operator << (serial_t& serial, const file_path2_t &file_path2)	throw();
	friend	serial_t& operator >> (serial_t & serial, file_path2_t &file_path2) 	throw(serial_except_t);			
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_PATH2_HPP__  */



