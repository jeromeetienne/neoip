/*! \file
    \brief Header of the file_aio_t
    
*/


#ifndef __NEOIP_FILE_AIO_HPP__ 
#define __NEOIP_FILE_AIO_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_file_aio_profile.hpp"
#include "neoip_file_err.hpp"
#include "neoip_file_path.hpp"
#include "neoip_file_mode.hpp"
#include "neoip_file_perm.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	file_aread_t;
class	file_awrite_t;

/** \brief class definition for file_aio
 */
class file_aio_t : NEOIP_COPY_CTOR_DENY {
private:
	file_path_t		file_path;
	file_mode_t		file_mode;
	file_aio_profile_t	profile;

	int			file_fd;	//!< the file descriptor of the file
	int			get_fd()	const throw()	{ return file_fd;	}
	
	/*************** List of file_aread_t	*******************************/
	std::list<file_aread_t *>	aread_db;
	void aread_dolink(file_aread_t *aread)		throw()	{ aread_db.push_back(aread);	}
	void aread_unlink(file_aread_t *aread)		throw()	{ aread_db.remove(aread);	}
	/*************** List of file_awrite_t	*******************************/
	std::list<file_awrite_t *>	awrite_db;
	void awrite_dolink(file_awrite_t *awrite)	throw()	{ awrite_db.push_back(awrite);	}
	void awrite_unlink(file_awrite_t *awrite)	throw()	{ awrite_db.remove(awrite);	}
public:
	/*************** ctor/dtor	***************************************/
	file_aio_t() 		throw();
	~file_aio_t()		throw();

	/*************** Setup function	***************************************/
	file_aio_t &	set_profile(const file_aio_profile_t &profile)	throw();	
	file_err_t	start(const file_path_t &file_path, const file_mode_t &file_mode
				, const file_perm_t &file_perm = file_perm_t::FILE_DFL)	throw();
	
	/*************** query function	***************************************/
	const file_mode_t &		get_mode()	const throw()	{ return file_mode;	}
	const file_path_t &		get_path()	const throw()	{ return file_path;	}
	const file_aio_profile_t &	get_profile()	const throw()	{ return profile;	}

	/*************** display function	*******************************/
	std::string			to_string()	const throw();
	friend	std::ostream &	operator << (std::ostream& os, const file_aio_t &file_aio)	throw()
					{ return os << file_aio.to_string();	}
		
	/*************** List of friend class	*******************************/
	friend class	file_aread_t;
	friend class	file_awrite_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_AIO_HPP__  */



