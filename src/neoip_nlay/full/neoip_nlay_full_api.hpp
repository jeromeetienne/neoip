/*! \file
    \brief Header of the \ref nlay_full_api_t
*/


#ifndef __NEOIP_NLAY_FULL_API_HPP__ 
#define __NEOIP_NLAY_FULL_API_HPP__ 
/* system include */
/* local include */
#include "neoip_nlay_err.hpp"
#include "neoip_nlay_nego.hpp"
#include "neoip_nlay_regpkt.hpp"
#include "neoip_pkt.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class nlay_full_t;
class nlay_regpkt_t;

/** \brief The virtual API for full connections
 * 
 * - this object provides a virtual API for nlay_full_api_t
 * - additionnally it stores various variable about the position of the layer 
 *   in the stack...
 *   - to store those information here, doesnt seems clean
 *   - TODO is there a cleaner way to do it ?
 * 
 */
class nlay_full_api_t {
private:
	nlay_full_t *		nlay_full;
	nlay_regpkt_t *		regpkt_upper;
	nlay_full_api_t *	full_api_lower;
public:
	//! ctor
	nlay_full_api_t()	throw() {
		nlay_full	= NULL;
		regpkt_upper	= NULL;
		full_api_lower	= NULL;
	}

	//! return the nlay_full_t to which this brick is attached
	nlay_full_t *		get_nlay_full()		const throw() { return nlay_full;	}

	/** \brief return the nlay_regpkt_t to which direct the packets going up out of this brick
	 * - if == NULL, the packet is sent to the nlay's caller
	 */
	nlay_regpkt_t *		get_regpkt_upper()	const throw() { return regpkt_upper;	}

	//! return the nlay_full_api_t to which direct the packets going down out of this brick
	nlay_full_api_t *	get_full_api_lower()	const throw() { return full_api_lower;	}
	
	//! register parameters which determine the position of this brick in the stack
	void	nlay_full_api_register( nlay_full_t *nlay_full
					, nlay_regpkt_t *regpkt_upper, nlay_regpkt_t *regpkt_lower
					, nlay_full_api_t * full_api_lower)	throw(){
		this->nlay_full		= nlay_full;
		this->regpkt_upper 	= regpkt_upper;
		this->full_api_lower	= full_api_lower;
		// register the pkttype's of this brick
		register_handler(regpkt_lower);
	}

public:
	/** \brief register the pkttype of this brick
	 */
	virtual void		register_handler(nlay_regpkt_t *regpkt_lower) throw()	{}
	//! Set parameters in the brick
	virtual nlay_err_t	set_parameter(const nlay_profile_t *nlay_profile
					, const nlay_nego_result_t *nego_result) throw()
								{ return nlay_err_t::OK;	}
	//! Start the operation for this brick
	virtual nlay_err_t	start()				throw()	{ return nlay_err_t::OK;	}
	//! Start the closure for this brick
	virtual void		start_closure()			throw()	{ }
	//! handle packet from the upper network layer for full connection
	virtual nlay_err_t	pkt_from_upper(pkt_t &pkt)	throw()	{ return nlay_err_t::OK;	}
	//! handle packet from the lower network layer for full connection
	virtual nlay_err_t	pkt_from_lower(pkt_t &pkt)	throw()	{ return nlay_err_t::OK;	}
	//! return the space overhead due to this brick in the mtu
	virtual size_t		get_mtu_overhead()	  const throw() { return 0;			}
	//! set the outter mtu of this brick
	virtual nlay_err_t	set_outter_mtu(size_t outter_mtu)throw(){ return nlay_err_t::OK;	}

	//! autodelete of the object constructed by created()
	virtual void		destroy()		throw()	= 0;

	// reliability function
	virtual nlay_err_t	recv_max_len_set(size_t recv_max_len)		throw()
			{ DBG_ASSERT(0); return nlay_err_t::OK;	} // MUST NOT be called if not inherited
	virtual	size_t		recv_max_len_get()				const throw()
			{ DBG_ASSERT(0); return 0;		} // MUST NOT be called if not inherited
	virtual nlay_err_t	maysend_set_threshold(size_t maysend_threshold)	throw()
			{ DBG_ASSERT(0); return nlay_err_t::OK;	} // MUST NOT be called if not inherited
	virtual size_t		maysend_get_threshold()				const throw()
			{ DBG_ASSERT(0); return 0;		} // MUST NOT be called if not inherited
	virtual bool		maysend_is_set()				const throw()
			{ DBG_ASSERT(0); return false;		} // MUST NOT be called if not inherited
	virtual nlay_err_t	sendbuf_set_max_len(size_t sendbuf_max_len)	throw()
			{ DBG_ASSERT(0); return nlay_err_t::OK;	} // MUST NOT be called if not inherited
	virtual size_t		sendbuf_get_max_len()				const throw()
			{ DBG_ASSERT(0); return 0;		} // MUST NOT be called if not inherited
	virtual size_t		sendbuf_get_used_len()				const throw()
			{ DBG_ASSERT(0); return 0;		} // MUST NOT be called if not inherited
	virtual size_t		sendbuf_get_free_len()				const throw()
			{ DBG_ASSERT(0); return 0;		} // MUST NOT be called if not inherited
	virtual bool		sendbuf_is_limited()				const throw()
			{ DBG_ASSERT(0); return false;		} // MUST NOT be called if not inherited
	
	//! virtual destructor
	virtual ~nlay_full_api_t() {};
};

//! all the object inheriting from nlay_full_api_t MUST define this
// - it cant be declared in nlay_full_api_t directly as create require to return an actual classname
//   and not only nlay_full_api_t
#define NEOIP_NLAY_FULL_API_CTORDTOR(classname)							\
	static classname * create() throw()	{ return nipmem_new classname();	}	\
	void		destroy() throw()	{ nipmem_delete this;			}

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NLAY_FULL_API_HPP__  */



