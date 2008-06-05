/*! \file
    \brief Header of the \ref kad_db_t

*/


#ifndef __NEOIP_KAD_DB_HPP__ 
#define __NEOIP_KAD_DB_HPP__ 
/* system include */
#include <map>
/* local include */
#include "neoip_kad_db_profile.hpp"
#include "neoip_kad_db_wikidbg.hpp"
#include "neoip_kad_id.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_peer_t;
class	kad_rec_t;
class	kad_recdups_t;
class	kad_addr_t;
class	kad_db_replicate_t;
class	kad_db_republish_t;

/** \brief Database for kademlia to store the published records
 */
class kad_db_t : NEOIP_COPY_CTOR_DENY, private wikidbg_obj_t<kad_db_t, kad_db_wikidbg_init> {
private:
	kad_peer_t *		kad_peer;		//!< backpointer to the kad_peer


	/*************** record database	*******************************/
	class 						rec_t;		//!< nested class to store record
	typedef std::multimap<kad_targetid_t,rec_t *>	distid_db_t;
	typedef std::map<kad_recid_t, rec_t *>		recid_db_t;
	distid_db_t	distid_db;	//!< record database indexed by the distance between their
					//!< key and the local peerid.
	recid_db_t	recid_db;	//!< record database indexed by their recid
	size_t		db_size;	//!< precomputed value of the sum of all the record payload size
	/*************** function to link rec_t to this database	*******/
	void	rec_dolink(rec_t *rec)	throw();
	void	rec_unlink(rec_t *rec)	throw();

	/*************** Replication	***************************************/
	kad_db_replicate_t *	db_replicate;	//!< service for the replication of record
	kad_db_republish_t *	db_republish;	//!< service for the republication of record
	
	/*************** utility function	*******************************/
	kad_targetid_t		get_distid(const kad_targetid_t &targetid)		const throw();
	kad_db_t::distid_db_t::iterator	get_iter_from_ptr(kad_db_t::rec_t *rec_ptr)	throw();
public:
	/*************** ctor/dtor	***************************************/
	kad_db_t(kad_peer_t *kad_peer)	throw();
	~kad_db_t()			throw();

	/*************** Query Function	***************************************/
	//! return true is this kad_db_t is the local_db in the kad_peer_t	
	bool			is_local_db()	const throw()	{ return !is_remote_db();		}
	bool			is_remote_db()	const throw();
	const kad_db_profile_t &get_profile()	const throw();
	
	/*************** Database Management	*******************************/
	bool		may_accept(const kad_rec_t &kad_rec)		const throw();	
	bool		may_accept(const kad_recdups_t &kad_recdups)	const throw();	
	void		update(const kad_rec_t &kad_rec)		throw();
	void		update(const kad_recdups_t &kad_recdups)	throw();
	void		remove(const kad_recid_t &recid)		throw();	
	bool		contain(const kad_recid_t &recid)		const throw();
	bool		contain(const kad_keyid_t &keyid)		const throw();
	kad_recdups_t	find_by_keyid(const kad_keyid_t &keyid
					, const kad_recid_t &recid_gt = kad_recid_t())	const throw();
	kad_keyid_t	get_next_keyid(const kad_keyid_t &keyid)	const throw();
	
	void		notify_new_node(const kad_addr_t &kad_addr)		throw();
	kad_err_t	replicate_in(const kad_recdups_t &kad_recdups, const kad_addr_t &dest_node)throw();

	/*************** List of friend function	***********************/
	friend	class	kad_db_wikidbg_t;
	friend	class	kad_db_rec_wikidbg_t;	
	friend	class	kad_db_republish_t;
	friend	class	kad_db_republish_wikidbg_t;	
	friend	class	kad_db_replicate_t;
	friend	class	kad_db_replicate_wikidbg_t;	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_DB_HPP__  */










