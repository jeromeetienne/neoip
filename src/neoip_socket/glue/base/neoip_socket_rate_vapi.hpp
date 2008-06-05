/*! \file
    \brief Declaration of the socket_rate_vapi_t

\brief Brief Description
\ref socket_rate_vapi_t describes the generic API to access rate scheduling
within the socket. socket_rate_vapi_t is used as a central point for every
socket type using rate_sched_t/rate_limit_t, aka it is not limited to 
the socket_full_t API, it is used in tcp_full_t and any other socket type
implementing the rate scheduling.

- TODO to port elsewhere
  - in the neoip_socket/glue

*/


#ifndef __NEOIP_SOCKET_RATE_VAPI_HPP__ 
#define __NEOIP_SOCKET_RATE_VAPI_HPP__ 

/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	rate_limit_arg_t;
class	rate_limit_tmp_t;
class	rate_limit_t;

/** \brief the callback class for socket_rate_vapi_t
 */
class socket_rate_vapi_t {
public:
	/*************** core function	***************************************/
	virtual void			xmit_limit(const rate_limit_arg_t &limit_arg)	throw()		= 0;
	virtual rate_limit_t &		xmit_limit()					throw()		= 0;
	virtual const rate_limit_t &	xmit_limit()					const throw()	= 0;
	virtual bool			xmit_limit_is_set()				const throw()	= 0;
	virtual void			recv_limit(const rate_limit_arg_t &limit_arg)	throw()		= 0;
	virtual rate_limit_t &		recv_limit()					throw()		= 0;
	virtual const rate_limit_t &	recv_limit()					const throw()	= 0;
	virtual bool			recv_limit_is_set()				const throw()	= 0;

#if 0	// TODO issue they requires to include neoip_rate_limit.hpp due to the inline stuff
	// - what to do ?
	// - is it possible to do with without the #include ?
	// - should i #include it anyway ? with the compilation cost ?
	// - should i remove the possibility to do the short-cut function ?
	/*************** short-cut functions	*******************************/
	double			xmit_average_rate()	const throw()	{ return xmit_limit().average_rate();	}
	double			xmit_absrate_max()	const throw()	{ return xmit_limit().absrate_max();	}
	void			xmit_absrate_max(double value)	throw() { xmit_limit().absrate_max(value);	}
	const rate_prec_t &	xmit_maxiprec()		const throw()	{ return xmit_limit().maxi_prec();	}
	void			xmit_maxiprec(const rate_prec_t &value)	throw() { xmit_limit().maxi_prec();	}
	const rate_prec_t &	xmit_usedprec()		const throw()	{ return xmit_limit.user_prec();	}
	void			xmit_request(size_t request_len, rate_limit_tmp_t &limit_tmp)	const throw()
									{ return xmit_limit().data_request(request_len, limit_tmp);	}
	size_t			xmit_notifiy(size_t notified_len, const rate_limit_tmp_t &limit_tmp)	throw()
									{ return xmit_limit().data_notify(notified_len, limit_tmp);	}
#endif
	// virtual destructor
	virtual ~socket_rate_vapi_t() {};
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SOCKET_RATE_VAPI_HPP__  */



