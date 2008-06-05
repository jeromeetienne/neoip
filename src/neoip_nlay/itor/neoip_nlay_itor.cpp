/*! \file
    \brief Definition of the \ref nlay_itor_t

*/


/* system include */
/* local include */
#include "neoip_nlay_itor.hpp"
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
nlay_itor_t::nlay_itor_t()	throw()
{
	this->nlay_nego		= NULL;
	this->scnx_itor		= NULL;	
}

/** \brief Destructor
 */
nlay_itor_t::~nlay_itor_t()	throw()
{
	if( scnx_itor )		nipmem_delete scnx_itor;	
	if( nlay_nego )		nipmem_delete nlay_nego;	
}

/** \brief Constructor with value
 */
nlay_itor_t::nlay_itor_t(nlay_profile_t *nlay_profile, const nlay_type_t &inner_type
		, const nlay_type_t &outter_type, nlay_itor_cb_t *callback, void *userptr)	throw()
{
	// copy the parameter
	this->nlay_profile	= nlay_profile;
	this->inner_type	= inner_type;
	this->outter_type	= outter_type;	
	this->nlay_nego		= NULL;
	this->scnx_itor		= NULL;
	this->callback		= callback;
	this->userptr		= userptr;	
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                           start() functions
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the action
 */
nlay_err_t	nlay_itor_t::start()			throw()
{
	scnx_err_t	scnx_err;
	
	// sanity check on the parameter
	DBG_ASSERT( callback );
	DBG_ASSERT( !inner_type.is_null() );
	DBG_ASSERT( !outter_type.is_null() );
	
	// open the scnx_itor
	scnx_itor	= nipmem_new scnx_itor_t(&nlay_profile->scnx());
	// set scnx_itor_t parameters
	scnx_itor->set_event_cb(this, NULL);
	scnx_itor->set_auxnego_cb(this, NULL);
	scnx_itor->set_reliable_outter_cnx(outter_type.is_reliable());
	// start the scnx_itor
	scnx_err	= scnx_itor->start();
	// if an error occured, convert it and report it 
	if( !scnx_err.succeed() ){
		// delete scnx_itor_t and mark it unused
		nipmem_zdelete	scnx_itor;
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
nlay_err_t	nlay_itor_t::pkt_from_lower(pkt_t &pkt, nlay_event_t &nlay_event)	throw()
{
	scnx_event_t	scnx_event;

	// zero the nlay_event_t
	nlay_event	= nlay_event_t();	

	// pass the packet to scnx_itor_t
	scnx_err_t	scnx_err = scnx_itor->pkt_from_lower(pkt, scnx_event);
	// if scnx_itor_t return an error, convert and forward it
	if( !scnx_err.succeed() )	return nlay_err_from_scnx(scnx_err);
	// sanity check - the event MUST be itor_ok()
	DBG_ASSERT( scnx_event.is_sync_itor_ok() );

	// handle each possible events from its type
	switch( scnx_event.get_value() ){
	case scnx_event_t::CNX_ESTABLISHED:{
			nlay_full_t *		nlay_full;
			nlay_nego_result_t	nego_result;
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
	case scnx_event_t::CNX_REFUSED:{
			// build the event
			std::string	reason	= scnx_event.get_cnx_refused_reason();
			nlay_event	= nlay_event_t::build_cnx_refused(reason);
			break;}	
	case scnx_event_t::NONE:	// happen when no connection is established
			break;
	default:	DBG_ASSERT( 0 );
	}
	
	// return no error
	return nlay_err_t::OK;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        scnx_itor_event
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief receive \ref scnx_event_t from \ref scnx_itor_t
 */
bool nlay_itor_t::neoip_scnx_itor_event_cb(void *cb_userptr, scnx_itor_t &cb_scnx_itor
							, const scnx_event_t &scnx_event) throw()
{
	nlay_event_t	nlay_event;
	// log to debug
	KLOG_DBG("received scnx_event=" << scnx_event);
	// sanity check - check the scnx_event is allowed for a itor
	DBG_ASSERT( scnx_event.is_async_itor_ok() );

	// handle each possible events from its type
	switch( scnx_event.get_value() ){
	case scnx_event_t::PKT_TO_LOWER:
			nlay_event = nlay_event_t::build_pkt_to_lower(scnx_event.get_pkt_to_lower());
			return notify_callback(nlay_event);
	default:	DBG_ASSERT( 0 );
	}
	// return tokeep
	return true;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        scnx callback for nego
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief build the auxnego payload contained in the ITOR_AUTH packet
 */
datum_t nlay_itor_t::neoip_scnx_itor_build_auxnego_cb(void *cb_userptr, scnx_itor_t &cb_scnx_itor) throw()
{
	datum_t		auxnego_req;
	nlay_err_t	nlay_err;
	// build the auxnego itor payload
	nlay_err	= nlay_nego->build_request(auxnego_req, nlay_profile);
	// log to debug
	KLOG_DBG("auxnego request=" << auxnego_req);
	// sanity check - there is no reason to fail but check just to be sure
	DBG_ASSERT( nlay_err.succeed() );
	// return the auxnego request
	return auxnego_req;
}

/** \brief receive the auxnego payload from the RESP_AUTH packet
 */
scnx_err_t nlay_itor_t::neoip_scnx_itor_recv_resp_auxnego_cb(void *cb_userptr,scnx_itor_t &cb_scnx_itor
				, const datum_t &auxnego_ans, void **auxnego_ptr) throw()
{
	nlay_nego_result_t *	nego_result	= nipmem_new nlay_nego_result_t();
	nlay_err_t		nlay_err;
	// zero auxnego_ptr
	*auxnego_ptr	= NULL;
	// parse the auxnego resp payload
	nlay_err	= nlay_nego->parse_answer(auxnego_ans, nlay_profile, *nego_result);
	// if an error occured, exit and report it
	if( !nlay_err.succeed() ){
		nipmem_delete nego_result;
		return scnx_err_t(scnx_err_t::BAD_AUXNEGO, "NLAY_ERR: "+ nlay_err.to_string());
	}
	// copy the pointer of nego_result
	*auxnego_ptr	= nego_result;	
	// report noerror
	return scnx_err_t::OK;	
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                     main function to notify event to the caller
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief notify the caller callback
 * 
 * @return a tokeep
 */
bool 	nlay_itor_t::notify_callback(const nlay_event_t &nlay_event)	throw()
{
	// sanity check - the callback MUST NOT be NULL
	DBG_ASSERT( callback );
	// backup the tokey_check_t context to check after the callback notification
	TOKEEP_CHECK_BACKUP_DFL(*callback);
	// notify the caller
	bool tokeep = callback->neoip_nlay_itor_event_cb(userptr, *this, nlay_event);
	// sanity check - tokeep MUST be false if the local object has been deleted, true otherwise 
	TOKEEP_CHECK_MATCH_DFL(tokeep);
	// return the tokeep
	return tokeep;
}

NEOIP_NAMESPACE_END

