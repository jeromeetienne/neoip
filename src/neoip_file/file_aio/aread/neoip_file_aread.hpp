/*! \file
    \brief Header of the file_aread_t
    
*/


#ifndef __NEOIP_FILE_AREAD_HPP__ 
#define __NEOIP_FILE_AREAD_HPP__ 
/* system include */
/* local include */
#include "neoip_file_aread_cb.hpp"
#include "neoip_file_err.hpp"
#include "neoip_file_size.hpp"
#include "neoip_bytearray.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_tokeep_check.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	file_aio_t;

/** \brief class definition for file_aread
 */
class file_aread_t : NEOIP_COPY_CTOR_DENY, private zerotimer_cb_t {
private:
	file_aio_t *	file_aio;
	file_size_t	file_offset;
	file_size_t	aread_len;
	
	bytearray_t	recved_data;	//!< queue the data already read

	/*************** zerotimer	***************************************/
	zerotimer_t	zerotimer;
	bool		neoip_zerotimer_expire_cb(zerotimer_t &cb_zerotimer, void *userptr)	throw();
	
	/*************** callback stuff	***************************************/
	file_aread_cb_t*callback;	//!< callback used to notify result
	void *		userptr;	//!< userptr associated with the callback
	bool		notify_callback(const file_err_t &file_err, const datum_t &datum) throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callback
public:
	/*************** ctor/dtor	***************************************/
	file_aread_t(file_aio_t *file_aio) 		throw();
	~file_aread_t()					throw();

	/*************** Setup function	***************************************/
	file_err_t	start(const file_size_t &file_offset, const file_size_t &read_len
					, file_aread_cb_t *callback, void *userptr)	throw();
	
	/*************** Query function	***************************************/
	const file_size_t &	get_offset()	const throw()	{ return file_offset;	}
	const file_size_t &	get_len()	const throw()	{ return aread_len;	}
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_AREAD_HPP__  */



