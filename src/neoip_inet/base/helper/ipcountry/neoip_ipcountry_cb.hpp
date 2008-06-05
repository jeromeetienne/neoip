/*! \file
    \brief Header of the ipcountry_t callback class
    
*/


#ifndef __NEOIP_IPCOUNTRY_CB_HPP__ 
#define __NEOIP_IPCOUNTRY_CB_HPP__ 
/* system include */
#include <string>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	ipcountry_t;
class	inet_err_t;
	
/** \brief the callback class for ipcountry_t
 */
class ipcountry_cb_t {
public:
	/** \brief callback notified by ipcountry_t when the result is known
	 */
	virtual bool neoip_ipcountry_cb(void *cb_userptr, ipcountry_t &cb_ipcountry, const inet_err_t &inet_err
					, const std::string &country_code)	throw() = 0;
	virtual ~ipcountry_cb_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_IPCOUNTRY_CB_HPP__  */



