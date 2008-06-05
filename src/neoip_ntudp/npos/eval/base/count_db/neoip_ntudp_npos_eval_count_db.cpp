/*! \file
    \brief Definition of the ntudp_npos_eval_t::count_db_t


*/

/* system include */
/* local include */
#include "neoip_ntudp_npos_eval_count_db.hpp"
#include "neoip_ntudp_npos_inetreach.hpp"
#include "neoip_ntudp_npos_saddrecho.hpp"
#include "neoip_ntudp_npos_natlback.hpp"
#include "neoip_ntudp_pserver_pool.hpp"
#include "neoip_log.hpp"
#include "neoip_nipmem_alloc.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
ntudp_npos_eval_t::count_db_t::count_db_t(const ntudp_npos_eval_t *npos_eval)	throw()
{
	this->npos_eval	= npos_eval;
}

/** \brief Desstructor
 */
ntudp_npos_eval_t::count_db_t::~count_db_t()		throw()
{
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  intersection with some reachability states
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Remove all the counted address which are NOT reachable
 */
ntudp_npos_eval_t::count_db_t &ntudp_npos_eval_t::count_db_t::intersect_reach()	throw()
{
	ntudp_pserver_pool_t *				pserver_pool	= npos_eval->pserver_pool;
	std::map<ipport_addr_t, size_t>::iterator	iter;
	std::map<ipport_addr_t, size_t>			tmp;
	// go thru the addr_db
	for( iter = addr_db.begin(); iter != addr_db.end(); iter++ ){
		// if this address is not reachable, skip it
		if( !pserver_pool->reach_contain(iter->first) )	continue;
		// else add it to the temporary copy
		bool	succeed	= tmp.insert(*iter).second;
		DBG_ASSERT( succeed );
	}
	// copy the temporary to the actual database
	addr_db	= tmp;
	// return the object itself
	return *this;
}

/** \brief Remove all the counted address which are NOT unknown
 */
ntudp_npos_eval_t::count_db_t &ntudp_npos_eval_t::count_db_t::intersect_unknown()	throw()
{
	ntudp_pserver_pool_t *				pserver_pool	= npos_eval->pserver_pool;
	std::map<ipport_addr_t, size_t>::iterator	iter;
	std::map<ipport_addr_t, size_t>			tmp;
	// go thru the addr_db
	for( iter = addr_db.begin(); iter != addr_db.end(); iter++ ){
		// if this address is not unknown, skip it
		if( !pserver_pool->unknown_contain(iter->first) )	continue;
		// else add it to the temporary copy
		bool	succeed	= tmp.insert(*iter).second;
		DBG_ASSERT( succeed );
	}	
	// copy the temporary to the actual database
	addr_db	= tmp;
	// return the object itself
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                 some 'union' with the pserver_pool_t state
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief make sure all reachable addresses are in count_db_t
 * 
 * - if one address is not present, create it
 */
ntudp_npos_eval_t::count_db_t &ntudp_npos_eval_t::count_db_t::union_reach()	throw()
{
	ntudp_pserver_pool_t *				pserver_pool	= npos_eval->pserver_pool;
	std::map<ipport_addr_t, size_t>::iterator	iter;
	// go thru the reachable database
	for(size_t i = 0; i < pserver_pool->reach_size(); i++ ){
		const ipport_addr_t &	pserver_addr	 = pserver_pool->reach_at(i);
		// find this address in the addr_db
		iter	= addr_db.find(pserver_addr);
		// if this address is already in the addr_db, goto the next
		if( iter != addr_db.end() )	continue;
		// if not yet present in addr_db, create it
		bool	succeed	= addr_db.insert(std::make_pair(pserver_addr, 0)).second;
		DBG_ASSERT( succeed );
	}
	// return the object itself
	return *this;
}

/** \brief make sure all unknown addresses are in count_db_t
 * 
 * - if one address is not present, create it with a 0 count
 */
ntudp_npos_eval_t::count_db_t &ntudp_npos_eval_t::count_db_t::union_unknown()	throw()
{
	ntudp_pserver_pool_t *				pserver_pool	= npos_eval->pserver_pool;
	std::map<ipport_addr_t, size_t>::iterator	iter;
	// go thru the reachable database
	for(size_t i = 0; i < pserver_pool->unknown_size(); i++ ){
		const ipport_addr_t &	pserver_addr	 = pserver_pool->unknown_at(i);
		// find this address in the addr_db
		iter	= addr_db.find(pserver_addr);
		// if this address is already in the addr_db, goto the next
		if( iter != addr_db.end() )	continue;
		// if not yet present in addr_db, create it
		bool	succeed	= addr_db.insert(std::make_pair(pserver_addr, 0)).second;
		DBG_ASSERT( succeed );
	}
	// return the object itself
	return *this;
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                      misc function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


/** \brief count one address
 */
ntudp_npos_eval_t::count_db_t &
	ntudp_npos_eval_t::count_db_t::count_one_address(const ipport_addr_t &addr) throw()
{
	std::map<ipport_addr_t, size_t>::iterator	iter = addr_db.find(addr);
	// if the element is not already in the database, create it
	if( iter == addr_db.end() ){
		bool	succeed	= addr_db.insert(std::make_pair(addr, 0)).second;
		DBG_ASSERT( succeed );
		// reinit the iterator on the value
		iter = addr_db.find(addr);
	}
	// now increase the counter for this address.
	iter->second++;
	// return the object itself
	return *this;	
}

/** \brief get least counted address
 */
ipport_addr_t ntudp_npos_eval_t::count_db_t::get_least_counted()	const throw()
{
	std::map<ipport_addr_t, size_t>::const_iterator	iter;
	std::map<ipport_addr_t, size_t>::const_iterator	least	= addr_db.end();
	// if the addr_db is empty, return a null ipport_addr_t
	if( addr_db.empty() )	return ipport_addr_t();

	// got thru the whole addr_db
	for(iter = addr_db.begin(); iter != addr_db.end(); iter++ ){
		// if the least member has never been set or is more that current member=> least = current
		if( least == addr_db.end() || least->second > iter->second )
			least = iter;
	}
	// return the least_used_addr	
	return least->first;	
}

/** \brief get the sum of all counts
 */
size_t ntudp_npos_eval_t::count_db_t::get_sum_count()	const throw()
{
	std::map<ipport_addr_t, size_t>::const_iterator	iter;
	size_t	sum = 0;
	// got thru the whole addr_db
	for(iter = addr_db.begin(); iter != addr_db.end(); iter++ )
		sum	+= iter->second;
	// return the sum of all count
	return sum;
}

/** \brief Remove all the element of other in this count_db_t
 */
ntudp_npos_eval_t::count_db_t &ntudp_npos_eval_t::count_db_t::exclude(const count_db_t &other)
										throw()
{
	std::map<ipport_addr_t, size_t>::const_iterator	iter;
	// got thru the other addr_db
	for(iter = other.addr_db.begin(); iter != other.addr_db.end(); iter++){
		// try to find this element in the local addr_db
		std::map<ipport_addr_t, size_t>::iterator	local_iter = addr_db.find(iter->first);
		// if it is present, remove it
		if( local_iter != addr_db.end() )	addr_db.erase(local_iter);
	}
	// return the object itself
	return *this;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                     function to count each type of client
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Add all the inetreach clients to this count_db_t
 */
ntudp_npos_eval_t::count_db_t & ntudp_npos_eval_t::count_db_t::count_all_inetreach()	throw()
{
	std::set<ntudp_npos_inetreach_t *>::iterator	iter;
	// count the inetreach client
	for(iter = npos_eval->inetreach_db.begin(); iter != npos_eval->inetreach_db.end(); iter++)
		count_one_address( (*iter)->get_pserver_addr() );
	// return the object itself
	return *this;	
}

/** \brief Add all the natted clients to this count_db_t
 */
ntudp_npos_eval_t::count_db_t & ntudp_npos_eval_t::count_db_t::count_all_natted()	throw()
{
	std::set<ntudp_npos_saddrecho_t *>::iterator	iter;
	// count the inetreach client
	for(iter = npos_eval->natted_db.begin(); iter != npos_eval->natted_db.end(); iter++)
		count_one_address( (*iter)->get_pserver_addr() );
	// return the object itself
	return *this;	
}

/** \brief Add all the natsym clients to this count_db_t
 */
ntudp_npos_eval_t::count_db_t & ntudp_npos_eval_t::count_db_t::count_all_natsym()	throw()
{
	std::set<ntudp_npos_saddrecho_t *>::iterator	iter;
	// count the inetreach client
	for(iter = npos_eval->natsym_db.begin(); iter != npos_eval->natsym_db.end(); iter++)
		count_one_address( (*iter)->get_pserver_addr() );
	// return the object itself
	return *this;	
}

/** \brief Add all the natlback clients to this count_db_t
 */
ntudp_npos_eval_t::count_db_t & ntudp_npos_eval_t::count_db_t::count_all_natlback()	throw()
{
	std::set<ntudp_npos_natlback_t *>::iterator	iter;
	// count the inetreach client
	for(iter = npos_eval->natlback_db.begin(); iter != npos_eval->natlback_db.end(); iter++)
		count_one_address( (*iter)->get_pserver_addr() );
	// return the object itself
	return *this;	
}

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                  to_string() function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Convert the object to a string
 */
std::string	ntudp_npos_eval_t::count_db_t::to_string()	const throw()
{
	std::ostringstream oss;
	// handle the null case
	if( empty() )			return "empty";
	// build the string
	std::map<ipport_addr_t, size_t>::const_iterator	iter;
	for( iter = addr_db.begin(); iter != addr_db.end(); iter++ ){
		if( iter != addr_db.begin() )	oss << ":";
		oss << iter->first << "(" << iter->second << ")";
	}	
	// return a string	
	return oss.str();	
}


NEOIP_NAMESPACE_END


