/*! \file
    \brief Declaration of the file_awrite_t
    
*/


#ifndef __NEOIP_FILE_AWRITE_CB_HPP__ 
#define __NEOIP_FILE_AWRITE_CB_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// forward declaration
class	file_awrite_t;
class	file_err_t;

/** \brief the callback class for file_awrite_t
 */
class file_awrite_cb_t {
public:
	/** \brief callback notified by \ref file_awrite_t when to notify the result of the operation
	 */
	virtual bool neoip_file_awrite_cb(void *cb_userptr, file_awrite_t &cb_file_awrite
							, const file_err_t &file_err)	throw() = 0;
	// virtual destructor
	virtual ~file_awrite_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_FILE_AWRITE_CB_HPP__  */



