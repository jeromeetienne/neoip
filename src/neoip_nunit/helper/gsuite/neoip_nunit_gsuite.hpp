/*! \file
    \brief Header of the nunit_gsuite_t
*/


#ifndef __NEOIP_NUNIT_GSUITE_HPP__ 
#define __NEOIP_NUNIT_GSUITE_HPP__ 
/* system include */
#include <map>
/* local include */
#include "neoip_namespace.hpp"

NEOIP_NAMESPACE_BEGIN;

// list of forward declaration
class	nunit_suite_t;

/** \brief Type for the populate_fct_t of nunit_gsuite_t
 */
typedef void (*nunit_gsuite_fct_t)(nunit_suite_t &nunit_suite);

/** \brief class to store and run nunit_gsuite_fct_t
 */
class nunit_gsuite_t {
private:
	std::multimap<int, nunit_gsuite_fct_t>	fct_db;	//!< contains all the nunit_gsuite_fct_t
public:
	/**************** utility function	*******************************/
	void	insert(int index, nunit_gsuite_fct_t gsuite_fct)	throw();
	void	populate(nunit_suite_t &nunit_suite)			throw();
	
	//! return the number of nunit_gsuite_fct_t
	size_t	size()	const throw()	{ return fct_db.size();	}
};

/** \brief Insert a nunit_gsuite_fct_t in nunit_gsuite_t during the initialization of static
 * 
 * - The whole purpose of nunit_gsuite_inserter_t is to use the dummy ctor trick
 *   to 'fix' the classic ordering issue of static ctor in c++.
 * - nunit_gsuite_inserter_t is a phony object used to execute code during
 *   the initialization of 'static' before reaching 'main()'
 * - it uses a dummy constructor 
 * - the nunit_gsuite_inserter_t has no other purposes
 */
class nunit_gsuite_inserter_t {
public:
	/** \brief constructor
	 */
	nunit_gsuite_inserter_t(nunit_gsuite_t **nunit_gsuite, int index, nunit_gsuite_fct_t gsuite_fct)
										throw()
	{
		// if the nunit_gsuite_t is not yet created, create it now
		if( *nunit_gsuite == NULL )	*nunit_gsuite	= new nunit_gsuite_t();
		// insert this gsuite_fct/index in the nunit_gsuite
		(*nunit_gsuite)->insert(index, gsuite_fct);
	}
};

/** \brief Define to insert a nunit_gsuite_fct_t into a nunit_gsuite_t at initialization time
 * 
 * - As the name of the variable is useless anyway - some funky stuff to create a unique name.
 *   - NOTE: i tried some anonymous namespace stuff... but i failed... not sure if it is supposed
 *     to work or if it is me doing it wrong... from the info i got, it is not supposed to 
 *     work anyway
 */
#define NUNIT_GSUITE_INSERTER(GSUITE, INDEX, FCT)				\
	nunit_gsuite_inserter_t	neoip_gsuite_inserter_ ## GSUITE ## _ ## INDEX ## _ ## FCT(&GSUITE, INDEX, FCT)

//! helper for the declaration a nunit_gsuite_t
#define NUNIT_GSUITE_DECLARATION(GSUITE)	extern nunit_gsuite_t *GSUITE
//! helper for the definition a nunit_gsuite_t
#define NUNIT_GSUITE_DEFINITION(GSUITE)		nunit_gsuite_t *GSUITE	= NULL

	
NEOIP_NAMESPACE_END

#endif	/* __NEOIP_NUNIT_GSUITE_HPP__  */



