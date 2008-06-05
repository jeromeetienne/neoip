/*! \file
    \brief Header of the \ref kad_nsearch_t
    
*/


#ifndef __NEOIP_KAD_NSEARCH_LLIST_HPP__ 
#define __NEOIP_KAD_NSEARCH_LLIST_HPP__ 
/* system include */
#include <set>
/* local include */
#include "neoip_kad_nsearch_llist_wikidbg.hpp"
#include "neoip_kad_nsearch.hpp"
#include "neoip_kad_addr.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_nsearch_lnode_t;
class	kad_peer_t;
class	kad_addr_arr_t;

/** \brief class definition for id
 */
class kad_nsearch_llist_t : NEOIP_COPY_CTOR_DENY
			, private wikidbg_obj_t<kad_nsearch_llist_t, kad_nsearch_llist_wikidbg_init> {
private:
	kad_nsearch_t *	kad_nsearch;		//!< backpointer for this kad_nsearch
	size_t		nb_succ_useless_query;	//!< counts the number of successive query
						//!< which failed to "a node any closer than
						//!< the closest already seen"
	size_t		findsome_max_nb_rec;
	kad_addr_t	caching_candidate;	//!< last see caching_candidate (used only
						//!< is query_type is FINDSOMEVAL and 
						//!< caching_enable())
	
	// to store the node learned by the query
	typedef std::map<kad_targetid_t, kad_nsearch_lnode_t *>	lnode_db_t;
	lnode_db_t	lnode_db;	//!< the database of learned nodes indexed
					//!< by their distance to the target_id
	void	lnode_link(kad_nsearch_lnode_t *lnode)		throw();
	void	lnode_unlink(kad_nsearch_lnode_t *lnode)	throw();
	bool	lnode_present(const kad_peerid_t &peerid)	const throw();

// TODO find better name for those function
	bool			nsearch_is_completed()					const throw();
	std::set<kad_targetid_t>get_nclosest_distid()					const throw();
	bool			merge_addr_arr(const kad_addr_arr_t &kad_addr_arr)	throw();
	void			launch_queries(size_t max_nb_query)			throw();
	kad_nsearch_lnode_t *	get_next_lnode_to_query()				const throw();
	size_t			get_nb_querying_node()					const throw();

	/*************** result notification	*******************************/
	bool			notify_end_result()		throw();
	bool			notify_findnode_result()	throw();
	bool			notify_findsomeval_result()	throw();
public:
	/*************** ctor/dtor	***************************************/
	kad_nsearch_llist_t(kad_nsearch_t *kad_nsearch)	throw();
	~kad_nsearch_llist_t()				throw();

	/*************** List of friend classes	*******************************/
	friend class	kad_nsearch_llist_wikidbg_t;
	friend class	kad_nsearch_t;
	friend class	kad_nsearch_lnode_t;	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_NSEARCH_LLIST_HPP__  */










