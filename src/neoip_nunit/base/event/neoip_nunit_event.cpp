/*! \file
    \brief Implementation of the nunit_event_t
*/

/* system include */
#include <iostream>

/* local include */
#include "neoip_nunit_event.hpp"
#include "neoip_pkt.hpp"
#include "neoip_log.hpp"

NEOIP_NAMESPACE_BEGIN

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       CTOR/DTOR
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief constructor
 */
nunit_event_t::nunit_event_t() throw()
{
	// set the default value of the type_val
	type_val = NONE;
}

/** \brief desstructor
 */
nunit_event_t::~nunit_event_t() throw()
{
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
//                       OSTREAM redirection
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

/** \brief ostream redirection
 */
std::ostream & operator << (std::ostream & os, const nunit_event_t &event) throw()
{
	switch( event.type_val ){
	case nunit_event_t::NONE:
		os << "NONE";
		break;
	case nunit_event_t::TEST_BEGIN:
		os << "TEST_BEGIN";
		break;	
	case nunit_event_t::TEST_END:
		os << "TEST_END (" << event.get_test_end_result() << ")";
		break;	
	case nunit_event_t::TEST_NOMORE:
		os << "TEST_NOMORE (" << event.get_test_nomore_result() << ")";
		break;	
	default: 	DBG_ASSERT(0);
	}
	// return the stream itself
	return os;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    TEST_BEGIN
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is TEST_BEGIN, false otherwise
 */
bool	nunit_event_t::is_test_begin() const throw()
{
	return type_val == TEST_BEGIN;
}

/** \brief Build a nunit_event_t to TEST_BEGIN
 */
nunit_event_t nunit_event_t::build_test_begin()					throw()
{
	nunit_event_t	nunit_event;
	// set the type_val
	nunit_event.type_val	= TEST_BEGIN;
	// return the built object
	return nunit_event;
}


//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    TEST_END
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is TEST_END, false otherwise
 */
bool	nunit_event_t::is_test_end() const throw()
{
	return type_val == TEST_END;
}

/** \brief Build a nunit_event_t to TEST_END
 */
nunit_event_t nunit_event_t::build_test_end(const nunit_res_t &nunit_res) throw()
{
	nunit_event_t	nunit_event;
	// set the type_val
	nunit_event.type_val	= TEST_END;
	nunit_event.nunit_res	= nunit_res;
	// return the built object
	return nunit_event;
}

/** \brief return the testname when TEST_END
 */
const nunit_res_t &	nunit_event_t::get_test_end_result() const throw()
{
	// sanity check - the event MUST be TEST_END
	DBG_ASSERT( is_test_end() );
	// return the nunit_res_t
	return nunit_res;
}



//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////
//                    TEST_NOMORE
//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

/** \brief return true is the type is TEST_NOMORE, false otherwise
 */
bool	nunit_event_t::is_test_nomore() const throw()
{
	return type_val == TEST_NOMORE;
}

/** \brief Build a nunit_event_t to TEST_NOMORE
 */
nunit_event_t nunit_event_t::build_test_nomore(const nunit_err_t &nunit_err)	throw()
{
	nunit_event_t	nunit_event;
	// set the type_val
	nunit_event.type_val	= TEST_NOMORE;
	nunit_event.nunit_err	= nunit_err;
	// return the built object
	return nunit_event;
}

/** \brief return the nunit_err_t
 */
const nunit_err_t &	nunit_event_t::get_test_nomore_result() const throw()
{
	// sanity check - the event MUST be TEST_NOMORE
	DBG_ASSERT( is_test_nomore() );
	// return the nunit_err_t
	return nunit_err;
}

NEOIP_NAMESPACE_END

