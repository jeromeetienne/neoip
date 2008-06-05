/*! \file
    \brief Header of the \ref tls_full_t

*/


#ifndef __NEOIP_BT_JAMMER_FULL_HPP__ 
#define __NEOIP_BT_JAMMER_FULL_HPP__ 
/* system include */
/* local include */
#include "neoip_tls_full_wikidbg.hpp"
#include "neoip_pkt.hpp"
#include "neoip_crypto_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	tls_privctx_t;

/** \brief This object contains the full needed for a scrambled connection
 */
class tls_full_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<tls_full_t, tls_full_wikidbg_init> {
private:
	tls_privctx_t *	m_tls_privctx;	//!< the tls_privctx_t
	pkt_t		m_upper_buf_u2l;
	pkt_t		m_upper_buf_l2u;
	pkt_t		m_lower_buf_u2l;
	pkt_t		m_lower_buf_l2u;

	/*************** read/write callback	*******************************/
	friend ssize_t	tls_full_c_read_cb(void *userptr, void *buf_ptr, size_t buf_len);
	friend ssize_t	tls_full_c_write_cb(void *userptr, const void *buf_ptr, size_t buf_len);
	ssize_t		cpp_read_cb(void *buf_ptr, size_t buf_len)		throw();
	ssize_t		cpp_write_cb(const void *buf_ptr, size_t buf_len)	throw();
public:
	/*************** ctor/dtor	***************************************/
	tls_full_t()	throw();
	~tls_full_t()	throw();
	
	/*************** Setup function	***************************************/
	crypto_err_t	start(tls_privctx_t * p_tls_privctx, const pkt_t &p_lower_buf_l2u) throw();
	
	/*************** Query function	***************************************/
	pkt_t &		lower_buf_u2l()	throw()	{ return m_lower_buf_u2l;	}
	pkt_t &		upper_buf_l2u()	throw()	{ return m_upper_buf_l2u;	}
	
	/*************** Action function	*******************************/
	crypto_err_t	notify_recved_data(const void *data_ptr, size_t data_len)	throw();
	crypto_err_t	notify_toxmit_data(const void *data_ptr, size_t data_len)	throw();
	crypto_err_t	notify_gracefull_shutdown()					throw();
	
	/*************** some alias to be more readable	***********************/
	pkt_t &		recv_buffer()	throw()	{ return upper_buf_l2u();	}
	pkt_t &		xmit_buffer()	throw()	{ return lower_buf_u2l();	}
	crypto_err_t	notify_recved_data(const pkt_t &pkt) 	throw()	{ return notify_recved_data(pkt.void_ptr(), pkt.length());	}
	crypto_err_t	notify_recved_data(const datum_t &datum)throw()	{ return notify_recved_data(datum.void_ptr(), datum.length());	}
	crypto_err_t	notify_toxmit_data(const pkt_t &pkt) 	throw()	{ return notify_toxmit_data(pkt.void_ptr(), pkt.length());	}
	crypto_err_t	notify_toxmit_data(const datum_t &datum)throw()	{ return notify_toxmit_data(datum.void_ptr(), datum.length());	}
	
	/*************** List of friend class	*******************************/
	friend class	tls_full_wikidbg_t;
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_BT_JAMMER_FULL_HPP__  */



