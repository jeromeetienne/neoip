/*! \file
    \brief Definition of the nunit_gsuite_t

\par Brief Description
nunit_gsuite_t is a modules made to globalize the creation of nunit_suite_t.
Thus it is supposed to be easier to create/manage the test suite for the 
whole project.

\par Implementation Notes
The \ref nunit_gsuite_t stuff are used to store all nunit_suite in a global
manner. It is just a helper for the nunit stuff.
Usually i tend to avoid global variable but i considere this case as 
an exception for the following reasons:
-# unit testing are special as it is not part of the 'production code'
-# to have global nunit_suite_t allows an easy management of it
   - e.g. if the nunit_suite_t for a given module is not linked or is added
     the global feature allow it to be automatically added/removed from
     the unittesting.

- The implementation in itself is a bunch of functions (nunit_gsuite_fct_t)
  pointer stored in a multimap indexed by an integer.

*/

/* system include */
/* local include */
#include "neoip_nunit_gsuite.hpp"

NEOIP_NAMESPACE_BEGIN;


////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                       Misc function
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Insert a nunit_gsuite_fct_t into this nunit_gsuite_t at the index position
 */
void	nunit_gsuite_t::insert(int index, nunit_gsuite_fct_t gsuite_fct)	throw()
{
	// add this gsuite_fctinto the fct_db
	fct_db.insert(std::make_pair(index, gsuite_fct));	
}


/** \brief Populate a nunit_suite_t with all the nunit_gsuite_fct_t using the index increasing order
 */
void	nunit_gsuite_t::populate(nunit_suite_t &nunit_suite)		throw()
{
	std::multimap<int, nunit_gsuite_fct_t>::iterator	iter;
	// go thru all the nunit_gsuite_fct_t of this nunit_gsuite_t
	// - as the iterator is indexed by a int, the nunit_gsuite_fct_t are called in order of index
	for( iter = fct_db.begin(); iter != fct_db.end(); iter++ ){
		// call this nunit_gsuite_fct_t on the nunit_suite_t parameter
		iter->second(nunit_suite);
	}
}


NEOIP_NAMESPACE_END




