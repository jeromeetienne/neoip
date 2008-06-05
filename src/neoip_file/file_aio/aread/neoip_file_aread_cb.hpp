/*! \file
    \brief Declaration of the file_aread_t
    
*/


#ifndef __NEOIP_FILE_AREAD_CB_HPP__ 
#define __NEOIP_FILE_AREAD_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	file_aread_t;
class	file_err_t;
class	datum_t;

/** \brief the callback class for file_aread_t
 */
class file_aread_cb_t {
public:
	/** \brief callback notified by \ref file_aread_t when to notify the result of the operation
	 */
	virtual bool neoip_file_aread_cb(void *cb_userptr, file_aread_t &cb_file_aread
							, const file_err_t &file_err
							, const datum_t &data)	throw() = 0;
	// virtual destructor
	virtual ~file_aread_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_AREAD_CB_HPP__  */



