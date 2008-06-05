/*! \file
    \brief Header of the \ref file_dir_t.cpp

*/


#ifndef __NEOIP_FILE_DIR_HPP__ 
#define __NEOIP_FILE_DIR_HPP__ 

/* system include */
#include <vector>
/* local include */
#include "neoip_file_err.hpp"
#include "neoip_file_path.hpp"
#include "neoip_file_stat.hpp"
#include "neoip_string.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class to read in files in a syncronous mode
 * 
 */
class file_dir_t : NEOIP_COPY_CTOR_ALLOW {
public:
	/** \brief The filter functor used in file_dir_t::filter()
	 */
	class filter_ftor {
	public:	virtual bool operator()(const file_path_t &file_path) const throw() = 0;
		virtual ~filter_ftor() {};
	};
	
	struct filter_hidden;		// a predefined filter to remove 'hidden files'
	struct filter_glob_nomatch;	// a predefined filter to remove files not maching a glob pattern
private:
	file_path_t		dir_path;
public:	// TODO to remove once the protoype of the cmp function for sort() is made a part of this class
	class			name_t;
	std::vector<name_t>	name_db;
public:
	/*************** setup Function	***************************************/
	file_err_t	open(const file_path_t &dir_path)	throw();
	void		close()					throw();

	/*************** query Function	***************************************/
	size_t		size()				const throw()	{ return name_db.size();	}
	file_path_t	operator[](size_t idx)		const throw()	{ return relpath(idx);		}
	
	void		remove(size_t idx)		throw();
	file_path_t	fullpath(size_t idx)		const throw();
	file_path_t	relpath(size_t idx)		const throw();
	file_path_t	basename(size_t idx)		const throw();
	
	const file_stat_t &	get_file_stat(size_t idx)	const throw();
	
	file_dir_t &	sort(const std::string &criteria_str = "n")		throw();
	file_dir_t &	filter(const file_dir_t::filter_ftor &filter_ftor)	throw();
};

/** \brief private nested class of file_dir_t to store a single name
 */
class file_dir_t::name_t : NEOIP_COPY_CTOR_ALLOW {
private:
	std::string	basename;
	file_stat_t	file_stat;
public:
	/*************** ctor/dtor	***************************************/
	name_t(const std::string &basename, const file_stat_t &file_stat)	throw(){
		// sanity check - the file_stat_t MUST NOT be null
		DBG_ASSERT( !file_stat.is_null() );
		// copy the parameter
		this->basename	= basename;
		this->file_stat	= file_stat;	
	}
	
	/*************** query Function	***************************************/
	const std::string &	get_basename()	const throw() { return basename;	}
	const file_stat_t &	get_file_stat()	const throw() { return file_stat;	}
};

/** \brief Functor to filter out 'hidden file' aka the one starting by '.'
 * 
 * - NOTE: likely to be unix fs style only
 */
struct file_dir_t::filter_hidden : public file_dir_t::filter_ftor {
	bool operator()(const file_path_t &file_path)	const throw() {
		if( file_path.basename().to_string().substr(0, 1) != "." )
			return false;
		return true;
	}
};

/** \brief Functor to filter out all files with a name which doesnt match a given pattern
 */
struct file_dir_t::filter_glob_nomatch : public file_dir_t::filter_ftor {
private:std::string	pattern;
public:	filter_glob_nomatch(const std::string &pattern)	throw() : pattern(pattern) {}
	bool operator()(const file_path_t &file_path)	const throw() {
		// if the filter doesnt match, skip this entry
		if( string_t::glob_match(pattern, file_path.to_os_path_string()) )
			return false;
		return true;
	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_DIR_HPP__  */



