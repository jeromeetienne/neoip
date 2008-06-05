/*! \file
    \brief Header of the \ref scnx_resp_auxnego_cb_t
*/


#ifndef __NEOIP_SCNX_RESP_AUXNEGO_CB_HPP__ 
#define __NEOIP_SCNX_RESP_AUXNEGO_CB_HPP__ 
/* system include */
/* local include */
#include "neoip_scnx_err.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	scnx_resp_t;
class	datum_t;

/** \brief callback declaration for the auxnego of \ref scnx_resp_t
 */
class scnx_resp_auxnego_cb_t {
public:	
	/** \brief receive the auxnego payload from the ITOR_AUTH packet and build the one for RESP_AUTH
	 */
	virtual scnx_err_t neoip_scnx_resp_auxnego_cb(void *cb_userptr, scnx_resp_t &cb_scnx_resp
				, datum_t &auxnego_datum, void **auxnego_ptr)	throw() = 0;
	//! virtual destructor
	virtual ~scnx_resp_auxnego_cb_t() {};
};
\
NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SCNX_RESP_AUXNEGO_CB_HPP__  */



