/*! \file
    \brief Header of the \ref bt_jamrc4_full_t

*/

/* system include */
/* local include */
#include "neoip_bt_jamrc4_full.hpp"
#include "neoip_bt_jamrc4_helper.hpp"
#include "neoip_bt_id.hpp"
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
bt_jamrc4_full_t::bt_jamrc4_full_t()				throw()
{
	// zero some fields
	m_recv_ciph	= NULL;
	m_xmit_ciph	= NULL;
}

/** \brief Destructor
 */
bt_jamrc4_full_t::~bt_jamrc4_full_t()				throw()
{
	// delete m_recv_ciph if needed
	nipmem_zdelete m_recv_ciph;
	// delete m_xmit_ciph if needed
	nipmem_zdelete m_xmit_ciph;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the action
 * 
 * - m_init_recvbuf is the initial data recved during the connection establishement
 */
bt_err_t bt_jamrc4_full_t::start(skey_ciph_t *m_xmit_ciph, skey_ciph_t *m_recv_ciph
						, const pkt_t &p_init_recvbuf)	throw()
{
	// log to debug
	KLOG_DBG("enter init_recvbuf.length()=" << p_init_recvbuf.length());
	// sanity check - m_xmit_ciph/m_recv_ciph MUST be either both NULL or both non-NULL
	DBG_ASSERT( !m_xmit_ciph == !m_recv_ciph );
	// copy the parameters
	this->m_xmit_ciph	= m_xmit_ciph;
	this->m_recv_ciph	= m_recv_ciph;
	this->m_init_recvbuf	= p_init_recvbuf;
	// return no error
	return bt_err_t::OK;
}


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the received data
 */
bt_err_t	bt_jamrc4_full_t::notify_recved_data(const pkt_t &pkt_inp, pkt_t &pkt_out)throw()
{
	// log to debug
	KLOG_DBG("enter");
	// build the pkt_out first - as the crypto is done in place
	if( m_init_recvbuf.empty() ){
		// do a zerocopy assignment of pkt_inp data into pkt_out
		pkt_out.work_on_data_nocopy(pkt_inp.void_ptr(), pkt_inp.length());
	}else{
		// set pkt_out as concatenate of m_init_recvbuf and pkt_inp
		pkt_out		= m_init_recvbuf;
		pkt_out.append(pkt_inp.void_ptr(), pkt_inp.length());
		// zero m_init_recvbuf;
		m_init_recvbuf	= pkt_t();
	}
	
	// if m_xmit_ciph is NULL, return now
	if( !m_xmit_ciph )	return bt_err_t::OK;
	// encrypt pkt_out inplace
	skey_ciph_iv_t	ciph_iv( m_xmit_ciph );
	m_xmit_ciph->encipher(ciph_iv	, pkt_out.void_ptr(), pkt_out.length()
					, pkt_out.void_ptr(), pkt_out.length());
	// return no error
	return bt_err_t::OK;
}

/** \brief notify the data to xmit
 */
bt_err_t	bt_jamrc4_full_t::notify_toxmit_data(const pkt_t &pkt_inp, pkt_t &pkt_out)throw()
{
	// log to debug
	KLOG_DBG("enter");
	// - do a zerocopy assignment of pkt_inp data into pkt_out 
	pkt_out.work_on_data_nocopy(pkt_inp.void_ptr(), pkt_inp.length());

	// if m_recv_ciph is NULL, return now
	if( !m_recv_ciph )	return bt_err_t::OK;
	// decrypt pkt_out inplace
	skey_ciph_iv_t	ciph_iv( m_recv_ciph );
	m_recv_ciph->decipher(ciph_iv	, pkt_out.void_ptr(), pkt_out.length()
					, pkt_out.void_ptr(), pkt_out.length());
	// return no error
	return bt_err_t::OK;
}

NEOIP_NAMESPACE_END




