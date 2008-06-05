/*! \file
    \brief Header of the \ref nlay_err_t

*/


#ifndef __NEOIP_NLAY_ERR_HPP__ 
#define __NEOIP_NLAY_ERR_HPP__ 

/* system include */
/* local include */
#include "neoip_errtype.hpp"
#include "neoip_namespace.hpp"

// include for error convertion
#include "neoip_scnx_err.hpp"
#include "neoip_pktcomp_err.hpp"
#include "neoip_reachpeer_err.hpp"
#include "neoip_pktfrag_err.hpp"
#include "neoip_rdgram_err.hpp"
#include "neoip_simuwan_err.hpp"

NEOIP_NAMESPACE_BEGIN

NEOIP_ERRTYPE_DECLARATION_START(nlay_err_t)
NEOIP_ERRTYPE_DECLARATION_ITEM(nlay_err_t	, OK)
NEOIP_ERRTYPE_DECLARATION_ITEM(nlay_err_t	, BOGUS_PKT)
NEOIP_ERRTYPE_DECLARATION_ITEM(nlay_err_t	, NEGO_FAILED)
NEOIP_ERRTYPE_DECLARATION_ITEM(nlay_err_t	, FROM_IDLETIMEOUT)
NEOIP_ERRTYPE_DECLARATION_ITEM(nlay_err_t	, FROM_ORDGRAM)
NEOIP_ERRTYPE_DECLARATION_ITEM(nlay_err_t	, FROM_PKTCOMP)
NEOIP_ERRTYPE_DECLARATION_ITEM(nlay_err_t	, FROM_PKTFRAG)
NEOIP_ERRTYPE_DECLARATION_ITEM(nlay_err_t	, FROM_RDGRAM)
NEOIP_ERRTYPE_DECLARATION_ITEM(nlay_err_t	, FROM_REACHPEER)
NEOIP_ERRTYPE_DECLARATION_ITEM(nlay_err_t	, FROM_SCNX)
NEOIP_ERRTYPE_DECLARATION_ITEM(nlay_err_t	, FROM_SIMUWAN)
NEOIP_ERRTYPE_DECLARATION_ITEM_LAST(nlay_err_t)
	bool succeed()	const throw() { return get_value() == OK;	}
	bool failed()	const throw() { return !succeed();		}
NEOIP_ERRTYPE_DECLARATION_END(nlay_err_t)

// convert external errors into socket_err_t
nlay_err_t nlay_err_from_idletimeout(const std::string &str="")					throw();
nlay_err_t nlay_err_from_ordgram(const rdgram_err_t &rdgram_err, const std::string &prefix_str="")	throw();
nlay_err_t nlay_err_from_pktcomp(const pktcomp_err_t &pktcomp_err, const std::string &prefix_str="")	throw();
nlay_err_t nlay_err_from_pktfrag(const pktfrag_err_t &pktfrag_err, const std::string &prefix_str="")	throw();
nlay_err_t nlay_err_from_reachpeer(const reachpeer_err_t &reachpeer_err, const std::string &prefix_str="")	throw();
nlay_err_t nlay_err_from_rdgram(const rdgram_err_t &rdgram_err, const std::string &prefix_str="")	throw();
nlay_err_t nlay_err_from_scnx(const scnx_err_t &scnx_err, const std::string &prefix_str="")		throw();
nlay_err_t nlay_err_from_simuwan(const simuwan_err_t &simuwan_err, const std::string &prefix_str="")	throw();

NEOIP_NAMESPACE_END


#endif	/* __NEOIP_NLAY_ERR_HPP__  */



