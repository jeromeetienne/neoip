/*! \file
    \brief Header of the \ref kad_db_t
    
*/


#ifndef __NEOIP_KAD_DB_REC_HPP__ 
#define __NEOIP_KAD_DB_REC_HPP__ 
/* system include */
/* local include */
#include "neoip_kad_db_rec_wikidbg.hpp"
#include "neoip_kad_db.hpp"
#include "neoip_kad_rec.hpp"
#include "neoip_timeout.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// list of forward declaration

/** \brief to store a record in the 
 */
class kad_db_t::rec_t : NEOIP_COPY_CTOR_DENY, private timeout_cb_t
				, private wikidbg_obj_t<kad_db_t::rec_t, kad_db_rec_wikidbg_init>
				{
private:
	kad_db_t *	kad_db;		//!< backpointer to the kad_db_t
	kad_rec_t	kad_rec;	//!< the record itself
	
	/*************** republish timeout handling	***********************/
	timeout_t	republish_timeout;	//!< the timeout which trigger the republish
	bool		republish_timeout_cb()	throw();
	delay_t		cpu_republish_delay()	const throw();

	/*************** expire timeout handling	***********************/
	timeout_t	expire_timeout;		//!< the timeout which trigger the expiration
	bool		expire_timeout_cb()	throw();
	delay_t		cpu_expire_delay()	const throw();
	
	/*************** timeout_cb_t	***************************************/
	bool		neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
public:
	/*************** ctor/dtor	***************************************/
	rec_t(kad_db_t *kad_db, const kad_rec_t &kad_rec)	throw();
	~rec_t()						throw();
	
	/*************** query function	***************************************/	
	kad_rec_t		get_kad_rec()	const throw();
	const kad_recid_t &	get_recid()	const throw()	{ return kad_rec.get_recid();	}
	const kad_keyid_t &	get_keyid()	const throw()	{ return kad_rec.get_keyid();	}
	const datum_t &		get_payload()	const throw()	{ return kad_rec.get_payload();	}

	/*************** List of friend class	*******************************/
	friend	class	kad_db_rec_wikidbg_t;
	
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_KAD_DB_REC_HPP__  */










