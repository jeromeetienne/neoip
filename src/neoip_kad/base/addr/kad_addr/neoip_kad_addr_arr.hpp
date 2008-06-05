/*! \file
    \brief Header of the \ref kad_addr_arr_t
    
*/


#ifndef __NEOIP_KAD_ADDR_ARR_HPP__ 
#define __NEOIP_KAD_ADDR_ARR_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_addr.hpp"
#include "neoip_item_arr.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;


NEOIP_ITEM_ARR_DECLARATION_START(kad_addr_arr_t, kad_addr_t);
private:/** \brief Functor for sort_by_dist()
	 */
	struct cmp_xor_dist_t : public std::binary_function<kad_addr_t, kad_addr_t, bool> {
		private:kad_peerid_t	from_id;
		public:	cmp_xor_dist_t(const kad_peerid_t &from_id) : from_id(from_id) {}
			bool operator()(const kad_addr_t &node1, const kad_addr_t &node2){
				kad_peerid_t dist1	= from_id ^ node1.get_peerid();
				kad_peerid_t dist2	= from_id ^ node2.get_peerid();
				return dist1 < dist2;
			}
	};
public:
	/** \brief Sort this kad_addr_arr according to the distance XOR metric of the paper
	 * 
	 * - used in the "get the n closest of FINDNODE"
	 */
	kad_addr_arr_t &	sort_by_dist(const kad_peerid_t &from_id)	throw()
	{
		std::vector<kad_addr_t> & item_db = item_arr_t<kad_addr_t>::item_db;
		std::sort(item_db.begin(), item_db.end(), cmp_xor_dist_t(from_id));
		return *this;
	}
NEOIP_ITEM_ARR_DECLARATION_END(kad_addr_arr_t, kad_addr_t);

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_ADDR_ARR_HPP__  */










