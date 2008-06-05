/*! \file
    \brief Header of the neoip_asyncexe class
    
*/


#ifndef __NEOIP_ASYNCEXE_CB_HPP__ 
#define __NEOIP_ASYNCEXE_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	asyncexe_t;
class	libsess_err_t;
class	bytearray_t;

	
/** \brief the callback class for asyncexe_t
 */
class asyncexe_cb_t {
public:
	/** \brief callback notified by asyncexe_t when the result is known
	 * 
	 * - this callback MAY delete the asyncexe_t
	 * 
	 * @param cb_userptr	the userptr provided by the caller
	 * @param asyncexe	the object created by the caller
	 * @param stdout_barray	the stdout output of the excecuted command
	 * @param exit_status	the error code returned by the excecuted command
	 */
	virtual bool neoip_asyncexe_cb(void *cb_userptr, asyncexe_t &cb_asyncexe
					, const libsess_err_t &libsess_err, const bytearray_t &stdout_barray
					, const int &exit_status)		throw() = 0;
	virtual ~asyncexe_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ASYNCEXE_CB_HPP__  */



