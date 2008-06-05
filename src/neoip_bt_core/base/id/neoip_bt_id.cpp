/*! \file
    \brief Definition of the \ref bt_id_t

*/


/* system include */
/* local include */
#include "neoip_bt_id.hpp"
#include "neoip_bt_peerid_helper.hpp"

NEOIP_NAMESPACE_BEGIN;

////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//                        peerid helper
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

/** \brief Return the progfull encoded in this bt_id_t peerid
 */
std::string	bt_id_t::peerid_progfull()	const throw()
{
	return peerid_progname() + " " + peerid_progvers();
}


/** \brief Return the progname encoded in this bt_id_t peerid
 */
std::string	bt_id_t::peerid_progname()	const throw()
{
	return bt_peerid_helper_t::parse_progname(*this);
}

/** \brief Return the progvers encoded in this bt_id_t peerid
 */
std::string	bt_id_t::peerid_progvers()	const throw()
{
	return bt_peerid_helper_t::parse_progvers(*this);
}

/** \brief Build a bt_id_t to appears as a peerid from the progname/progvers
 */
bt_id_t		bt_id_t::build_peerid(const std::string &progname, const std::string &progvers)	throw()
{
	return bt_peerid_helper_t::build(progname, progvers);
}
	
NEOIP_NAMESPACE_END

