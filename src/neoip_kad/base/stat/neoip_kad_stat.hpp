/*! \file
    \brief Header of the \ref kad_stat_t
    
*/


#ifndef __NEOIP_KAD_STAT_HPP__ 
#define __NEOIP_KAD_STAT_HPP__ 
/* system include */
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

/** \brief class definition for kad's statictic
 */
class kad_stat_t {
public:
	uint64_t	nb_ping_sent		, nb_ping_recv;
	uint64_t	nb_store_sent		, nb_store_recv;
	uint64_t	nb_findnode_sent	, nb_findnode_recv;
	uint64_t	nb_findsomeval_sent	, nb_findsomeval_recv;
	uint64_t	nb_findallval_sent	, nb_findallval_recv;
	uint64_t	nb_delete_sent		, nb_delete_recv;
	
	uint64_t	nb_byte_sent		, nb_byte_recv;
public:
	kad_stat_t()	throw();
	
	void		reset()	throw();
};


NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_STAT_HPP__  */










