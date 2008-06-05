/*! \file
    \brief Declaration of the ntudp_npos_eval_t::count_db_t
    
*/


#ifndef __NEOIP_NTUDP_NPOS_EVAL_COUNT_DB_HPP__ 
#define __NEOIP_NTUDP_NPOS_EVAL_COUNT_DB_HPP__ 
/* system include */
/* local include */
#include "neoip_ntudp_npos_eval.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

/** \brief This is a special object only here to make all the scheduling function
 *         a lot clearer in the ntudp_npos_eval_t
 */
class ntudp_npos_eval_t::count_db_t : NEOIP_COPY_CTOR_ALLOW {
private:
	std::map<ipport_addr_t, size_t>	addr_db;//!< the database hold the count for each ipport_addr_t.
	const ntudp_npos_eval_t *npos_eval;	//!< backpointer on the ntudp_npos_eval_t on which it 
						//!< operate.
public:
	/*************** ctor/dtor	***************************************/
	count_db_t(const ntudp_npos_eval_t *npos_eval)	throw();
	~count_db_t()					throw();
	
	/*************** basic function	***************************************/
	size_t		size()		const throw()	{ return addr_db.size();	}
	bool		empty()		const throw()	{ return addr_db.empty();	}
	
	count_db_t &	count_one_address(const ipport_addr_t &addr)	throw();	
	count_db_t &	exclude(const count_db_t &other)		throw();
	ipport_addr_t	get_least_counted()				const throw();
	size_t		get_sum_count()					const throw();

	/*************** to count the clients type	***********************/
	count_db_t &	count_all_inetreach()		throw();
	count_db_t &	count_all_natted()		throw();
	count_db_t &	count_all_natsym()		throw();
	count_db_t &	count_all_natlback()		throw();

	/*************** to intersect with the ntudp_pserver_pool_t ***********/
	count_db_t &	intersect_reach()		throw();
	count_db_t &	intersect_unknown()		throw();
	
	count_db_t &	union_reach()		throw();
	count_db_t &	union_unknown()		throw();
	
	/*************** display function	*******************************/
	std::string	to_string()							const throw();
	friend	std::ostream & operator << (std::ostream & os
					, const ntudp_npos_eval_t::count_db_t &count_db )	throw()
						{ return os << count_db.to_string();	}
};



NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NTUDP_NPOS_EVAL_COUNT_DB_HPP__  */


