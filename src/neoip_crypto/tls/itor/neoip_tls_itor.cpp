/*! \file
    \brief Header of the \ref tls_itor_t

*/

/* system include */
#include "gnutls/gnutls.h"
/* local include */
#include "neoip_tls_itor.hpp"
#include "neoip_tls_full.hpp"
#include "neoip_tls_privctx.hpp"
#include "neoip_crypto_err.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

// prototype for the C version of the read/write callback
ssize_t	tls_itor_c_read_cb(void *userptr, void *buf_ptr, size_t buf_len);
ssize_t	tls_itor_c_write_cb(void *userptr, const void *buf_ptr, size_t buf_len);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                               CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Default Constructor
 */
tls_itor_t::tls_itor_t()	throw()
{
	// zero some fields
	m_tls_privctx	= NULL;
	// init the static link http - to ease access while debugging - TODO to remove
	wikidbg_obj_add_static_page("/tls_itor_" + OSTREAMSTR(this));
}

/** \brief Destructor
 */
tls_itor_t::~tls_itor_t()	throw()
{
	// delete gnutls_sess if needed
	nipmem_zdelete	m_tls_privctx;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Set the profile for this object
 */
tls_itor_t &tls_itor_t::profile(const tls_profile_t &p_profile)	throw()
{
	// sanity check - the profile MUST be checked OK
	DBG_ASSERT( p_profile.check() == crypto_err_t::OK );	
	// copy the parameter
	this->m_profile	= p_profile;
	// return the object iself
	return *this;
}

/** \brief Start the action
 */
crypto_err_t tls_itor_t::start()		throw()
{
	crypto_err_t	crypto_err;
	
	// start the tls_privctx_t
	m_tls_privctx	= nipmem_new tls_privctx_t();
	crypto_err	= m_tls_privctx->start_client(m_profile);
	if( crypto_err.failed() )	return crypto_err;

	// set all the custom read/write function
	gnutls_session_t	gnutls_sess	= m_tls_privctx->session();
	gnutls_transport_set_ptr(gnutls_sess, (gnutls_transport_ptr_t) this);
	gnutls_transport_set_pull_function(gnutls_sess, tls_itor_c_read_cb);
	gnutls_transport_set_push_function(gnutls_sess, tls_itor_c_write_cb);
	// return no error
	return crypto_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief used by the caller to notify recved_pkt
 * 
 * @param tls_full_out	if returned as non-null, it contains a tls_full_t
 *			and the connection is considered established
 * 
 * @return a crypto_err_t. if crypto_err.failed() then the connection should be dropped
 */
crypto_err_t tls_itor_t::notify_recved_data(const pkt_t &recved_pkt
					, tls_full_t **tls_full_out)	throw()
{
	gnutls_session_t	gnutls_sess	= m_tls_privctx->session();
	// log to debug
	KLOG_DBG("enter recved_pkt.length()= " << recved_pkt.length() );
	// set tls_full_out to NULL by default
	*tls_full_out	= NULL;
	
	// put the recved_pkt into the m_recv_buffer
	m_recv_buffer.append(recved_pkt.to_datum(datum_t::NOCOPY));

	// call gnutls_handshake to start/go-on the handshake
	// - NOTE: this function will call the read/write callback
	int gnutls_err	= gnutls_handshake(gnutls_sess);
	// if it returns a fatal error, return it now
	if(gnutls_err != GNUTLS_E_AGAIN && gnutls_err != GNUTLS_E_INTERRUPTED && gnutls_err)
		return crypto_err_from_gnutls(gnutls_err);

	// if the gnutls_handshake is not yet completed, wait for more data and return ok
	if( gnutls_err ){
		// sanity check - at this point, gnutls_err MUST NOT be a fatal one
		DBG_ASSERT(gnutls_err == GNUTLS_E_AGAIN || gnutls_err == GNUTLS_E_INTERRUPTED);
		// return ok
		return crypto_err_t::OK;
	}
	
	// NOTE: at this point, the establishement is completed, create a tls_full_t
	crypto_err_t	crypto_err;
	tls_full_t *	tls_full;
	tls_full	= nipmem_new tls_full_t();
	crypto_err	= tls_full->start(m_tls_privctx, m_recv_buffer);
	// mark m_tls_privctx as unused as it is now as owned by tls_full_t
	m_tls_privctx	= NULL;
	// if tls_full_t creation failed, delete tls_full_t and return the error
	if( crypto_err.failed() ){
		nipmem_zdelete tls_full;
		return crypto_err;
	}

	// copy the just-built tls_full_t into tls_full_out
	*tls_full_out	= tls_full;
	
	// return no error
	return crypto_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			Handle read/write from gnutls
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief callback called by gnutls when willing to read
 * 
 * - just a trick of c/c++ interaction as libevent is in C
 */
ssize_t	tls_itor_c_read_cb(void *userptr, void *buf_ptr, size_t buf_len)
{
	tls_itor_t *	tls_itor	= (tls_itor_t *)userptr;
	return tls_itor->cpp_read_cb(buf_ptr, buf_len);
}

/** \brief callback called by gnutls when willing to write
 * 
 * - just a trick of c/c++ interaction as libevent is in C
 */
ssize_t	tls_itor_c_write_cb(void * userptr, const void *buf_ptr, size_t buf_len)
{
	tls_itor_t *	tls_itor	= (tls_itor_t *)userptr;
	return tls_itor->cpp_write_cb(buf_ptr, buf_len);
}

/** \brief Handle the c++ read for gnutls 
 */
ssize_t	tls_itor_t::cpp_read_cb(void *buf_ptr, size_t buf_len)		throw()
{
	// log to debug
	KLOG_DBG("enter buf_len=" << buf_len);
	// compute the read_len - min between data available and data wanted
	size_t	read_len	= std::min(m_recv_buffer.length(), buf_len);
	// copy read_len byte into buf_ptr
	memcpy( buf_ptr, m_recv_buffer.void_ptr(), read_len );
	// remove read_len byte from m_recv_buffer
	m_recv_buffer.head_free( read_len );
	KLOG_DBG("read_len=" << read_len);

	// if there are no data to read, return -1 with errno set to EAGAIN 
	if( read_len == 0 ){
		errno	= EAGAIN;
		return -1;
	}
	// return the read_len
	return read_len;
}

/** \brief Handle the c++ write for gnutls 
 */
ssize_t	tls_itor_t::cpp_write_cb(const void *buf_ptr, size_t buf_len)	throw()
{
	// log to debug
	KLOG_DBG("enter buf_len=" << buf_len);
	// put the whole buffer into m_xmit_buffer
	m_xmit_buffer.append(buf_ptr, buf_len);
	// return buf_len all the time - as all byte have been 'written'
	return buf_len;
}

NEOIP_NAMESPACE_END




