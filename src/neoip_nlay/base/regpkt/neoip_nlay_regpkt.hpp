/*! \file
    \brief Header of the \ref nlay_regpkt_t

- see \ref neoip_nlay_regpkt.cpp
*/


#ifndef __NEOIP_NLAY_REGPKT_HPP__ 
#define __NEOIP_NLAY_REGPKT_HPP__ 
/* system include */
#include <map>
/* local include */
#include "neoip_nlay_err.hpp"
#include "neoip_nlay_pkttype.hpp"
#include "neoip_pkt.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class nlay_full_api_t;

/** \brief to handle the regpkt in the nlay stack
 */
class nlay_regpkt_t : NEOIP_COPY_CTOR_ALLOW {
private:
	std::map<nlay_pkttype_t, nlay_full_api_t *>	handler_db;
	nlay_full_api_t	*				dfl_handler;
public:
	// ctor/dtor
	nlay_regpkt_t()		throw();
	~nlay_regpkt_t()	throw();


	void			register_handler(nlay_pkttype_t pkttype, nlay_full_api_t *callback)throw();
	nlay_full_api_t *	find(nlay_pkttype_t pkttype)			throw();	

	size_t			get_nb_handler()				const throw();
	
	void			set_dfl_handler(nlay_full_api_t *callback)	throw();
	nlay_full_api_t *	get_dfl_handler()				throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NLAY_REGPKT_HPP__  */



