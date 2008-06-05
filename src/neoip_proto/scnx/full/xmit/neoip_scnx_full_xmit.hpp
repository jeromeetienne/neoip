/*! \file
    \brief Declaration of \ref scnx_full_xmit_t

*/


#ifndef __NEOIP_SCNX_FULL_XMIT_HPP__ 
#define __NEOIP_SCNX_FULL_XMIT_HPP__ 
/* system include */
/* local include */
#include "neoip_scnx_err.hpp"
#include "neoip_pkt.hpp"
#include "neoip_skey_ciph.hpp"
#include "neoip_skey_auth.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class scnx_full_t;

/** \brief Class to handle the packet transmition for \ref scnx_full_t
 */
class scnx_full_xmit_t : NEOIP_COPY_CTOR_DENY {
private:	
	scnx_full_t *		scnx_full;	//!< backpointer on the \ref scnx_full_t

	/*************** cipher data	***************************************/
	skey_ciph_t		skey_ciph;	//!< context to encrypt data
	skey_ciph_iv_t		ciph_iv;	//!< the IV for to encrypt data

	/*************** auth data	***************************************/
	skey_auth_t		skey_auth;	//!< context to authenticate outgoing data
	datum_t			auth_key;	//!< the authentication key

	/*************** anti replay sequence number	***********************/
	uint32_t		seqnb;		//!< sequence number to ensure antireplay
public:
	/*************** ctor/dtor	***************************************/
	scnx_full_xmit_t(scnx_full_t *scnx_full, const skey_ciph_type_t &ciph_type
				, const skey_auth_type_t &auth_type, const datum_t &base_key )	throw();
	~scnx_full_xmit_t()	throw();
	
	/*************** query function	***************************************/
	uint32_t	get_seqnb()			const throw() { return seqnb;	}
	size_t		get_mtu_overhead()		const throw();

	/*************** Action Function	*******************************/
	scnx_err_t	pkt_from_upper( pkt_t &pkt )	throw();	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SCNX_FULL_XMIT_HPP__  */



