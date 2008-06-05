/*! \file
    \brief Header of the \ref bt_jamrc4_itor_t

\par ITOR algo Description
- if m_profile.itor_type_default().is_nojam(), send nothing and spawn a 
  bt_jamrc4_full_t without any skey_ciph_t
- if m_profile.itor_type_default().is_dojam(), send a bt_jamrc4_type_t::DOJAM header
  - then wait of bt_jamrc4_resp_t to reply its own bt_jamrc4_type_t::DOJAM header
- on bt_jamrc4_resp_t header reception, the 'check' is verified
  - if the verification fails, an error is notified
  - if the verification succeed, a bt_jamrc4_full_t is spawned 

*/

/* system include */
/* local include */
#include "neoip_bt_jamrc4_itor.hpp"
#include "neoip_bt_jamrc4_full.hpp"
#include "neoip_bt_jamrc4_helper.hpp"
#include "neoip_bt_err.hpp"
#include "neoip_skey_ciph.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                               CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Default Constructor
 */
bt_jamrc4_itor_t::bt_jamrc4_itor_t()	throw()
{
}

/** \brief Destructor
 */
bt_jamrc4_itor_t::~bt_jamrc4_itor_t()	throw()
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
bt_jamrc4_itor_t &bt_jamrc4_itor_t::profile(const bt_jamrc4_profile_t &p_profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( p_profile.check() == bt_err_t::OK );	
	// copy the parameter
	this->m_profile	= p_profile;
	// return the object iself
	return *this;
}

/** \brief Start the action
 */
bt_err_t bt_jamrc4_itor_t::start()	throw()
{
	// get a random nonce
	m_itor_nonce	= bt_id_t::build_random();
	// put nonce+check into m_xmit_buffer IIF m_profile.itor_type_default().is_dojam()
	if( m_profile.itor_type_default().is_dojam() ){
		m_xmit_buffer << m_itor_nonce;
		m_xmit_buffer << bt_id_t(m_itor_nonce.to_datum());
	}
	// return no error
	return bt_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief used by the caller to notify recved_pkt
 * 
 * @param jamrc4_full_out	if returned as non-null, it contains a bt_jamrc4_full_t
 *			and the connection is considered established
 * 
 * @return a bt_err_t. if bt_err.failed() then the connection should be dropped
 */
bt_err_t bt_jamrc4_itor_t::notify_recved_data(const pkt_t &recved_pkt
					, bt_jamrc4_full_t **jamrc4_full_out)	throw()
{
	skey_ciph_t * xmit_ciph	= NULL;
	skey_ciph_t * recv_ciph = NULL;
	// set jamrc4_full_out to NULL by default
	*jamrc4_full_out	= NULL;

	// put the recved_pkt into the m_recv_buffer
	m_recv_buffer.append(recved_pkt.to_datum(datum_t::NOCOPY));

	// parse the resp data IIF m_profile.itor_type_default().is_dojam()
	if( m_profile.itor_type_default().is_dojam() ){
		// if m_recv_buffer not yet received the remote nonce, return now
		if( m_recv_buffer.length() < 2*bt_id_t::size() )	return bt_err_t::OK;
	
		// get the resp_nonce from m_recv_buffer
		bt_id_t		resp_nonce, resp_check;
		m_recv_buffer	>> resp_nonce;
		m_recv_buffer	>> resp_check;
		
		// if resp_check is invalid, return an error
		if( resp_check != bt_id_t(resp_nonce.to_datum()) )
			return bt_err_t(bt_err_t::ERROR, "received an invalid resp_check");

		// build xmit_ciph and recv_ciph
		xmit_ciph	= bt_jamrc4_helper_t::build_xmit_ciph(m_itor_nonce, resp_nonce, true);
		recv_ciph	= bt_jamrc4_helper_t::build_recv_ciph(m_itor_nonce, resp_nonce, true);
		if( !xmit_ciph || !recv_ciph ){
			nipmem_zdelete	xmit_ciph;
			nipmem_zdelete	recv_ciph;
			return bt_err_t(bt_err_t::ERROR, "unable to init xmit/recv_ciph");
		}
	}
		
	// spawn a bt_jamrc4_full_t and copy it to *jamrc4_full_out
	bt_jamrc4_full_t *	jamrc4_full;
	jamrc4_full	= nipmem_new bt_jamrc4_full_t();
	bt_err_t bt_err	= jamrc4_full->start(xmit_ciph, recv_ciph, m_recv_buffer);
	if( bt_err.failed() ){
		nipmem_zdelete	jamrc4_full;
		return bt_err;
	}
	*jamrc4_full_out	= jamrc4_full;

	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END




