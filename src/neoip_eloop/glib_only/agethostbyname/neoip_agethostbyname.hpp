/*! \file
    \brief Header of the \ref kad_http_t class

*/


#ifndef __NEOIP_AGETHOSTBYNAME_HPP__ 
#define __NEOIP_AGETHOSTBYNAME_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_agethostbyname_cb.hpp"
#include "neoip_inet_err.hpp"
#include "neoip_asyncop_cb.hpp"
#include "neoip_hostent.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
struct asyncop_t;

/** \brief to perform an asynchronous gethostbyname 
 * 
 * - it runs in a separate thread thus the main thread is not blocked by it
 * - the result is provided by a callback
 */
class agethostbyname_t : NEOIP_COPY_CTOR_DENY, private asyncop_completed_cb_t {
private:
	/*************** handle async operation	*******************************/
	asyncop_t *		asyncop;
	void			neoip_asyncop_completed_cb(void *cb_userptr, asyncop_t &asyncop
							, void *work_result)	throw();

	/*************** callback stuff	***************************************/
	agethostbyname_cb_t *	callback;	//!< the callback to notify on completion
	void *			userptr;	//!< the userptr associated with the callback
						// NOTE: no tokeep check as the callback return void
public:
	/*************** ctor/dtor	***************************************/
	agethostbyname_t()	throw();
	~agethostbyname_t()	throw();
	
	/*************** setup function	***************************************/
	inet_err_t	start(const std::string &hostname, agethostbyname_cb_t *callback
							, void *userptr)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_AGETHOSTBYNAME_HPP__  */


 
