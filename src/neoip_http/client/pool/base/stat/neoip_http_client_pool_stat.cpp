/*! \file
    \brief Definition of the \ref http_client_pool_stat_t


*/

/* system include */
#include <sstream>
/* local include */
#include "neoip_http_client_pool_stat.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        ctor/dtor
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Constructor
 */
http_client_pool_stat_t::http_client_pool_stat_t()	throw()
{
	// set the initial value
	nb_cnx_peak		( 0 );
	nb_cnx_put		( 0 );
	nb_cnx_get		( 0 );
	nb_cnx_died_internal	( 0 );
	nb_cnx_died_external	( 0 );
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                        display function
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief convert the object to a string
 */
std::string	http_client_pool_stat_t::to_string()				const throw()
{
	std::ostringstream	oss;
	// build the string to return
	oss        << "nb_cnx_peak="		<< nb_cnx_peak();
	oss << " " << "nb_cnx_put="		<< nb_cnx_put();
	oss << " " << "nb_cnx_get="		<< nb_cnx_get();
	oss << " " << "nb_cnx_died_internal="	<< nb_cnx_died_internal();
	oss << " " << "nb_cnx_died_external="	<< nb_cnx_died_external();
	
	// return the just built string
	return oss.str();
}

NEOIP_NAMESPACE_END

