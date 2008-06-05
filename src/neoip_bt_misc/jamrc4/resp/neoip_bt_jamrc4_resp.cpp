/*! \file
    \brief Header of the \ref bt_jamrc4_resp_t

\par JAMRC4 protocol goal
- this protocol has been made to consume very little rescource
  - no expensive asymmetric public key crypto
  - no overhead for the data
  - overhead of 40-byte for cnx establishement (i.e. the bt_jamrc4_type_t::DOJAM header)
- Its goals is only to conceal the internal technology of neoip-apps
  - aka the fact it uses bittorrent
  - in the hope of slowing down possible copies of it
- Additionnaly it support easy detection of bt_jamrc4_type_t::NOJAM peers
  in order to allow compatibility with plain bt implementation. 
  - this requirement makes thing a little less eleguant.

\par RESP algo Description
- resp receives data from the itor
- if resp accepts DOJAM, it considere those data as being itor header
  - it check itor header validity
  - if it is valid, it spawn a full
    - and send its own bt_jamrc4_type_t::DOJAM header
  - if it is invalid and if resp doesnt accepts NOJAM, return an error
  - if it is invalid and if resp does accepts NOJAM, spawn a full
    - without consuming the data sent instead of the DOJAM header
  - else notify an error

\par ITOR algo Description
- if m_profile.itor_type_default().is_nojam(), send nothing and spawn a 
  bt_jamrc4_full_t without any skey_ciph_t
- if m_profile.itor_type_default().is_dojam(), send a bt_jamrc4_type_t::DOJAM header
  - then wait of bt_jamrc4_resp_t to reply its own bt_jamrc4_type_t::DOJAM header
- on bt_jamrc4_resp_t header reception, the 'check' is verified
  - if the verification fails, an error is notified
  - if the verification succeed, a bt_jamrc4_full_t is spawned 

\par Description of a bt_jamrc4_type_t::DOJAM header
- a header is composed of a 'nonce' followed by a 'check'
  - a 'nonce' is a random number of 20-byte (as in SHA1 output size, aka bt_id_t)
  - a 'check' is a SHA1 output computed over the nonce
  - so the whole header is 40-byte long
- the 'check' has a single goal: ensure it is unlikely to distinguish a bt_jamrc4_itor_t
  doing bt_jamrc4_type_t::DOJAM from one doing bt_jamrc4_type_t::NOJAM
  - the likelyhood to get a bt_jamrc4_type_t::NOJAM which has a valid 'check'
    is very low due to the SHA1 output size

\par jamrc4 protocol once the connection is established
- if the connection has been established in bt_jamrc4_type_t::NOJAM, the data are
  exchange without any modification (no encryption or anything)
- if the connection has been established in bt_jamrc4_type_t::DOJAM, the data are
  exchange encrypted by RC4

\par JAMRC4 connection establishement for bt_jamrc4_type_t::DOJAM itor 
- itor sends a bt_jamrc4_type_t::DOJAM header
- resp receives it and check its validity
  - if it is valid and if resp accepts DOJAM, it spawn a full
    - and send its own bt_jamrc4_type_t::DOJAM header
  - if it is invalid and if resp accepts NOJAM, it spawn a full
    - without consuming the data sent instead of the DOJAM header
    - and send its own bt_jamrc4_type_t::DOJAM header
  - else notify an error
- itor receives resp header and check its validity
  - if it is valid, spawn a full
  - else notify an error

\par JAMRC4 connection establishement for bt_jamrc4_type_t::NOJAM itor 
- itor sends connect and send data
  - BIG ASSUMPTION: on establishement
- resp receives it and check its validity
  - if it is valid and if resp accepts DOJAM, it spawn a full
    - and send its own bt_jamrc4_type_t::DOJAM header
  - if it is invalid and if resp accepts NOJAM, it spawn a full
    - without consuming the data sent instead of the DOJAM header
    - and send its own bt_jamrc4_type_t::DOJAM header
  - else notify an error
- itor receives resp header and check its validity
  - if it is valid, spawn a full
  - else notify an error


*/

/* system include */
/* local include */
#include "neoip_bt_jamrc4_resp.hpp"
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
bt_jamrc4_resp_t::bt_jamrc4_resp_t()	throw()
{
}

/** \brief Destructor
 */
bt_jamrc4_resp_t::~bt_jamrc4_resp_t()	throw()
{
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
bt_jamrc4_resp_t &bt_jamrc4_resp_t::profile(const bt_jamrc4_profile_t &p_profile)	throw()
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
bt_err_t bt_jamrc4_resp_t::start()	throw()
{
	// get a random nonce
	m_resp_nonce	= bt_id_t::build_random();

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
 * - NOTE: TODO: this function is a mess... rewrite it
 * 
 * @param jamrc4_full_out	if returned as non-null, it contains a bt_jamrc4_full_t
 *			and the connection is considered established
 * 
 * @return a bt_err_t. if bt_err.failed() then the connection should be dropped
 */
bt_err_t bt_jamrc4_resp_t::notify_recved_data(const pkt_t &recved_pkt
					, bt_jamrc4_full_t **jamrc4_full_out)	throw()
{
	bool	accept_nojam	= m_profile.resp_type_accept_arr().contain(bt_jamrc4_type_t::NOJAM);
	bool	accept_dojam	= m_profile.resp_type_accept_arr().contain(bt_jamrc4_type_t::DOJAM);
	skey_ciph_t * xmit_ciph	= NULL;
	skey_ciph_t * recv_ciph = NULL;
	// set jamrc4_full_out to NULL by default
	*jamrc4_full_out	= NULL;

	// put the recved_pkt into the m_recv_buffer
	m_recv_buffer.append(recved_pkt.to_datum(datum_t::NOCOPY));

	// try to parse the jam header if accept_dojam is true
	if( accept_dojam ){
		// if m_recv_buffer not yet received the remote nonce, return now
		if( m_recv_buffer.length() < 2*bt_id_t::size() )	return bt_err_t::OK;
	
		// get the itor_nonce/itor_check from m_recv_buffer
		// - NOTE: do a copy in case it is a bt_jamrc4_type_t::NOJAM connection
		bt_id_t		itor_nonce, itor_check;
		pkt_t		recvbuf_copy	= m_recv_buffer;
		recvbuf_copy	>> itor_nonce;
		recvbuf_copy	>> itor_check;

		// determine if itor_check is valid
		bool is_check_valid	= itor_check == bt_id_t(itor_nonce.to_datum());

		// if itor_check is invalid and accept_nojam is false, return an error
		if( !is_check_valid && !accept_nojam )
			return bt_err_t(bt_err_t::ERROR, "received an invalid itor_check");
		
		// if is_check_valid, send the header and build the cipher
		if( is_check_valid ){
			// remove data from the m_recv_buffer
			m_recv_buffer	>> itor_nonce >> itor_check;
		
			// sanity check - accept_dojam MUST be true
			DBG_ASSERT( accept_dojam );
			// put nonce+check into m_xmit_buffer IIF is_check_valid
			m_xmit_buffer << m_resp_nonce;
			m_xmit_buffer << bt_id_t(m_resp_nonce.to_datum());	
			
			// build xmit_ciph and recv_ciph
			xmit_ciph	= bt_jamrc4_helper_t::build_xmit_ciph(itor_nonce, m_resp_nonce, false);
			recv_ciph	= bt_jamrc4_helper_t::build_recv_ciph(itor_nonce, m_resp_nonce, false);
			if( !xmit_ciph || !recv_ciph ){
				nipmem_zdelete xmit_ciph;
				nipmem_zdelete recv_ciph;
				return bt_err_t(bt_err_t::ERROR, "unable to init xmit/recv_ciph");
			}
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




