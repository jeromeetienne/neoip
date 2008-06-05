/*! \file
    \brief Declaratioln of the \ref kad_bstrap_t callback
    
*/


#ifndef __NEOIP_KAD_BSTRAP_CB_HPP__ 
#define __NEOIP_KAD_BSTRAP_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class kad_bstrap_t;

/** \brief the callback class for kad_bstrap_t
 */
class kad_bstrap_cb_t {
public:
	/** \brief callback notified when a kad_bstrap_t is completed
	 */
	virtual bool neoip_kad_bstrap_cb(void *cb_userptr, kad_bstrap_t &cb_kad_bstrap)	throw() = 0;
	virtual ~kad_bstrap_cb_t() {};
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_BSTRAP_CB_HPP__  */










