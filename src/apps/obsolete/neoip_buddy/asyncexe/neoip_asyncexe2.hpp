/*! \file
    \brief Header of the \ref kad_http_t class

*/


#ifndef __NEOIP_ASYNCEXE_HPP__ 
#define __NEOIP_ASYNCEXE_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_asyncexe_cb.hpp"
#include "neoip_asyncop_cb.hpp"
#include "neoip_hostent.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
struct asyncop_t;

/** \brief to excecute a executable in a separate thread and get back the result
 */
class asyncexe_t : NEOIP_COPY_CTOR_DENY, private asyncop_completed_cb_t {
private:
	asyncexe_cb_t *	callback;	//!< the callback to notify on completion
	void *		userptr;	//!< the userptr associated with the callback
	
	/*************** handle async operation	*******************************/
	asyncop_t *	asyncop;
	void		neoip_asyncop_completed_cb(void *cb_userptr, asyncop_t &asyncop
							, void *work_result)	throw();
public:
	/*************** ctor/dtor	***************************************/
	asyncexe_t()	throw();
	~asyncexe_t()	throw();
	
	/*************** setup function	***************************************/
	bool		start(const std::string &cmd_line, asyncexe_cb_t *callback
							, void *userptr)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_ASYNCEXE_HPP__  */


 
