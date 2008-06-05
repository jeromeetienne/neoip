/*! \file
    \brief Declaration of \ref scnx_full_rkey_t

*/


#ifndef __NEOIP_SCNX_FULL_RKEY_HPP__ 
#define __NEOIP_SCNX_FULL_RKEY_HPP__ 
/* system include */
/* local include */
#include "neoip_scnx_err.hpp"
#include "neoip_pkt.hpp"
#include "neoip_scnx_nonceid.hpp"
#include "neoip_timeout.hpp"
#include "neoip_delaygen.hpp"
#include "neoip_zerotimer.hpp"
#include "neoip_namespace.hpp"


NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class scnx_full_t;

/** \brief Class to handle the packet reception for \ref scnx_full_t
 */
class scnx_full_rkey_t : public timeout_cb_t {
private:
	scnx_full_t *		scnx_full;		//!< backpointer on the \ref scnx_full_t
	scnx_nonceid_t		local_nonce;		//!< the local_nonce used in REKEY_REQUEST during 
							//!< a rekeying as itor. if local_nonce.is_null() is
							//!< true, no rekeying is in progress.
	scnx_nonceid_t		last_remote_nonce;	//!< the remote_nonce of the last REKEY_REPLY
							//!< (usefull for REKEY_REPLY dropped by the network)

	/*************** Packet rxmit	***************************************/
	delaygen_t	rxmit_delaygen;		//!< the delaygen_t for the packet rxmit
	timeout_t	rxmit_timeout;		//!< retransmition timer to rxmit REKEY_REQUEST
	bool		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)		throw();

	/*************** Packet Building	*******************************/
	pkt_t		build_rekey_request()					throw();
	pkt_t		build_rekey_reply(const scnx_nonceid_t &nonce_i)	throw();
public:
	scnx_full_rkey_t(scnx_full_t *scnx_full)	throw();
	~scnx_full_rkey_t()				throw();
	
	bool		is_inprogress()			const throw();
	void		initiate()			throw();
	scnx_err_t	recv_rekey_request(pkt_t &pkt)	throw();
	scnx_err_t	recv_rekey_reply(pkt_t &pkt)	throw();
};	

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_SCNX_FULL_RKEY_HPP__  */



