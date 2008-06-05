/*! \file
    \brief Declaratioln of the \ref kad_bstrap_cand_t callback
    
*/


#ifndef __NEOIP_KAD_BSTRAP_CAND_CB_HPP__ 
#define __NEOIP_KAD_BSTRAP_CAND_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_bstrap_cand_t;
class	kad_err_t;

/** \brief the callback class for kad_bstrap_cand_t
 */
class kad_bstrap_cand_cb_t {
public:
	/** \brief callback notified when a kad_bstrap_cand_t is completed
	 */
	virtual bool neoip_kad_bstrap_cand_cb(void *cb_userptr, kad_bstrap_cand_t &cb_kad_bstrap
							, const kad_err_t &kad_err)	throw() = 0;
	virtual ~kad_bstrap_cand_cb_t() {};
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_BSTRAP_CAND_CB_HPP__  */










