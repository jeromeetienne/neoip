/*! \file
    \brief Declaration of \ref scnx_full_recv_t

*/


#ifndef __NEOIP_SCNX_FULL_RECV_HPP__ 
#define __NEOIP_SCNX_FULL_RECV_HPP__ 
/* system include */
/* local include */
#include "neoip_scnx_err.hpp"
#include "neoip_pkt.hpp"
#include "neoip_skey_ciph.hpp"
#include "neoip_skey_auth.hpp"
#include "neoip_slidwin.hpp"
#include "neoip_wai.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class scnx_full_t;

/** \brief Class to handle the packet reception for \ref scnx_full_t
 */
class scnx_full_recv_t : NEOIP_COPY_CTOR_DENY {
public:	////////////////// declaration internal constant //////////////////////
	static const size_t	AREPLAY_WIN_SIZE_DFL;
private:
	scnx_full_t *	scnx_full;	//!< backpointer on the scnx_full_t

	/*************** cipher data	***************************************/
	skey_ciph_t	skey_ciph;	//!< context to decrypt data
	skey_ciph_iv_t	ciph_iv;	//!< the IV for to decrypt data

	/*************** auth data	***************************************/
	skey_auth_t	skey_auth;	//!< context to authenticate incoming data
	datum_t		auth_key;	//!< the authentication key

	/*************** anti replay window	*******************************/
	slidwin_t<bool, uint32_t> *	areplay_window;	//!< context to determine if a pkt has been
							//!< already received or not.
public:
	/*************** ctor/dtor	***************************************/
	scnx_full_recv_t(scnx_full_t *scnx_full, const skey_ciph_type_t &ciph_type
					, const skey_auth_type_t &auth_type
					, const datum_t &base_key )		throw();
	~scnx_full_recv_t()		throw();

	/*************** Action function	*******************************/
	scnx_err_t	pkt_from_lower( pkt_t &pkt )	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SCNX_FULL_RECV_HPP__  */



