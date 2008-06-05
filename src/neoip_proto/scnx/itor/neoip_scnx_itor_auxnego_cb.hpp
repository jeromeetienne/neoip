/*! \file
    \brief Header of the \ref scnx_itor_t

*/


#ifndef __NEOIP_SCNX_ITOR_AUXNEGO_HPP__ 
#define __NEOIP_SCNX_ITOR_AUXNEGO_HPP__ 
/* system include */
/* local include */
#include "neoip_scnx_err.hpp"
#include "neoip_datum.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	scnx_itor_t;

/** \brief callback declaration for the auxnego of \ref scnx_itor_t
 */
class scnx_itor_auxnego_cb_t {
public:
	/** \brief build the auxnego payload contained in the ITOR_AUTH packet
	 */
	virtual datum_t neoip_scnx_itor_build_auxnego_cb(void *cb_userptr, scnx_itor_t &cb_scnx_itor)
										throw() = 0;
	/** \brief receive the auxnego payload from the RESP_AUTH packet
	 */
	virtual scnx_err_t neoip_scnx_itor_recv_resp_auxnego_cb(void *cb_userptr
				, scnx_itor_t &cb_scnx_itor, const datum_t &payl_resp
				, void **auxnego_ptr)	throw() = 0;
	virtual ~scnx_itor_auxnego_cb_t() {};
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SCNX_ITOR_AUXNEGO_HPP__  */



