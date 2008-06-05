/*! \file
    \brief Header of the \ref kad_db_republish_t

*/


#ifndef __NEOIP_KAD_DB_REPUBLISH_HPP__ 
#define __NEOIP_KAD_DB_REPUBLISH_HPP__ 
/* system include */
#include <list>
/* local include */
#include "neoip_kad_db_republish_wikidbg.hpp"
#include "neoip_kad_store_cb.hpp"
#include "neoip_kad_err.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration
class	kad_db_t;
class	kad_rec_t;


/** \brief to handle the republication of the kad_rec_t in the kad_db_t
 */
class kad_db_republish_t : NEOIP_COPY_CTOR_DENY, private kad_store_cb_t
			, private wikidbg_obj_t<kad_db_republish_t, kad_db_republish_wikidbg_init>
			{
private:
	kad_db_t *	kad_db;		//!< backpointer to the kad_db_t

	/*************** Republication	***************************************/
	std::list<kad_store_t *>	kad_store_db;
	bool 				neoip_kad_store_cb(void *cb_userptr, kad_store_t &cb_kad_store
								, const kad_event_t &kad_event)	throw();
public:
	/*************** ctor/dtor	***************************************/
	kad_db_republish_t(kad_db_t *kad_db)	throw();
	~kad_db_republish_t()			throw();

	kad_err_t	republish_rec(const kad_rec_t &kad_rec)			throw();

	/*************** List of friend function	***********************/
	friend	class	kad_db_republish_wikidbg_t;
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_DB_REPUBLISH_HPP__  */










