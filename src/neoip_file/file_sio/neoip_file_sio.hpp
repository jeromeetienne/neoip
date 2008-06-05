/*! \file
    \brief Header of the file_sio_t
    
*/


#ifndef __NEOIP_FILE_SIO_HPP__ 
#define __NEOIP_FILE_SIO_HPP__ 
/* system include */
/* local include */
#include "neoip_file_err.hpp"
#include "neoip_file_path.hpp"
#include "neoip_file_mode.hpp"
#include "neoip_file_perm.hpp"
#include "neoip_file_size.hpp"
#include "neoip_datum.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for file_sio
 */
class file_sio_t : NEOIP_COPY_CTOR_DENY {
private:
	file_path_t		file_path;
	file_mode_t		file_mode;
	int			file_fd;	//!< the file descriptor of the file
	int			get_fd()	const throw()	{ return file_fd;	}
public:
	/*************** ctor/dtor	***************************************/
	file_sio_t() 		throw();
	~file_sio_t()		throw();

	/*************** Setup function	***************************************/
	file_err_t	start(const file_path_t &file_path, const file_mode_t &file_mode
				, const file_perm_t &file_perm = file_perm_t::FILE_DFL)	throw();
	
	/*************** query function	***************************************/
	const file_mode_t &	get_mode()	const throw()	{ return file_mode;	}
	const file_path_t &	get_path()	const throw()	{ return file_path;	}
	
	/*************** action function	*******************************/
	file_err_t	seek(const file_size_t &offset)				const throw();
	file_err_t	write(const datum_t &data)				const throw();
	file_err_t	read(const file_size_t &len, datum_t &data_out)		const throw();
	file_err_t	read_max(const file_size_t &maxlen, datum_t &data_out)	const throw();
	
	/*************** static helper	***************************************/
	static	file_err_t	readall(const file_path_t &file_path, datum_t &data_out
						, const file_size_t &maxlen = file_size_t())	throw();
	static	file_err_t	writeall(const file_path_t &file_path, const datum_t &data
					, const file_perm_t &file_perm = file_perm_t::FILE_DFL
					, const file_mode_t &file_mode = file_mode_t::WPLUS)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_SIO_HPP__  */



