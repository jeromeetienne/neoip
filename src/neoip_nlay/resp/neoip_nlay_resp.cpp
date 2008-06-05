/*! \file
    \brief Definition of the \ref nlay_resp_t

*/


/* system include */
/* local include */
#include "neoip_nlay_resp.hpp"
#include "neoip_nlay_full.hpp"
#include "neoip_nlay_type.hpp"
#include "neoip_nlay_profile.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"


NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                           ctor/dtor
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief default constructor
 */
nlay_resp_t::nlay_resp_t()	throw()
{
	this->nlay_nego		= NULL;
	this->scnx_resp		= NULL;
}

/** \brief Destructor
 */
nlay_resp_t::~nlay_resp_t()	throw()
{
	nipmem_zdelete scnx_resp;
	nipmem_zdelete nlay_nego;	
}

/** \brief Constructor with value
 */
nlay_resp_t::nlay_resp_t(nlay_profile_t *nlay_profile, const nlay_type_t &inner_type
					, const nlay_type_t &outter_type)	throw()
{
	this->nlay_profile	= nlay_profile;
	this->inner_type	= inner_type;
	this->outter_type	= outter_type;
	this->nlay_nego		= NULL;	
	this->scnx_resp		= NULL;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        start() functions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the action
 */
nlay_err_t	nlay_resp_t::start()			throw()
{
	scnx_err_t	scnx_err;
	// sanity check on the parameter
	DBG_ASSERT( !inner_type.is_null() );
	DBG_ASSERT( !outter_type.is_null() );
	
	// open the scnx_resp
	scnx_resp	= nipmem_new scnx_resp_t(&nlay_profile->scnx());
	// start the scnx_resp
	scnx_err	= scnx_resp->set_auxnego_cb(this, NULL)
					.set_reliable_outter_cnx(outter_type.is_reliable())
					.start();
	// if an error occured, convert it and report it 
	if( !scnx_err.succeed() ){
		// delete scnx_resp_t and mark it unused
		nipmem_zdelete	scnx_resp;
		return nlay_err_from_scnx(scnx_err);
	}
	
	// init nlay_nego
	nlay_nego	= nipmem_new nlay_nego_t(inner_type, outter_type);
	// return noerror
	return nlay_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                            pkt from lower
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief handle packet received from the lower layer
 */
nlay_err_t	nlay_resp_t::pkt_from_lower(pkt_t &pkt, const datum_t &pathid, nlay_event_t &nlay_event)
										throw()
{
	scnx_event_t	scnx_event;
	scnx_err_t	scnx_err;

	// pass the packet to scnx_resp_t
	scnx_err = scnx_resp->pkt_from_lower(pkt, pathid, scnx_event);
	// if scnx_resp_t return an error, convert and forward it
	if( !scnx_err.succeed() )	return nlay_err_from_scnx(scnx_err);
	// sanity check - the event MUST be resp_ok()
	DBG_ASSERT( scnx_event.is_sync_resp_ok() );

	// handle each possible events from its type
	switch( scnx_event.get_value() ){
	case scnx_event_t::CNX_ESTABLISHED:{
			nlay_full_t *		nlay_full;
			nlay_nego_result_t	nego_result;
			// log to debug
			KLOG_DBG("NLAY connection established");
			// get the parameter from scnx
			void *		auxnego_ptr;
			scnx_full_t *	scnx_full = scnx_event.get_cnx_established(&auxnego_ptr);
			// copy the nego_result
			DBG_ASSERT( auxnego_ptr );
			nego_result	= *((nlay_nego_result_t *)auxnego_ptr);
			// delete the auxnego_ptr
			nipmem_delete	(nlay_nego_result_t *)auxnego_ptr;
			// build the nlay_full_t
			nlay_full	= nipmem_new nlay_full_t(nlay_profile, &nego_result
							, inner_type, outter_type, scnx_full);
			// build and copy the event
			nlay_event	= nlay_event_t::build_cnx_established(nlay_full);
			break;}
	case scnx_event_t::NONE:	// happen when no connection is established
			break;
	default:	DBG_ASSERT( 0 );
	}

	return nlay_err_t::OK;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        scnx callback for nego
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief receive the auxnego payload from the ITOR_AUTH packet and build the one for RESP_AUTH
 */
scnx_err_t	nlay_resp_t::neoip_scnx_resp_auxnego_cb(void *cb_userptr, scnx_resp_t &cb_scnx_resp
				, datum_t &auxnego_reqans, void **auxnego_ptr)	throw()
{
	nlay_nego_result_t *nego_result	= nipmem_new nlay_nego_result_t();
	// zero auxnego_ptr
	*auxnego_ptr	= NULL;
	// parse the auxnego resp payload
	nlay_err_t	nlay_err = nlay_nego->parse_request(auxnego_reqans, nlay_profile, *nego_result);
	// if an error occured, exit, convert and report it
	if( !nlay_err.succeed() ){
		nipmem_delete nego_result;
		return scnx_err_t(scnx_err_t::BAD_AUXNEGO, "NLAY_ERR: "+ nlay_err.to_string());
	}
	// copy the pointer of nego_result
	*auxnego_ptr	= nego_result;
	// exit and report OK
	return scnx_err_t::OK;	
}


NEOIP_NAMESPACE_END

