/*! \file
    \brief Header of the \ref cookie_db_t.cpp

*/


#ifndef __NEOIP_COOKIE_HPP__ 
#define __NEOIP_COOKIE_HPP__ 

/* system include */
#include <set>
/* local include */
#include "neoip_cookie_id.hpp"
#include "neoip_datum.hpp"
#include "neoip_timeout.hpp"
#include "neoip_assert.hpp"
#include "neoip_copy_ctor_checker.hpp"
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN

// TODO put that in a profile
//! The amount of time between 2 cookie's secret renewal
const	delay_t		COOKIE_SECRET_RENEWAL_PERIOD	= delay_t::from_sec(2*60);

/** \brief class to handle cookie
 */
class cookie_db_t : NEOIP_COPY_CTOR_ALLOW, public timeout_cb_t {
private:
	/*************** Current stuff	***************************************/
	std::set<cookie_id_t> 	cur_accepted_db;
	datum_t			cur_secret;	//!< local secret to produce cookies.


	/*************** Old stuff - for smooth transition	***************/
	std::set<cookie_id_t>	old_accepted_db;
	datum_t			old_secret;

	/*************** Rekey timeout	***************************************/
	timeout_t		rekey_timeout;		//!< time to rekey
	bool			neoip_timeout_expire_cb(void *userptr, timeout_t &cb_timeout)	throw();
	
	cookie_id_t		cpu(const datum_t &pathid, const datum_t &secret)		const throw();
public:
	/*************** ctor/dtor	***************************************/
	cookie_db_t(const delay_t &renewal_period = COOKIE_SECRET_RENEWAL_PERIOD)		throw();
	~cookie_db_t()		throw();
	
	/*************** action function	*******************************/
	cookie_id_t	generate(const datum_t &pathid)					const throw();
	bool		check(const datum_t &pathid, const cookie_id_t &cookie_id)	const throw();
	void		accept(const datum_t &pathid, const cookie_id_t &cookie_id)	throw();
};

NEOIP_NAMESPACE_END

#endif	/* __NEOIP_COOKIE_HPP__  */



