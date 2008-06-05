/*! \file
    \brief Header of the \ref pktcomp_t

- see \ref neoip_pktcomp.cpp
*/


#ifndef __NEOIP_PKTCOMP_HPP__ 
#define __NEOIP_PKTCOMP_HPP__ 
/* system include */
/* local include */
#include "neoip_pktcomp_err.hpp"
#include "neoip_pktcomp_profile.hpp"
#include "neoip_pkt.hpp"
#include "neoip_compress.hpp"
#include "neoip_timeout.hpp"
#include "neoip_timer_expboff.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN


/** \brief A layer to compress packet
 */
class pktcomp_t : NEOIP_COPY_CTOR_ALLOW, private timeout_cb_t {
public:	/////////////////// declaration internal constant ////////////////////
	static const size_t	MAX_UNCOMPRESSED_LEN_DFL;
	static const size_t	NOCOMP_NB_SUCC_MAX_DFL;
	static const delay_t	NOCOMP_DELAY_MIN_DFL;	
	static const delay_t	NOCOMP_DELAY_MAX_DFL;	
	
private:
	compress_t		compress_ctx;
	
	size_t			max_uncompress_len;
	
	// to handle the period during which no compression is attempted
	size_t			nocomp_cur_nb_succ;	//!< number of failed compression in a raw
	size_t			nocomp_max_nb_succ;	//!< number of failed compression in a raw
	timeout_t		nocomp_timeout;		//!< nocompression period timeout
	timer_expboff_t	nocomp_timer_policy;	//!< the timer policy for the retransmission
	bool			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();	
	void			nocomp_pkt_cant_compress()		throw();
	void			nocomp_pkt_compressed()			throw();
	bool			nocomp_in_progress()			const throw();
	
	size_t		cpu_max_compressed_len(size_t uncompressed_len)		throw();
	pktcomp_err_t	recv_compressed_pkt(pkt_t &pkt)				throw(serial_except_t);
public:
	// ctor/dtor
	pktcomp_t()						throw();
	~pktcomp_t()						throw();
	pktcomp_t(const compress_type_t &compress_type)		throw();
	
	bool		is_null() const throw()	{ return compress_ctx.is_null();	}
	
	pktcomp_err_t	set_from_profile(const pktcomp_profile_t &profile)	throw();


	pktcomp_err_t	pkt_from_upper(pkt_t &pkt)		throw();
	pktcomp_err_t	pkt_from_lower(pkt_t &pkt)		throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_PKTCOMP_HPP__  */



