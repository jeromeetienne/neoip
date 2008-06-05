/*! \file
    \brief Definition of the \ref nlay_full_t specific to the stack

*/


/* system include */
/* local include */
#include "neoip_nlay_full.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_log.hpp"

// include all the nlay bricks
#include "neoip_nlay_closure.hpp"
#include "neoip_nlay_dataize.hpp"
#include "neoip_nlay_idletimeout.hpp"
#include "neoip_nlay_ordgram.hpp"
#include "neoip_nlay_pktcomp.hpp"
#include "neoip_nlay_pktfrag.hpp"
#include "neoip_nlay_rate_limit.hpp"
#include "neoip_nlay_reachpeer.hpp"
#include "neoip_nlay_rdgram.hpp"
#include "neoip_nlay_scnx_full.hpp"
#include "neoip_nlay_simuwan.hpp"


NEOIP_NAMESPACE_BEGIN

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        stack construction
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief build the stack when the outter nlay_type_t is DGRAM
 * 
 * - this function setup uppest_full_api and lowest_regpkt
 */
void	nlay_full_t::stack_build_outter_dgram(scnx_full_t *scnx_full)		throw()
{
	nlay_dataize_t *	nlay_dataize	= NULL;
	nlay_idletimeout_t *	nlay_idletimeout= NULL;
	nlay_ordgram_t *	nlay_ordgram	= NULL;
	nlay_pktcomp_t *	nlay_pktcomp	= NULL;
	nlay_pktfrag_t *	nlay_pktfrag	= NULL;
	nlay_rdgram_t *		nlay_rdgram	= NULL;
	nlay_rate_limit_t *	nlay_rate_limit	= NULL;
	nlay_reachpeer_t *	nlay_reachpeer	= NULL;
	//nlay_simuwan_t *	nlay_simuwan	= NULL;
	nlay_err_t		nlay_err;
	
	// sanity check - the direct pointer to nlay brick MUST be NULL
	DBG_ASSERT( nlay_closure	== NULL );
	DBG_ASSERT( nlay_scnx_full	== NULL );
	DBG_ASSERT( nlay_reliability	== NULL );
	
	// sanity check - inner_type and outter_type MUST be set
	DBG_ASSERT( !inner_type.is_null() );
	DBG_ASSERT( !outter_type.is_null() );
	// sanity check - here the outter_type MUST be nlay_type_t::DGRAM
	DBG_ASSERT( outter_type.is_dgram() );
	// NOTE: but the inner_type may be any of the 4

// build the regpkt vector for the stack
	regpkt_db.push_back(nlay_regpkt_t());
	regpkt_db.push_back(nlay_regpkt_t());
	regpkt_db.push_back(nlay_regpkt_t());
	regpkt_db.push_back(nlay_regpkt_t());
	regpkt_db.push_back(nlay_regpkt_t());
	regpkt_db.push_back(nlay_regpkt_t());
	// setup some alias to ease the readability
	nlay_regpkt_t &	regpkt_rate_limit	= regpkt_db[0];
	nlay_regpkt_t &	regpkt_scnx		= regpkt_db[1];
	nlay_regpkt_t &	regpkt_reachpeer	= regpkt_db[2];
	nlay_regpkt_t &	regpkt_fork		= regpkt_db[3];
	nlay_regpkt_t &	regpkt_idletimeout	= regpkt_db[4];
	nlay_regpkt_t &	regpkt_void		= regpkt_db[5];	// NOTE: nothing is registered in
								// regpkt_void. it is only use as sanity
								// check

// init common to all inner_type
	// init nlay_dataize
	nlay_closure	= nlay_closure_t::create();
	full_api_db.push_back(nlay_closure);
	// init nlay_idletimeout
	nlay_idletimeout= nlay_idletimeout_t::create();
	full_api_db.push_back(nlay_idletimeout);
	// init nlay_pktfrag_post
	nlay_pktfrag	= nlay_pktfrag_t::create();
	full_api_db.push_back(nlay_pktfrag);
	// init nlay_pktcomp
	nlay_pktcomp	= nlay_pktcomp_t::create();
	full_api_db.push_back(nlay_pktcomp);
	// init nlay_rate_limit
	nlay_rate_limit	= nlay_rate_limit_t::create();
	full_api_db.push_back(nlay_rate_limit);
	// init nlay_reachpeer
	nlay_reachpeer	= nlay_reachpeer_t::create();
	full_api_db.push_back(nlay_reachpeer);
	nlay_err	= nlay_reachpeer->set_reliable_outter_cnx( outter_type.is_reliable() );
	DBG_ASSERT( nlay_err.succeed() );	// TODO poor error management (anyway it cant failed)
	// init nlay_scnx_full
	nlay_scnx_full	= nlay_scnx_full_t::create();
	full_api_db.push_back(nlay_scnx_full);
	nlay_err	= nlay_scnx_full->set_scnx_full(scnx_full);
	DBG_ASSERT( nlay_err.succeed() );	// TODO poor error management (anyway it cant failed)

	// nlay_for_data is just an alias for the nlay_full_api_t which handle the data (e.g. dataize
	// , rdgram, ordgram etc...)
	// - TODO poor name. rename it
	nlay_full_api_t *	nlay_for_data	= NULL;
	
	// setup the lowest_regpkt and uppest_full_api
	lowest_regpkt	= &regpkt_rate_limit;
	uppest_full_api = nlay_idletimeout;
	
/* NOTE:
 * - TODO the dgram inner_type need another pktfrag on top of nlay_for_data to support 
 *   datagram larger than the connection MTU
 *   - if this pktfrag is on top of rdm or seqpacket, no timeout for the fragment as the fragment
 *     can't be lost
 *     - what about the pool size ? is it limited ? or not ?
 *     - if it is limited, what happend when it is full ? 
 *       - the fragment it discarded ? impossible as rdgram/ordgram are supposed to ensure reliability
 *     - so it can't be limited ? what about the DOS is the other peer start to send large dgram ?
 *   - if the inner type is DGRAM, timeout and pool size are limited
 * - TODO what about the bstream2dgram on top of ordgram when inner_type is STREAM ?
 *   - how to handle the buffering ?
 */

// init specific to inner_type
	switch(inner_type.get_value()){
	case nlay_type_t::DGRAM:	nlay_dataize	= nlay_dataize_t::create();
					nlay_for_data	= nlay_dataize;
					full_api_db.push_back(nlay_for_data);
					break;
	case nlay_type_t::RDM:		nlay_rdgram	= nlay_rdgram_t::create();
					nlay_for_data	= nlay_rdgram;
					nlay_reliability= nlay_rdgram;
					full_api_db.push_back(nlay_for_data);
					break;
	case nlay_type_t::SEQPACKET:	nlay_ordgram	= nlay_ordgram_t::create();
					nlay_for_data	= nlay_ordgram;
					nlay_reliability= nlay_rdgram;
					full_api_db.push_back(nlay_for_data);
					break;
	case nlay_type_t::STREAM:	// TMP: currently inner_type cant be STREAM - i need to code bstream2dgram
					DBG_ASSERT(0);
					nlay_ordgram	= nlay_ordgram_t::create();
					nlay_for_data	= nlay_ordgram;
					full_api_db.push_back(nlay_for_data);
					break;
	default:	DBG_ASSERT(0);
	}

// registering common to all inner_type
	nlay_idletimeout->nlay_full_api_register(this, NULL, &regpkt_idletimeout, nlay_for_data);
	nlay_closure->nlay_full_api_register(this, &regpkt_void, &regpkt_fork, nlay_pktfrag);
	nlay_pktfrag->nlay_full_api_register(this, &regpkt_fork, &regpkt_fork, nlay_scnx_full);
	nlay_pktcomp->nlay_full_api_register(this, &regpkt_fork, &regpkt_fork, nlay_pktfrag);
	nlay_reachpeer->nlay_full_api_register(this, &regpkt_fork, &regpkt_reachpeer, nlay_pktfrag);
	nlay_scnx_full->nlay_full_api_register(this, &regpkt_reachpeer, &regpkt_scnx, nlay_rate_limit);
	nlay_rate_limit->nlay_full_api_register(this, &regpkt_scnx, &regpkt_rate_limit, NULL);

// registering specific to inner_type
	switch(inner_type.get_value()){
	case nlay_type_t::DGRAM:
			nlay_dataize->nlay_full_api_register(this, &regpkt_idletimeout, &regpkt_fork
									, nlay_pktcomp);
			break;
	case nlay_type_t::RDM:
			DBG_ASSERT(!nlay_ordgram && nlay_rdgram);
			nlay_rdgram->nlay_full_api_register(this, &regpkt_idletimeout, &regpkt_fork
									, nlay_pktcomp);
			break;
	case nlay_type_t::SEQPACKET:
			DBG_ASSERT(nlay_ordgram && !nlay_rdgram);
			nlay_ordgram->nlay_full_api_register(this, &regpkt_idletimeout, &regpkt_fork
									, nlay_pktcomp);
			break;
	case nlay_type_t::STREAM:
			// TMP: currently inner_type cant be STREAM - i need to code bstream2dgram
			DBG_ASSERT(0);
			nlay_ordgram->nlay_full_api_register(this, &regpkt_idletimeout, &regpkt_fork
									, nlay_pktcomp);
			break;
	default:	DBG_ASSERT(0);
	}
	
	// sanity check on the regpkt stuff
	DBG_ASSERT(regpkt_idletimeout.get_nb_handler() == 1 && regpkt_idletimeout.get_dfl_handler() );
	DBG_ASSERT(regpkt_fork.get_nb_handler() > 0 && !regpkt_fork.get_dfl_handler());
	DBG_ASSERT(regpkt_reachpeer.get_nb_handler() > 1 && regpkt_reachpeer.get_dfl_handler());
	DBG_ASSERT(regpkt_scnx.get_nb_handler() == 1 && regpkt_scnx.get_dfl_handler());
	DBG_ASSERT(regpkt_rate_limit.get_nb_handler() == 1 && regpkt_rate_limit.get_dfl_handler());
	DBG_ASSERT(regpkt_void.get_nb_handler() == 0);
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                       set_parameter
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////


/** \brief Set the parameter of each brick in the whole stack
 */
nlay_err_t	nlay_full_t::stack_set_parameter(const nlay_profile_t *nlay_profile
					, const nlay_nego_result_t *nego_result)	throw()
{
	std::list<nlay_full_api_t *>::iterator	iter;
	nlay_err_t				nlay_err;
	// start the whole stack
	for( iter = full_api_db.begin(); iter != full_api_db.end(); iter++ ){
		nlay_full_api_t	*	full_api = *iter;
		// set the parameter
		nlay_err = full_api->set_parameter(nlay_profile, nego_result);
		// if it failed, return the error
		if( nlay_err.failed() )	return nlay_err;
	}
	// as everything went ok, return no error
	return nlay_err_t::OK;		
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                      start/destroy
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Start the whole stack
 * 
 * - start the bricks in any order
 * - so it the brick's start() MUST NOT do any event/packet *DURING* the start.
 *   - aka at least wait the next loop round (via timer or other)
 */
nlay_err_t	nlay_full_t::stack_start()	throw()
{
	std::list<nlay_full_api_t *>::iterator	iter;
	nlay_err_t	nlay_err;
	// loop over the whole stack
	for( iter = full_api_db.begin(); iter != full_api_db.end(); iter++ ){
		nlay_full_api_t	*	full_api = *iter;
		// start this layer
		nlay_err = full_api->start();
		// in case of error, destroy the whole stack and return
		// - thus a just started brick wont try to use an unproperly started nlay_full_t
		if( nlay_err.failed() ){
			stack_destroy();
			return nlay_err;
		}
	}
	// as everything went ok, return no error
	return nlay_err_t::OK;		
}		


/** \brief Destroy the whole stack
 */
void	nlay_full_t::stack_destroy()	throw()
{
	// delete the whole stack
	while( !full_api_db.empty() ){
		nlay_full_api_t	*full_api = full_api_db.front();
		// delete the object
		full_api->destroy();
		// remove it from the list
		full_api_db.remove(full_api);
	}

	// zero some nlay_full_api_t which need direct access
	nlay_closure	= NULL;
	nlay_scnx_full	= NULL;	
	nlay_reliability= NULL;
}

/** \brief Start the closure in the whole stack
 * 
 * - start the closure for the bricks in any order
 * - so it the brick's start_closure() MUST NOT do any event/packet *DURING* the start.
 *   - aka at least wait the next loop round (via timer or other)
 */
void	nlay_full_t::stack_start_closure()	throw()
{
	std::list<nlay_full_api_t *>::iterator	iter;
	// loop over the whole stack
	for( iter = full_api_db.begin(); iter != full_api_db.end(); iter++ ){
		nlay_full_api_t	*	full_api = *iter;
		// log to debug
		KLOG_DBG("Start closure for " << neoip_cpp_typename(*full_api));
		// start the closure this layer
		full_api->start_closure();
	}	
}	

NEOIP_NAMESPACE_END


