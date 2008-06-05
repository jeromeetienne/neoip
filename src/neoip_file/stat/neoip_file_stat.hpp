/*! \file
    \brief Header of the \ref file_stat_t.cpp

*/


#ifndef __NEOIP_FILE_STAT_HPP__ 
#define __NEOIP_FILE_STAT_HPP__ 

/* system include */
#include <vector>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
/* local include */
#include "neoip_file_err.hpp"
#include "neoip_file_path.hpp"
#include "neoip_file_size.hpp"
#include "neoip_date.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class to stat() a file
 * 
 */
class file_stat_t : NEOIP_COPY_CTOR_ALLOW {
private:
	struct stat	stat_stt;	//!< the struct returned by stat
	bool		is_initialized;
public:
	/*************** ctor/dtor	***************************************/
	file_stat_t(const file_path_t &file_path = file_path_t())	throw();

	/*************** open/close	***************************************/
	file_err_t	open(const file_path_t &file_path)	throw();
	void		close()					throw();

	/*************** query function	***************************************/
	bool		is_null()		const throw()
			{ return is_initialized == false;				}
	file_size_t	get_size() 		const throw()
			{ DBG_ASSERT(!is_null()); return file_size_t(stat_stt.st_size);	}
	bool		is_dir()   		const throw()
			{ DBG_ASSERT(!is_null()); return S_ISDIR(stat_stt.st_mode);	}
	bool		is_regular_file()   	const throw()
			{ DBG_ASSERT(!is_null()); return S_ISREG(stat_stt.st_mode);	}
	date_t		get_mtime()  	 	const throw()
			{ DBG_ASSERT(!is_null()); return delay_t::from_sec(stat_stt.st_mtime);	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_STAT_HPP__  */



