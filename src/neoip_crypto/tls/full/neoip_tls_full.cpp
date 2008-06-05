/*! \file
    \brief definition of the \ref tls_full_t
    
\par TODO fix the massive padding
- gnutls send a LOT of padding each time data are sent. this wastes bandwidth
  - later version of gnutls got a specific function for disabeling it
    - gnutls_record_disable_padding()
  - from the GnuTLS 2.0.3 release note: "** Added gnutls_record_disable_padding() to 
    allow servers talking to buggy clients that complain if the TLS 1.0 record 
    protocol padding is used."
  - this seems more like a wordaround than a real function
- maybe it is possible to tune at the session level... i dunno

*/

/* system include */
#include "gnutls/gnutls.h"
/* local include */
#include "neoip_tls_full.hpp"
#include "neoip_tls_privctx.hpp"
#include "neoip_crypto_err.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"

NEOIP_NAMESPACE_BEGIN

// prototype for the C version of the read/write callback
ssize_t	tls_full_c_read_cb(void *userptr, void *buf_ptr, size_t buf_len);
ssize_t	tls_full_c_write_cb(void *userptr, const void *buf_ptr, size_t buf_len);

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                               CTOR/DTOR
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Default Constructor
 */
tls_full_t::tls_full_t()				throw()
{
	// zero some fields
	m_tls_privctx	= NULL;
	// init the static link http - to ease access while debugging - TODO to remove
	wikidbg_obj_add_static_page("/tls_full_" + OSTREAMSTR(this));
}

/** \brief Destructor
 */
tls_full_t::~tls_full_t()				throw()
{
	// delete gnutls_sess if needed
	nipmem_zdelete	m_tls_privctx;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                     Setup function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the action
 */
crypto_err_t	tls_full_t::start(tls_privctx_t * p_tls_privctx
					, const pkt_t &p_lower_buf_l2u)	throw()
{
	// log to debug
	KLOG_DBG("enter");
	// copy the parameters
	this->m_tls_privctx	= p_tls_privctx;
	this->m_lower_buf_l2u	= p_lower_buf_l2u;

	// set all the custom read/write function
	gnutls_session_t	gnutls_sess	= m_tls_privctx->session();
	gnutls_transport_set_ptr(gnutls_sess, (gnutls_transport_ptr_t) this);
	gnutls_transport_set_pull_function(gnutls_sess, tls_full_c_read_cb);
	gnutls_transport_set_push_function(gnutls_sess, tls_full_c_write_cb);
	// return no error
	return crypto_err_t::OK;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//			action function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief notify the received data
 */
crypto_err_t	tls_full_t::notify_recved_data(const void *data_ptr, size_t data_len)	throw()
{
	gnutls_session_t	gnutls_sess	= m_tls_privctx->session();
	// log to debug
	KLOG_DBG("enter data_len= " << data_len);
	
	// put the data into m_lower_buf_l2u
	m_lower_buf_l2u.append(data_ptr, data_len);
	
	// call gnutls recv function
	char		buf_tmp[64*1024];
	ssize_t		read_len;
	read_len	= gnutls_record_recv(gnutls_sess, buf_tmp, sizeof(buf_tmp));
	// if the read can't be completed, return ok now
	if(read_len == GNUTLS_E_AGAIN || read_len == GNUTLS_E_INTERRUPTED)
		return crypto_err_t::OK;
	// if a fatal error occurs, return it now
	if(read_len < 0 )	return crypto_err_from_gnutls(read_len);

	// put the read data into m_upper_buf_u2l
	m_upper_buf_l2u.append(buf_tmp, read_len);
	
	// return no error
	return crypto_err_t::OK;
}

/** \brief notify the data to xmit
 */
crypto_err_t	tls_full_t::notify_toxmit_data(const void *data_ptr, size_t data_len)	throw()
{
	gnutls_session_t	gnutls_sess	= m_tls_privctx->session();
	// log to debug
	KLOG_DBG("enter data_len= " << data_len);
	// put the data into m_upper_buf_u2l
	m_upper_buf_u2l.append(data_ptr, data_len);
	// call gnutls send function on the whole m_upper_buf_u2l
	ssize_t		written_len;
	written_len	= gnutls_record_send(gnutls_sess, m_upper_buf_u2l.void_ptr(), m_upper_buf_u2l.length());
	// if the read can't be completed, return ok now
	if(written_len == GNUTLS_E_AGAIN || written_len == GNUTLS_E_INTERRUPTED)
		return crypto_err_t::OK;
	// if a fatal error occurs, return it now
	if(written_len < 0)	return crypto_err_from_gnutls(written_len);

	// remove written_len byte from m_upper_buf_u2l
	m_upper_buf_u2l.head_free( written_len );

	// return no error
	return crypto_err_t::OK;
}

/** \brief xmit a gracefull shutdown in the xmit_buffer
 * 
 * - WARNING: after the tls_full_t is unusable execpt for the xmit_buffer
 */
crypto_err_t	tls_full_t::notify_gracefull_shutdown()				throw()
{
	gnutls_session_t	gnutls_sess	= m_tls_privctx->session();
	int			gnutls_err;
	// send a bye
	gnutls_err	= gnutls_bye(gnutls_sess, GNUTLS_SHUT_RDWR);
	if( gnutls_err < 0 )	return crypto_err_from_gnutls(gnutls_err);
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
ssize_t	tls_full_c_read_cb(void *userptr, void *buf_ptr, size_t buf_len)
{
	tls_full_t *	tls_full	= (tls_full_t *)userptr;
	return tls_full->cpp_read_cb(buf_ptr, buf_len);
}

/** \brief callback called by gnutls when willing to write
 * 
 * - just a trick of c/c++ interaction as libevent is in C
 */
ssize_t	tls_full_c_write_cb(void * userptr, const void *buf_ptr, size_t buf_len)
{
	tls_full_t *	tls_full	= (tls_full_t *)userptr;
	return tls_full->cpp_write_cb(buf_ptr, buf_len);
}

/** \brief Handle the c++ read for gnutls 
 */
ssize_t	tls_full_t::cpp_read_cb(void *buf_ptr, size_t buf_len)		throw()
{
	// log to debug
	KLOG_DBG("enter buf_len=" << buf_len);
	// compute the read_len - min between data available and data wanted
	size_t	read_len	= std::min(m_lower_buf_l2u.length(), buf_len);
	// copy read_len byte into buf_ptr
	memcpy( buf_ptr, m_lower_buf_l2u.void_ptr(), read_len );
	// remove read_len byte from m_lower_buf_l2u
	m_lower_buf_l2u.head_free( read_len );

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
ssize_t	tls_full_t::cpp_write_cb(const void *buf_ptr, size_t buf_len)	throw()
{
	// log to debug
	KLOG_DBG("enter buf_len=" << buf_len);
	// put the whole buffer into m_lower_buf_u2l
	m_lower_buf_u2l.append(buf_ptr, buf_len);
	// return buf_len all the time - as all byte have been 'written'
	return buf_len;
}

NEOIP_NAMESPACE_END




