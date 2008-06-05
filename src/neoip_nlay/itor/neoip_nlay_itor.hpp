/*! \file
    \brief Header of the \ref nlay_itor_t

- see \ref neoip_nlay_itor.cpp
*/


#ifndef __NEOIP_NLAY_ITOR_HPP__ 
#define __NEOIP_NLAY_ITOR_HPP__ 
/* system include */
/* local include */
#include "neoip_nlay_itor_cb.hpp"
#include "neoip_nlay_err.hpp"
#include "neoip_nlay_event.hpp"
#include "neoip_nlay_type.hpp"
#include "neoip_scnx_itor.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	nlay_profile_t;
class	nlay_nego_t;


/** \brief to handle the itor in the nlay stack
 */
class nlay_itor_t : public scnx_itor_auxnego_cb_t, public scnx_itor_cb_t, NEOIP_COPY_CTOR_DENY {
private:
	nlay_profile_t *nlay_profile;	//!< pointer on the nlay_profile_t
	nlay_type_t	inner_type;	//!< the inner type of this connection
	nlay_type_t	outter_type;	//!< the outter type of this connection
	nlay_nego_t *	nlay_nego;	//!< pointer on the nlay_nego_t

	/*************** scnx_itor_t	***************************************/
	scnx_itor_t *	scnx_itor;
	bool		neoip_scnx_itor_event_cb(void *cb_userptr, scnx_itor_t &cb_scnx_itor
					, const scnx_event_t &scnx_event) 		throw();
					
	/*************** scnx_itor_t auxnego callback	***********************/
	datum_t		neoip_scnx_itor_build_auxnego_cb(void *cb_userptr, scnx_itor_t &cb_scnx_itor)
											throw();
	scnx_err_t	neoip_scnx_itor_recv_resp_auxnego_cb(void *cb_userptr,scnx_itor_t &cb_scnx_itor
					, const datum_t &payl_resp, void **auxnego_ptr)	throw();

	/*************** Callback	***************************************/
	nlay_itor_cb_t *callback;
	void *		userptr;
	bool		notify_callback(const nlay_event_t &nlay_event)	throw();
	TOKEEP_CHECK_DECL_DFL();	//!< used to sanity check the 'tokeep' value returned by callbacks
public:
	/*************** ctor/dtor	***************************************/
	nlay_itor_t(nlay_profile_t *nlay_profile, const nlay_type_t &inner_type
				, const nlay_type_t &outter_type
				, nlay_itor_cb_t *callback, void *userptr)	throw();
	nlay_itor_t()	throw();
	~nlay_itor_t()	throw();	

	/*************** Setup function	***************************************/
	nlay_err_t	start()		throw();

	/*************** Query function	***************************************/
	bool		is_null()	const throw()	{ return nlay_profile == NULL;	}
	
	/*************** Action function	*******************************/
	nlay_err_t	pkt_from_lower(pkt_t &pkt,  nlay_event_t &nlay_event)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NLAY_ITOR_HPP__  */



